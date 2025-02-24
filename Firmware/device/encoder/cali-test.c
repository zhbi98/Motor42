
#if 0

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define HARD_STEPS   ((int32_t)(200))
#define DIVIDE ((int32_t)(256))
#define SUBDIV  ((int32_t)(HARD_STEPS * DIVIDE))

#define ENC_RANGE 16384U // 81.92 * 200
#define DIFF ((int32_t)(ENC_RANGE / HARD_STEPS))

enum {
    /**< There is no error in the data*/
    ERR_NO = 0x00, 
    /**< Wrong direction of average value acquisition*/
    ERR_AVG_DIR, 
    /**< Error in continuity of mean*/
    ERR_AVG_CONTINUTY, 
    /**< Step number error, used to find the motor starting 
    position, because the motor does not necessarily start 
    from the 0 position to collect data*/
    ERR_PHASE_STEP, 
    /**< Error parsing the amount of data*/
    ERR_QUANTITY, 
};

uint32_t count; // 用于各个计数
int32_t sub_data; // 用于各个算差
int32_t rcd_x, rcd_y;
uint8_t dir;
uint8_t  errid;
uint32_t error_data;
uint32_t result_num;

uint16_t forward[HARD_STEPS + 1] = {
    14401, 14516, 14584, 14672, 14742, 
    14839, 14911, 15000, 15073, 15168, 
    15241, 15331, 15403, 15498, 15572, 
    15661, 15734, 15828, 15904, 15993, 
    16065, 16157, 16232, 16321, 11, 
    101,   175,   264,   338,   430, 
    504,   593,   666,   760,   835, 
    923,   996,   1090,  1166,  1255, 
    1327,  1420,  1496,  1584,  1657, 
    1751,  1823,  1912,  1986,  2079, 
    2151,  2240,  2315,  2405,  2480, 
    2569,  2642,  2733,  2809,  2897, 
    2971,  3063,  3137,  3226,  3300, 
    3391,  3464,  3552,  3627,  3718, 
    3791,  3877,  3951,  4043,  4116, 
    4202,  4275,  4366,  4440,  4526, 
    4599,  4690,  4763,  4851,  4925, 
    5016,  5089,  5176,  5251,  5343, 
    5415,  5502,  5577,  5668,  5741, 
    5829,  5901,  5992,  6067,  6154, 
    6225,  6317,  6392,  6478,  6551, 
    6643,  6717,  6804,  6878,  6970, 
    7044,  7131,  7205,  7297,  7372, 
    7459,  7532,  7624,  7700,  7787, 
    7860,  7952,  8027,  8115,  8189, 
    8281,  8355,  8444,  8517,  8610, 
    8684,  8773,  8846,  8940,  9015, 
    9104,  9176,  9269,  9346,  9435, 
    9506,  9599,  9674,  9764,  9834, 
    9929,  10000, 10091, 10162, 10257, 
    10327, 10419, 10489, 10583, 10655, 
    10747, 10815, 10911, 10983, 11075, 
    11143, 11240, 11312, 11405, 11473, 
    11569, 11640, 11732, 11802, 11898, 
    11968, 12058, 12128, 12225, 12295, 
    12384, 12453, 12549, 12620, 12710, 
    12779, 12874, 12946, 13037, 13107, 
    13202, 13273, 13364, 13435, 13530, 
    13600, 13690, 13762, 13858, 13928, 
    14019, 14088, 14183, 14256, 14346, 
    14413, 
};

