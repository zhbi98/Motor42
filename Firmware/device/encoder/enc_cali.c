/**
 * @file enc_cali.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "enc_cali.h"
#include "tb67h450.h"
#include "mt6816.h"
#include "romf103cb.h"
#include "log.h"

/*********************
 *      DEFINES
 *********************/

#define AUTO_SPEED 2U
#define FINE_SPEED 1U

/**********************
 *      TYPEDEFS
 **********************/

_cali_attr_t cali = {
    .state = STATE_IDLE,
    .errid = ERR_NO,
    ._target = 0,
    ._start = false,
    .raw_num = 0,
    .avg_cnt = 0,
    .errdata = 0,
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static int32_t _average(const uint16_t * data_p, uint16_t len, int32_t _cyc);
static int32_t _average2(int32_t a, int32_t b, int32_t _cyc);
static int32_t _subtract(int32_t a, int32_t b, int32_t _cyc);
static uint32_t _mod(uint32_t _a, uint32_t _b);
static void _state_idle_execute(_cali_attr_t * cali_p);
static void _state_fwd_ready_execute(_cali_attr_t * cali_p);
static void _state_fwd_start_execute(_cali_attr_t * cali_p);
static void _state_bwd_return_execute(_cali_attr_t * cali_p);
static void _state_bwd_gap_execute(_cali_attr_t * cali_p);
static void _state_bwd_start_execute(_cali_attr_t * cali_p);
static void _enc_cali_verify();

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Computes cyclic-aware average of buffered data with overflow handling.
 * @param data_p Pointer to circular buffer data array.
 * @param len Number of elements in the buffer (must be > 0).
 * @param _cyc Cycle period value for overflow compensation.
 * @return Averaged value within [0, _cyc) range
 */
static int32_t _average(const uint16_t * data_p, 
    uint16_t len, int32_t _cyc)
{
    int32_t _sum = 0;
    int32_t _diff = 0;
    int32_t _sub = 0;

    _sum += (int32_t)data_p[0];

    for (uint16_t i = 1; i < len; i++) {
        _diff = (int32_t)data_p[i];
        _sub = (int32_t)data_p[i] - (int32_t)data_p[0];
        if (_sub > (_cyc >> 1))
            _diff = (int32_t)data_p[i] - _cyc;
        if (_sub < (-_cyc >> 1))
            _diff = (int32_t)data_p[i] + _cyc;
        _sum += _diff;
    }

    _sum = _sum / len;

    if (_sum < 0) _sum += _cyc;
    if (_sum > _cyc) _sum -= _cyc;

    return _sum;
}

/**
 * Computes cyclic-aware average of two values with overflow compensation.
 * @param a First input value (32-bit integer).
 * @param b Second input value (32-bit integer).
 * @param _cyc Cycle modulus value (e.g., 360 for angles, 65536 for 16-bit rollover).
 * @return Averaged value within [0, _cyc) range.
 */
static int32_t _average2(int32_t a, 
    int32_t b, int32_t _cyc)
{
    int32_t _sub = 0;
    int32_t _avg = 0;

    _sub = a - b;
    _avg = (a + b) >> 1;

    if (abs(_sub) > (_cyc >> 1)) {
        if (_avg >= (_cyc >> 1))
            _avg -= (_cyc >> 1);
        else _avg += (_cyc >> 1);
    }

    return _avg;
}

/**
 * Computes cyclic-aware subtraction with overflow compensation.
 * @param a Minuend value (must be in [0, _cyc)).
 * @param b Subtrahend value (must be in [0, _cyc)).
 * @param _cyc Cycle period for overflow correction (e.g., 360° or 65536).
 * @return Corrected difference in [-_cyc/2, _cyc/2) range.
 */
static int32_t _subtract(int32_t a, 
    int32_t b, int32_t _cyc)
{
    int32_t _sub = 0;

    _sub = a - b;
    if (_sub > (_cyc >> 1))
        _sub -= _cyc;
    if (_sub < (-_cyc >> 1))
        _sub += _cyc;

    return _sub;
}

/**
 * Computes cyclic-safe modulus with wrap-around correction.
 * @param _a Input value (unsigned 32-bit integer).
 * @param  _b Modulus base (unsigned 32-bit integer).
 * @return Safe modulus result within [0, _b) range.
 */
static uint32_t _mod(uint32_t _a, uint32_t _b)
{
    return (_a + _b) % _b;
}

extern _angle_t _angle;

/**
 * State IDLE callback function, The Mag calibration program is idle.
 * @param cali_p pointer to an '_cali_attr_t' cali.
 * @return next state.
 */
static void _state_idle_execute(_cali_attr_t * cali_p)
{
    tb_foc_set_current_vector(cali_p->_target, 
        Current_Cali_Current);

    cali_p->state = STATE_FWD_READY;
    cali_p->_target = Move_Pulse_NUM;
}

/**
 * State forward ready callback function, prepare the forward auto-calibration encoder.
 * @param cali_p pointer to an '_cali_attr_t' cali.
 * @return next state.
 */
static void _state_fwd_ready_execute(_cali_attr_t * cali_p)
{
    uint32_t target = Move_Pulse_NUM * 2;

    tb_foc_set_current_vector(cali_p->_target, 
        Current_Cali_Current);

    cali_p->_target += AUTO_SPEED;

    if (cali_p->_target != target) return;

    cali_p->state = STATE_FWD_START;
    cali_p->_target = Move_Pulse_NUM;
}

/**
 * State forward start callback function, forward acquisition of encoder measurements.
 * @param cali_p pointer to an '_cali_attr_t' cali.
 * @return next state.
 */
static void _state_fwd_start_execute(_cali_attr_t * cali_p)
{
    uint32_t target = Move_Pulse_NUM * 2;

    /**
     * 由于第一次执行到此处就会采集数据，
     * 所以最终会采集 201 个数据，
     * 即数组下标 0-200。
     */
    if ((cali_p->_target % Move_Divide_NUM) == 0) {
        cali_p->rawbuf[cali_p->raw_num] = _angle.raw;
        cali_p->raw_num++;

        if (cali_p->raw_num == READ_CNT) {
            cali_p->forward[cali_p->avg_cnt] = \
                _average(cali_p->rawbuf, READ_CNT, RESOLUTION);

            cali_p->avg_cnt++;
            cali_p->raw_num = 0;
            cali_p->_target += FINE_SPEED;
        }
    } else cali_p->_target += FINE_SPEED;

    tb_foc_set_current_vector(cali_p->_target, 
        Current_Cali_Current);
    
    if (cali_p->_target <= target) return;
    cali_p->state = STATE_BWD_RETURN;
}

/**
 * State backward return callback function, back it up.
 * @param cali_p pointer to an '_cali_attr_t' cali.
 * @return next state.
 */
static void _state_bwd_return_execute(_cali_attr_t * cali_p)
{
    uint32_t target = Move_Pulse_NUM * 2 + Move_Divide_NUM * 20;

    cali_p->_target += FINE_SPEED;

    tb_foc_set_current_vector(cali_p->_target, 
        Current_Cali_Current);

    if (cali_p->_target != target) return;
    cali_p->state = STATE_BWD_GAP;
}

/**
 * State backward elimination callback function, reverse elimination of error.
 * @param cali_p pointer to an '_cali_attr_t' cali.
 * @return next state.
 */
static void _state_bwd_gap_execute(_cali_attr_t * cali_p)
{
    uint32_t target = Move_Pulse_NUM * 2;

    cali_p->_target -= FINE_SPEED;

    tb_foc_set_current_vector(cali_p->_target, 
        Current_Cali_Current);

    if (cali_p->_target != target) return;

    cali_p->state = STATE_BWD_START;
    cali_p->avg_cnt = Move_Step_NUM;
}

/**
 * State backward start callback function, backward acquisition of encoder measurements.
 * @param cali_p pointer to an '_cali_attr_t' cali.
 * @return next state.
 */
static void _state_bwd_start_execute(_cali_attr_t * cali_p)
{
    uint32_t target = Move_Pulse_NUM;

    /**
     * 由于第一次执行到此处就会采集数据，
     * 所以最终会采集 201 个数据，
     * 即数组下标 0-200。
     */
    if ((cali_p->_target % Move_Divide_NUM) == 0) {
        cali_p->rawbuf[cali_p->raw_num] = _angle.raw;
        cali_p->raw_num++;

        if (cali_p->raw_num == READ_CNT) {
            cali_p->backward[cali_p->avg_cnt] = \
                _average(cali_p->rawbuf, READ_CNT, RESOLUTION);

            cali_p->avg_cnt--;
            cali_p->raw_num = 0;
            cali_p->_target -= FINE_SPEED;
        }
    } else cali_p->_target -= FINE_SPEED;

    tb_foc_set_current_vector(cali_p->_target, 
        Current_Cali_Current);

    if (cali_p->_target >= target) return;
    cali_p->state = STATE_SOLVE;
}

/**
 * Magnetic encoder calibration, data acquisition program, 
 * open-loop state control motor turns left once, 
 * turn right turn in the process to collect 
 * and store encoder output values.
 */
void _enc_cali_tick_work()
{
    if (!cali._start) return;
    switch (cali.state) {
    case STATE_IDLE: _state_idle_execute(&cali); break;
    case STATE_FWD_READY: _state_fwd_ready_execute(&cali); break;
    case STATE_FWD_START: _state_fwd_start_execute(&cali); break;
    case STATE_BWD_RETURN: _state_bwd_return_execute(&cali); break;
    case STATE_BWD_GAP: _state_bwd_gap_execute(&cali); break;
    case STATE_BWD_START: _state_bwd_start_execute(&cali); break;
    case STATE_SOLVE: tb_foc_set_current_vector(0, 0); break;
    }
}

/**
 * Look for interval subscripts and step differences, 
 * with the aim of finding the motor start position, 
 * as the motor does not necessarily start collecting data from the 0 position.
 */
static void _enc_cali_verify()
{
    int32_t diff2 = 0;
    int32_t diff3 = 0;
    uint32_t step = 0;

    int32_t resolution = RESOLUTION / Move_Step_NUM;

    for (uint32_t i = 0; i < (Move_Step_NUM + 1); i++) {
        cali.forward[i] = (uint16_t)_average2(
            (int32_t)cali.forward[i], 
            (int32_t)cali.backward[i], 
            RESOLUTION);
    }

    uint16_t * data_p = cali.forward;

    /*使用第一个数据和最后一个数据比较，判断数据方向性。*/
    int32_t diff1 = _subtract(
        data_p[0], data_p[Move_Step_NUM - 1], 
        RESOLUTION);


    if (diff1 != 0) {
        if (diff1 > 0) cali._dir = true;
        else cali._dir = false;
    } else {
        cali.errid = ERR_AVG_DIR;
        return;
    }
    
    for (uint32_t i = 1; i < Move_Step_NUM; i++) {
        /**
         * 连续循环计算前后相邻两个数据的相差值，用于判断数据的连续性，
         * 以及数据差值的分布是否均匀，在十分理想的条件下两个
         * 数据的相差值是 16384 / 200 = 81.92。
         * 即 RESOLUTION / Move_Step_NUM。
         */
        diff2 = _subtract(data_p[i], 
            data_p[i - 1], RESOLUTION);
        
        /*相邻两次采集的数据相差过大，大于正常间隔 3/2*/
        if (abs(diff2) > (resolution * 3 / 2)) {
            cali.errid = ERR_AVG_CONTINUTY;
            cali.errdata = i;
            return;
        }

        /*相邻两次采集的数据相差差过小，小于正常间隔 1/2*/
        if (abs(diff2) < (resolution * 1 / 2)) {
            cali.errid = ERR_AVG_CONTINUTY;
            cali.errdata = i;
            return;
        }

        /*数据方向错误*/
        if (diff2 == 0) {
            cali.errid = ERR_AVG_DIR;
            cali.errdata = i;
            return;
        }

        if ((diff2 > 0) && (!cali._dir)) {
            cali.errid = ERR_AVG_DIR;
            cali.errdata = i;
            return;
        }

        if ((diff2 < 0) && (cali._dir)) {
            cali.errid = ERR_AVG_DIR;
            cali.errdata = i;
            return;
        }
    }

    uint16_t _ofs = 0;
    uint16_t ofs = 0;

    if (cali._dir) {
        for (uint32_t i = 0; i < Move_Step_NUM; i++) {
            _ofs = _mod(i + 0, Move_Step_NUM);
            ofs = _mod(i + 1, Move_Step_NUM);

            diff3 = (int32_t)data_p[ofs] - (int32_t)data_p[_ofs];

            if (diff3 < 0) {
                step++;
                /*使用区间前标*/
                cali.rcd_x = i;
                cali.rcd_y = (RESOLUTION - 1) - \
                    data_p[_mod((cali.rcd_x + 0), Move_Step_NUM)];
            }
        }
        if (step != 1) {cali.errid = ERR_PHASE_STEP; return;}
    } else {
        for (uint32_t i = 0; i < Move_Step_NUM; i++) {
            _ofs = _mod(i + 0, Move_Step_NUM);
            ofs = _mod(i + 1, Move_Step_NUM);

            diff3 = (int32_t)data_p[ofs] - (int32_t)data_p[_ofs];

            if (diff3 > 0) {
                step++;
                /*使用区间前标*/
                cali.rcd_x = i;
                cali.rcd_y = (RESOLUTION - 1) - \
                    data_p[_mod((cali.rcd_x + 1), Move_Step_NUM)];
            }
        }
        if (step != 1) {cali.errid = ERR_PHASE_STEP; return;}
    }

    cali.errid = ERR_NO;
}

/**
 * The collected 200 data (one data collected every 1.8°) are processed linearly interpolated, 
 * and the 200 data are interpolated to 16384 data, 
 * and the interpolated data range is 0-512000, that is, 256 * 200 = 512000, 
 * which is the number of single-turn pulses of the stepper motor.
 */
void _enc_cali_solve()
{
    int32_t step_x = 0;
    int32_t step_y = 0;
    uint16_t _val = 0;
    int32_t val = 0;

    if (cali.state != STATE_SOLVE) return;
    _enc_cali_verify();
    if (cali.errid != ERR_NO) return;
    /*Erase the data area*/
    rom_data_clear(&_quick_cali);
    /*Start writing the data area*/
    rom_data_begin(&_quick_cali);
    cali.result_num = 0;

    if (cali._dir) {
        for (step_x = cali.rcd_x; step_x < cali.rcd_x + Move_Step_NUM + 1; step_x++) {
            val = _subtract(
                cali.forward[_mod(step_x + 1, Move_Step_NUM)], 
                cali.forward[_mod(step_x, Move_Step_NUM)], 
                RESOLUTION);

            /*Start the edge*/
            if (step_x == cali.rcd_x) {
                for (step_y = cali.rcd_y; step_y < val; step_y++) {
                    _val = _mod(Move_Divide_NUM * step_x + Move_Divide_NUM * step_y / val, 
                                Move_Pulse_NUM);
                    rom_write_data16(&_quick_cali, &_val, 1);
                    cali.result_num++;
                }
            } 
            else 
            /*End edge*/
            if (step_x == cali.rcd_x + Move_Step_NUM) {
                for (step_y = 0; step_y < cali.rcd_y; step_y++) {
                    _val = _mod(Move_Divide_NUM * step_x + Move_Divide_NUM * step_y / val, 
                                Move_Pulse_NUM);
                    rom_write_data16(&_quick_cali, &_val, 1);
                    cali.result_num++;
                }
            }
            /*Middle*/
            else {
                for (step_y = 0; step_y < val; step_y++) {
                    _val = _mod(Move_Divide_NUM * step_x + Move_Divide_NUM * step_y / val, 
                                Move_Pulse_NUM);
                    rom_write_data16(&_quick_cali, &_val, 1);
                    cali.result_num++;
                }
            }
        }
    } else {
        for (step_x = cali.rcd_x + Move_Step_NUM; step_x > cali.rcd_x - 1; step_x--) {
            val = _subtract(
                cali.forward[_mod(step_x, Move_Step_NUM)], 
                cali.forward[_mod(step_x + 1, Move_Step_NUM)], 
                RESOLUTION);

            /*Start the edge*/
            if (step_x == cali.rcd_x + Move_Step_NUM) {
                for (step_y = cali.rcd_y; step_y < val; step_y++) {
                    _val = _mod(Move_Divide_NUM * (step_x + 1) - Move_Divide_NUM * step_y / val, 
                                Move_Pulse_NUM);
                    rom_write_data16(&_quick_cali, &_val, 1);
                    cali.result_num++;
                }
            } 
            else
            /*End edge*/
            if (step_x == cali.rcd_x) {
                for (step_y = 0; step_y < cali.rcd_y; step_y++) {
                    _val = _mod(Move_Divide_NUM * (step_x + 1) - Move_Divide_NUM * step_y / val, 
                                Move_Pulse_NUM);
                    rom_write_data16(&_quick_cali, &_val, 1);
                    cali.result_num++;
                }
            } 
            /*Middle*/
            else {
                for (step_y = 0; step_y < val; step_y++) {
                    _val = _mod(Move_Divide_NUM * (step_x + 1) - Move_Divide_NUM * step_y / val, 
                                Move_Pulse_NUM);
                    rom_write_data16(&_quick_cali, &_val, 1);
                    cali.result_num++;
                }
            }
        }
    }

    /*End the write data area*/
    rom_data_end(&_quick_cali);
    
    /*The number of calibrated data is incorrect*/
    if (cali.result_num != RESOLUTION)
        cali.errid = ERR_QUANTITY;

    if (cali.errid != ERR_NO) {
        _angle.rectify_valid = false;
        rom_data_clear(&_quick_cali);
    } else _angle.rectify_valid = true;

    cali.state = STATE_IDLE;
    cali._start = false;
    
    /*Software restart*/
    HAL_NVIC_SystemReset();
}
