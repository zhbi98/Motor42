/**
 * @file _485_protocol.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "control_config.h"
#include "motor_control.h"
#include "Location_Tracker.h"
#include "Speed_Tracker.h"
#include "Current_Tracker.h"
#include "setup.h"
#include "enc_cali.h"

#include "mb.h"
#include "mbrtu.h"
#include "mbtimer.h"
#include "_485_protocol.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

_cmd_reg_t _cmd = {0};
_write_reg_t _wri = {0};
_req_reg_t _req = {0};

/**********************
 *  STATIC PROTOTYPES
 **********************/

extern _cali_attr_t cali;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Rs485 is configured in identifier masking mode, 
 * all packets are received, and the corresponding ID packets are filtered 
 * out in the form of software masks, which is commonly 
 * referred to as software filtering
 */
void dev_rs485_cmd(uint8_t _cmd, uint8_t * _data, uint32_t _len)
{
    uint8_t canNodeId = _setup.can_id;
    float _float_val = 0.0f;
    int32_t _int_val = 0U;

    switch (_cmd) {
    /*0x00~0x0F No Memory CMDs*/
    case 0x01: /*Enable Motor*/
        motor_control.mode_order = (*(uint32_t *)(_data/*RxData*/) == 1) ?
            Motor_Mode_Digital_Speed : Control_Mode_Stop;
        break;
    case 0x02: /*Do Calibration*/
        if (cali._start != 1) cali._start = 1;
        break;
    case 0x03: /*Set Current SetPoint*/
        if (motor_control.mode_run != Motor_Mode_Digital_Current)
            Motor_Control_SetMotorMode(Motor_Mode_Digital_Current);
        Motor_Control_Write_Goal_Current((int32_t)(*(float *)_data/*RxData*/ * 1000));
        break;
    case 0x04: /*Set Velocity SetPoint*/
        if (motor_control.mode_run != Motor_Mode_Digital_Speed) {
            Move_Rated_Speed = 30U * Move_Pulse_NUM;
            Motor_Control_SetMotorMode(Motor_Mode_Digital_Speed);
        }
        Motor_Control_Write_Goal_Speed(
            (int32_t)(*(float *)_data/*RxData*/ * (float)Move_Pulse_NUM));
        break;
    case 0x05: /*Set Position SetPoint*/
    {
        if (motor_control.mode_run != Motor_Mode_Digital_Location) {
            Move_Rated_Speed = 30U * Move_Pulse_NUM;
            Motor_Control_SetMotorMode(Motor_Mode_Digital_Location);
        }
        Motor_Control_Write_Goal_Location(
            (int32_t)(*(float *)_data/*RxData*/ * (float)Move_Pulse_NUM));
    }
        break;
    case 0x06: /*Set Position with Time*/
    {
        if (motor_control.mode_run != Motor_Mode_Digital_Location)
            Motor_Control_SetMotorMode(Motor_Mode_Digital_Location);
        Motor_Control_Write_Goal_Location_WithTime(
            (int32_t)(*(float *)_data/*RxData*/ * (float)Move_Pulse_NUM),
            *(float*)(_data/*RxData*/ + 4));
    }
        break;
    case 0x07: /*Set Position with Velocity-Limit*/
    {
        if (motor_control.mode_run != Motor_Mode_Digital_Location) {
            Move_Rated_Speed = 30U * Move_Pulse_NUM;
            Motor_Control_SetMotorMode(Motor_Mode_Digital_Location);
        }
        Move_Rated_Speed = (int32_t)(*(float *)(_data/*RxData*/ + 4) * \
            (float)Move_Pulse_NUM);

        /**The new rated speed needs to be synchronized to 
        the position tracker, which needs the rated 
        speed to generate the process speed.*/
        Location_Tracker_Set_MaxSpeed(Move_Rated_Speed);
        /*Updating acceleration at the same time as updating the 
        rated speed can cause the acceleration process to be too slow when the
        rated speed is very low.Causes uncontrollable exercise time*/
        /*Location_Tracker_Set_UpAcc(Move_Rated_Speed);*/
        /*Location_Tracker_Set_DownAcc(Move_Rated_Speed);*/

        Motor_Control_Write_Goal_Location(
            (int32_t)(*(float *)_data/*RxData*/ * (float)Move_Pulse_NUM));
    }
        break;


    /*0x10~0x1F CMDs with Memory*/
    case 0x11: /*Set Node-ID and Store to EEPROM*/
        _setup.can_id = *(uint32_t*)(_data/*RxData*/);
        if (_data[4]) { /*It need to be stored*/
            operate_file(0);
        }
        break;
    case 0x12: /*Set Current-Limit and Store to EEPROM*/
        Current_Rated_Current = (int32_t)(*(float *)_data/*RxData*/ * 1000);
        _setup.current_rated = Current_Rated_Current;
        if (_data[4]) { /*It need to be stored*/
            operate_file(0);
        }
        break;
    case 0x13: /*Set Velocity-Limit and Store to EEPROM*/
        Move_Rated_Speed = (int32_t)(*(float *)_data/*RxData*/ *
                       (float)Move_Pulse_NUM);
        _setup.speed_rated = Move_Rated_Speed;
        if (_data[4]) { /*It need to be stored*/
            operate_file(0);
        }
        break;
    case 0x14: /*Set Acceleration （and Store to EEPROM）*/
        _float_val = *(float *)_data/*RxData*/ * (float)Move_Pulse_NUM;

        Move_Rated_UpAcc = (int32_t)_float_val;
        Move_Rated_DownAcc = (int32_t)_float_val;

        /**The new speed acc needs to be synchronized to 
        the speed tracker, which needs the speed 
        acc to generate the process acc.*/
        Speed_Tracker_Set_UpAcc((int32_t)_float_val);
        Speed_Tracker_Set_DownAcc((int32_t)_float_val);
        Location_Tracker_Set_UpAcc((int32_t)_float_val);
        Location_Tracker_Set_DownAcc((int32_t)_float_val);

        _setup.speed_up_acc = Move_Rated_UpAcc;
        _setup.speed_down_acc = Move_Rated_DownAcc;
        if (_data[4]) { /*It need to be stored*/
            operate_file(0);
        }
        break;
    case 0x15: /*Apply Home-Position and Store to EEPROM*/
        /*Obtain the current location and mark the location as the home zero point.*/
        Motor_Control_Write_PosAsHomeOffset();
        _setup.home_ofs = Move_Home_Offset % Move_Pulse_NUM;
        operate_file(0);
        break;
    case 0x16: /*Set Auto-Enable and Store to EEPROM*/
        _setup.motor_onboot = (*(uint32_t *)(_data/*RxData*/) == 1);
        if (_data[4]) { /*It need to be stored*/
            operate_file(0);
        }
        break;
    case 0x17: /*Set DCE Kp*/
        dce.kp = *(int32_t *)(_data/*RxData*/);
        _setup.dce_kp = dce.kp;
        if (_data[4]) { /*It need to be stored*/
            operate_file(0);
        }
        break;
    case 0x18: /*Set DCE Kv*/
        dce.kv = *(int32_t *)(_data/*RxData*/);
        _setup.dce_kv = dce.kv;
        if (_data[4]) { /*It need to be stored*/
            operate_file(0);
        }
        break;
    case 0x19: /*Set DCE Ki*/
        dce.ki = *(int32_t *)(_data/*RxData*/);
        _setup.dce_ki = dce.ki;
        if (_data[4]) { /*It need to be stored*/
            operate_file(0);
        }
        break;
    case 0x1A: /*Set DCE Kd*/
        dce.kd = *(int32_t *)(_data/*RxData*/);
        _setup.dce_kd = dce.kd;
        if (_data[4]) { /*It need to be stored*/
            operate_file(0);
        }
        break;
    case 0x1B: /*Set Enable Stall-Protect*/
        Motor_Control_SetStallSwitch((*(uint32_t *)(_data/*RxData*/) == 1));
        _setup.stall_protect = (*(uint32_t*)(_data/*RxData*/) == 1);
        if (_data[4]) { /*It need to be stored*/
            operate_file(0);
        }
        break;

    case 0x7e: /*Erase Configs*/
        /*CONFIG_RESTORE;*/
        operate_file(1);
        break;
    case 0x7f: /*Reboot*/
        HAL_NVIC_SystemReset();
        break;
    default: break;
    }
}