uint16_t backward[HARD_STEPS + 1] = {
    14450, 14524, 14614, 14685, 14776, 
    14852, 14940, 15013, 15104, 15179, 
    15268, 15342, 15433, 15508, 15596, 
    15672, 15762, 15837, 15924, 16001, 
    16090, 16166, 16253, 16329, 33, 
    110,   197,   273,   360,   439, 
    527,   603,   691,   768,   856, 
    933,   1021,  1098,  1185,  1263, 
    1351,  1429,  1515,  1592,  1679, 
    1759,  1845,  1919,  2008,  2086, 
    2172,  2246,  2336,  2412,  2499, 
    2575,  2664,  2740,  2827,  2904, 
    2992,  3069,  3155,  3230,  3318, 
    3396,  3482,  3556,  3644,  3721, 
    3808,  3881,  3969,  4045,  4131, 
    4206,  4294,  4370,  4455,  4531, 
    4619,  4695,  4781,  4856,  4944, 
    5022,  5108,  5182,  5270,  5348, 
    5434,  5509,  5597,  5674,  5760, 
    5836,  5923,  5998,  6084,  6161, 
    6247,  6323,  6410,  6484,  6571, 
    6650,  6736,  6811,  6899,  6976, 
    7063,  7139,  7228,  7304,  7391, 
    7468,  7556,  7631,  7719,  7796, 
    7884,  7961,  8047,  8124,  8211, 
    8289,  8376,  8452,  8538,  8617, 
    8705,  8781,  8869,  8946,  9034, 
    9112,  9200,  9277,  9365,  9443, 
    9533,  9609,  9698,  9774,  9864, 
    9941,  10029, 10102, 10196, 10269, 
    10359, 10433, 10525, 10597, 10688, 
    10762, 10854, 10927, 11019, 11091, 
    11183, 11257, 11348, 11418, 11512, 
    11586, 11676, 11746, 11839, 11912, 
    12002, 12073, 12165, 12238, 12326, 
    12398, 12492, 12563, 12651, 12724, 
    12817, 12890, 12978, 13050, 13142, 
    13217, 13306, 13377, 13470, 13544, 
    13634, 13706, 13798, 13871, 13961, 
    14034, 14124, 14196, 14288, 14361, 
    14447, 
};

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
 * Look for interval subscripts and step differences, 
 * with the aim of finding the motor start position, 
 * as the motor does not necessarily start collecting data from the 0 position.
 */
void Calibration_Data_Check()
{
    int32_t sub_data = 0;

    for (uint32_t count = 0; count < HARD_STEPS + 1; count++) {
        forward[count] = (uint16_t)_average2(
            (int32_t)forward[count], 
            (int32_t)backward[count], 
            ENC_RANGE
        );
    }

    // 平均值数据检查
    sub_data = _subtract(
        (int32_t)forward[0],
        (int32_t)forward[HARD_STEPS - 1],
        ENC_RANGE
    );

    if (sub_data == 0) {
        errid = ERR_AVG_DIR; 
        return;
    }

    if (sub_data > 0) {
        dir = true;
    }

    if (sub_data < 0) {
        dir = false;
    }

    for (uint32_t count = 1; count < HARD_STEPS; count++) {
        sub_data = _subtract(
            (int32_t)forward[count], 
            (int32_t)forward[count - 1], 
            ENC_RANGE
        );

        // 两次数据差过大
        if (abs(sub_data) > (DIFF * 3 / 2)) {
            errid = ERR_AVG_CONTINUTY;
            error_data = count;
            return;
        }

        // 两次数据差过小
        if (abs(sub_data) < (DIFF * 1 / 2)) {
            errid = ERR_AVG_CONTINUTY; 
            error_data = count; 
            return;
        }

        if (sub_data == 0) {
            errid = ERR_AVG_DIR;
            error_data = count;
            return;
        }

        if ((sub_data > 0) && (!dir)) {
            errid = ERR_AVG_DIR;
            error_data = count;
            return;
        }

        if ((sub_data < 0) && (dir)) {
            errid = ERR_AVG_DIR;
            error_data = count;
            return;
        }
    }

    uint32_t step_num = 0;

    if (dir) {
        for (uint32_t count = 0; count < HARD_STEPS; count++) {
            sub_data = (int32_t)forward[_mod(count + 1, HARD_STEPS)] - 
                        (int32_t)forward[_mod(count, HARD_STEPS)];

            if (sub_data < 0) {
                step_num++;
                rcd_x = count; // 使用区间前标
                rcd_y = (ENC_RANGE - 1) - forward[_mod(rcd_x, HARD_STEPS)];

                printf("rcd_x:%d\n", rcd_x);
                printf("rcd_y:%d\n", rcd_y);
            }
        }

        if (step_num != 1) {
            errid = ERR_PHASE_STEP;
            printf("STEP NUM:%d\n", step_num);
            return;
        }
    } else {
        for (uint32_t count = 0; count < HARD_STEPS; count++) {
            sub_data = (int32_t)forward[_mod(count + 1, HARD_STEPS)] - 
                        (int32_t)forward[_mod(count, HARD_STEPS)];
            
            if (sub_data > 0) {
                step_num++;
                rcd_x = count; // 使用区间前标
                rcd_y = (ENC_RANGE - 1) - forward[_mod(rcd_x + 1, HARD_STEPS)];

                printf("rcd_x:%d\n", rcd_x);
                printf("rcd_y:%d\n", rcd_y);
            }
        }

        if (step_num != 1) {
            errid = ERR_PHASE_STEP;
            printf("STEP NUM:%d\n", step_num);
            return;
        }
    }
}