/**
 * Rs485 is configured in identifier masking mode, 
 * all packets are received, and the corresponding ID packets are filtered 
 * out in the form of software masks, which is commonly 
 * referred to as software filtering
 */
uint32_t dev_rs485_apply()
{
    /*Key event handling for relay interaction*/
    bool valid = mb_rtu_reg_range_valid();
    uint16_t start = 0, end = 0;
    uint8_t _data[12] = {0};
    uint8_t val = 0;

    if (valid) {
        mb_rtu_reg_get_range(&start, &end);

        if (
            start < CMD_REG_ID_START || 
            end > WR_REG_ID_END
        ) {
            /*Data has been applied to the driver 
            to clear the event*/
            mb_rtu_reg_clear_range();
            return 0;
        }
        else
        if (
            start >= CMD_REG_ID_START && 
            end <= CMD_REG_VAL_END
        ) {
            uint8_t ofs = 0;
            uint8_t _ofs = 0;

            for (uint8_t cnt = 0; cnt < 3; cnt++) {
                memcpy(&_data[ofs + 0], 
                    (uint8_t *)&_cmd.val[_ofs + 1], 2);
                memcpy(&_data[ofs + 2], 
                    (uint8_t *)&_cmd.val[_ofs + 0], 2);

                ofs += 4;
                _ofs += 2;
            }

            /*float _float_val = 
                *(float *)_data;*/

            dev_rs485_cmd(_cmd._id, 
                _data, 12);
        }
        else
        if (
            start >= WR_REG_ID_START && 
            end <= WR_REG_ID_END
        ) {
            /*Data has been applied to the driver 
            to clear the event*/
            val = (uint8_t)_wri.slaveid;
            mb_rtu_set_slave_addr(val);
        }

        /*Data has been applied to the driver 
        to clear the event*/
        mb_rtu_reg_clear_range();
    }

    return 0;
}