/**
 * The collected 200 data (one data collected every 1.8°) are processed linearly interpolated, 
 * and the 200 data are interpolated to 16384 data, 
 * and the interpolated data range is 0-512000, that is, 256 * 200 = 512000, 
 * which is the number of single-turn pulses of the stepper motor.
 */
void calibration_loop()
{
    int32_t val;
    uint16_t _val;

    if (errid == ERR_NO) {
        int32_t step_x, step_y;
        result_num = 0;

        // rom_data_clear(&_quick_cali); // 擦除数据区
        // rom_data_begin(&_quick_cali); // 开始写数据区

        if (dir) {
            for (step_x = rcd_x; step_x < rcd_x + HARD_STEPS + 1; step_x++) {
                val = _subtract(
                    forward[_mod(step_x+1, HARD_STEPS)],
                    forward[_mod(step_x,   HARD_STEPS)],
                    ENC_RANGE
                );

                // 开始边缘
                if (step_x == rcd_x) { 
                    for (step_y = rcd_y; step_y < val; step_y++) {
                        _val = _mod(
                            DIVIDE * step_x + DIVIDE * step_y / val,
                            SUBDIV
                        );

                        // rom_write_data16(&_quick_cali, &_val, 1);
                        result_num++;
                    }
                } 
                else 
                // 结束边缘
                if (step_x == rcd_x + HARD_STEPS) { 
                    for (step_y = 0; step_y < rcd_y; step_y++) {
                        _val = _mod(
                            DIVIDE * step_x + DIVIDE * step_y / val,
                            SUBDIV
                        );

                        // rom_write_data16(&_quick_cali, &_val, 1);
                        result_num++;
                    }
                } 
                // 中间
                else { 
                    for (step_y = 0; step_y < val; step_y++) {
                        _val = _mod(
                            DIVIDE * step_x + DIVIDE * step_y / val,
                            SUBDIV
                        );

                        // rom_write_data16(&_quick_cali, &_val, 1);
                        result_num++;
                    }
                }
            }
        } else {
            for (step_x = rcd_x + HARD_STEPS; step_x > rcd_x - 1; step_x--) {
                val = _subtract(
                    forward[_mod(step_x, HARD_STEPS)],
                    forward[_mod(step_x + 1, HARD_STEPS)],
                    ENC_RANGE
                );

                // 开始边缘
                if (step_x == rcd_x + HARD_STEPS) { 
                    for(step_y = rcd_y; step_y < val; step_y++){
                        _val = _mod(
                            DIVIDE * (step_x + 1) - DIVIDE * step_y / val,
                            SUBDIV
                        );

                        // rom_write_data16(&_quick_cali, &_val, 1);
                        result_num++;
                    }
                } 
                else 
                // 结束边缘
                if(step_x == rcd_x) { 
                    for (step_y = 0; step_y < rcd_y; step_y++) {
                        _val = _mod(
                            DIVIDE * (step_x + 1) - DIVIDE * step_y / val,
                            SUBDIV
                        );

                        // rom_write_data16(&_quick_cali, &_val, 1);
                        result_num++;
                    }
                } 
                // 中间
                else { 
                    for (step_y = 0; step_y < val; step_y++) {
                        _val = _mod(
                            DIVIDE * (step_x + 1) - DIVIDE * step_y / val,
                            SUBDIV
                        );

                        // rom_write_data16(&_quick_cali, &_val, 1);
                        result_num++;
                    }
                }
            }
        }

        // rom_data_end(&_quick_cali); // 结束写数据区
        
        if (result_num != ENC_RANGE)
            errid = ERR_QUANTITY;
        else
            printf("MT6816 CAL SUCCESS\n");
    }
}

int32_t main()
{
    int32_t a = _subtract(1000, 800, 16384);
    printf("_subtract:%d\n", a);

    int32_t _a = _subtract(82, 16400, 16400);
    printf("_subtract:%d\n", _a);

    int32_t b = _mod(10, 200);
    printf("_mod:%d\n", b);

    Calibration_Data_Check();

    /*--------------------
    _subtract:200
    _subtract:82
    _mod:10
    rcd_x:199
    rcd_y:81
    --------------------*/

    errid = ERR_NO;
    calibration_loop();

    return 0;
}

#endif