/**
 * Rs485 is configured in identifier masking mode, 
 * all packets are received, and the corresponding ID packets are filtered 
 * out in the form of software masks, which is commonly 
 * referred to as software filtering
 */
uint32_t dev_rs485_refer()
{
    float _float_val = 0.0f;
    int32_t _int_val = 0U;
    uint8_t * bin = NULL;

    /*0x20~0x2F Inquiry CMDs*/

    /*_float_val = 5.2f;*/
    _float_val = Motor_Control_Read_Goal_FocCurrent();
    bin = (uint8_t *)&_float_val;
    /*for (int8_t i = 0; i < 4; i++)
        _data[i] = *(bin + i);*/

    _req.cur[0] = (bin[3] << 8) | bin[2];
    _req.cur[1] = (bin[1] << 8) | bin[0];

    /*_float_val = 10.2f;*/
    _float_val = Motor_Control_Read_Goal_Speed();
    bin = (uint8_t *)&_float_val;
    /*for (int8_t i = 0; i < 4; i++)
        _data[i] = *(bin + i);*/

    _req.vel[0] = (bin[3] << 8) | bin[2];
    _req.vel[1] = (bin[1] << 8) | bin[0];

    /*_float_val = 15.2f;*/
    _float_val = Motor_Control_Read_Goal_Position(false);
    bin = (uint8_t *)&_float_val;
    /*for (int i = 0; i < 4; i++)
        _data[i] = *(bin + i);*/

    _req.pos[0] = (bin[3] << 8) | bin[2];
    _req.pos[1] = (bin[1] << 8) | bin[0];

    _int_val = Move_Home_Offset;
    bin = (uint8_t *)&_int_val;
    /*for (int i = 0; i < 4; i++)
        _data[i] = *(bin + i);*/

    _req.home[0] = (bin[3] << 8) | bin[2];
    _req.home[1] = (bin[1] << 8) | bin[0];

    return 0;
}
