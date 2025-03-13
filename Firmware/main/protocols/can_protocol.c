/**
 * @file can_protocol.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "can_protocol.h"
#include "control_config.h"
#include "motor_control.h"
#include "Location_Tracker.h"
#include "Speed_Tracker.h"
#include "Current_Tracker.h"
#include "setup.h"
#include "enc_cali.h"
#include "can.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * CAN Tx message header structure definition.
 */
CAN_TxHeaderTypeDef txHeader = {
    .StdId = 0x00, .ExtId = 0x00,
    .IDE = CAN_ID_STD, .RTR = CAN_RTR_DATA, .DLC = 8,
    .TransmitGlobalTime = DISABLE,
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

extern _cali_attr_t cali;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * CAN is configured in identifier masking mode, 
 * all packets are received, and the corresponding ID packets are filtered 
 * out in the form of software masks, which is commonly 
 * referred to as software filtering
 */
void dev_can_cmd(uint8_t _cmd, uint8_t * _data, uint32_t _len)
{
    uint8_t canNodeId = _setup.can_id;
    float _float_val = 0.0f;
    int32_t _int_val = 0U;

    switch (_cmd) {
    /*0x00~0x0F No Memory CMDs*/
    case 0x01: /*Enable Motor*/
        motor_control.mode_order = (*(uint32_t *)(RxData) == 1) ?
            Motor_Mode_Digital_Speed : Control_Mode_Stop;
        break;
    case 0x02: /*Do Calibration*/
        if (cali._start != 1) cali._start = 1;
        break;
    case 0x03: /*Set Current SetPoint*/
        if (motor_control.mode_run != Motor_Mode_Digital_Current)
            Motor_Control_SetMotorMode(Motor_Mode_Digital_Current);
        Motor_Control_Write_Goal_Current((int32_t)(*(float *)RxData * 1000));
        break;
    case 0x04: /*Set Velocity SetPoint*/
        if (motor_control.mode_run != Motor_Mode_Digital_Speed) {
            Move_Rated_Speed = 30U * Move_Pulse_NUM;
            Motor_Control_SetMotorMode(Motor_Mode_Digital_Speed);
        }
        Motor_Control_Write_Goal_Speed(
            (int32_t)(*(float *)RxData * (float)Move_Pulse_NUM));
        break;
    case 0x05: /*Set Position SetPoint*/
    {
        if (motor_control.mode_run != Motor_Mode_Digital_Location) {
            Move_Rated_Speed = 30U * Move_Pulse_NUM;
            Motor_Control_SetMotorMode(Motor_Mode_Digital_Location);
        }
        Motor_Control_Write_Goal_Location(
            (int32_t)(*(float *)RxData * (float)Move_Pulse_NUM));
        /*增加 HomeOffset 是为了调节电机零点，便于因适配结构需要调节机械臂关节零点位置*/
        if (_data[4]) { /*Need Position & Finished ACK*/
            _float_val = Motor_Control_Read_Goal_Position(false);
            uint8_t * bin = (uint8_t *)&_float_val;
            for (int8_t i = 0; i < 4; i++)
                _data[i] = *(bin + i);
            _data[4] = motor_control.state == Control_State_Finish ? 1 : 0;
            txHeader.StdId = (canNodeId << 7) | 0x23;
            CAN_Send(&txHeader, _data);
        }
    }
        break;
    case 0x06: /*Set Position with Time*/
    {
        if (motor_control.mode_run != Motor_Mode_Digital_Location)
            Motor_Control_SetMotorMode(Motor_Mode_Digital_Location);
        Motor_Control_Write_Goal_Location_WithTime(
            (int32_t)(*(float *)RxData * (float)Move_Pulse_NUM),
            *(float*)(RxData + 4));
        if (_data[4]) { /*Need Position & Finished ACK*/
            _float_val = Motor_Control_Read_Goal_Position(false);
            uint8_t * bin = (uint8_t *)&_float_val;
            for (int8_t i = 0; i < 4; i++)
                _data[i] = *(bin + i);
            _data[4] = motor_control.state == Control_State_Finish ? 1 : 0;
            txHeader.StdId = (canNodeId << 7) | 0x23;
            CAN_Send(&txHeader, _data);
        }
    }
        break;
    case 0x07: /*Set Position with Velocity-Limit*/
    {
        if (motor_control.mode_run != Motor_Mode_Digital_Location) {
            Move_Rated_Speed = 30U * Move_Pulse_NUM;
            Motor_Control_SetMotorMode(Motor_Mode_Digital_Location);
        }
        Move_Rated_Speed = (int32_t)(*(float *)(RxData + 4) * \
            (float)Move_Pulse_NUM);
        Motor_Control_Write_Goal_Location(
            (int32_t)(*(float *)RxData * (float)Move_Pulse_NUM));
        // Always Need Position & Finished ACK
        _float_val = Motor_Control_Read_Goal_Position(false);
        uint8_t * bin = (uint8_t *)&_float_val;
        for (int8_t i = 0; i < 4; i++)
            _data[i] = *(bin + i);
        _data[4] = motor_control.state == Control_State_Finish ? 1 : 0;
        txHeader.StdId = (canNodeId << 7) | 0x23;
        CAN_Send(&txHeader, _data);
    }
        break;


    /*0x10~0x1F CMDs with Memory*/
    case 0x11: /*Set Node-ID and Store to EEPROM*/
        _setup.can_id = *(uint32_t*)(RxData);
        if (_data[4]) { /*It need to be stored*/
            operate_file(0);
        }
        break;
    case 0x12: /*Set Current-Limit and Store to EEPROM*/
        Current_Rated_Current = (int32_t)(*(float *)RxData * 1000);
        _setup.current_rated = Current_Rated_Current;
        if (_data[4]) { /*It need to be stored*/
            operate_file(0);
        }
        break;
    case 0x13: /*Set Velocity-Limit and Store to EEPROM*/
        Move_Rated_Speed = (int32_t)(*(float *)RxData *
                       (float)Move_Pulse_NUM);
        _setup.speed_rated = Move_Rated_Speed;
        if (_data[4]) { /*It need to be stored*/
            operate_file(0);
        }
        break;
    case 0x14: /*Set Acceleration （and Store to EEPROM）*/
        _float_val = *(float *)RxData * (float)Move_Pulse_NUM;

        Move_Rated_UpAcc = (int32_t)_float_val;
        Move_Rated_DownAcc = (int32_t)_float_val;

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
        _setup.motor_onboot = (*(uint32_t *)(RxData) == 1);
        if (_data[4]) { /*It need to be stored*/
            operate_file(0);
        }
        break;
    case 0x17: /*Set DCE Kp*/
        dce.kp = *(int32_t *)(RxData);
        _setup.dce_kp = dce.kp;
        if (_data[4]) { /*It need to be stored*/
            operate_file(0);
        }
        break;
    case 0x18: /*Set DCE Kv*/
        dce.kv = *(int32_t *)(RxData);
        _setup.dce_kv = dce.kv;
        if (_data[4]) { /*It need to be stored*/
            operate_file(0);
        }
        break;
    case 0x19: /*Set DCE Ki*/
        dce.ki = *(int32_t *)(RxData);
        _setup.dce_ki = dce.ki;
        if (_data[4]) { /*It need to be stored*/
            operate_file(0);
        }
        break;
    case 0x1A: /*Set DCE Kd*/
        dce.kd = *(int32_t *)(RxData);
        _setup.dce_kd = dce.kd;
        if (_data[4]) { /*It need to be stored*/
            operate_file(0);
        }
        break;
    case 0x1B: /*Set Enable Stall-Protect*/
        Motor_Control_SetStallSwitch((*(uint32_t *)(RxData) == 1));
        _setup.stall_protect = (*(uint32_t*)(RxData) == 1);
        if (_data[4]) { /*It need to be stored*/
            operate_file(0);
        }
        break;


    /*0x20~0x2F Inquiry CMDs*/
    case 0x21: /*Get Current*/
    {
        _float_val = Motor_Control_Read_Goal_FocCurrent();
        uint8_t * bin = (uint8_t *)&_float_val;
        for (int8_t i = 0; i < 4; i++)
            _data[i] = *(bin + i);
        _data[4] = (motor_control.state == Control_State_Finish ? 1 : 0);

        txHeader.StdId = (canNodeId << 7) | 0x21;
        CAN_Send(&txHeader, _data);
    }
        break;
    case 0x22: /*Get Velocity*/
    {
        _float_val = Motor_Control_Read_Goal_Speed();
        uint8_t * bin = (uint8_t *)&_float_val;
        for (int8_t i = 0; i < 4; i++)
            _data[i] = *(bin + i);
        _data[4] = (motor_control.state == Control_State_Finish ? 1 : 0);

        txHeader.StdId = (canNodeId << 7) | 0x22;
        CAN_Send(&txHeader, _data);
    }
        break;
    case 0x23: /*Get Position*/
    {
        _float_val = Motor_Control_Read_Goal_Position(false);
        uint8_t * bin = (uint8_t *)&_float_val;
        for (int i = 0; i < 4; i++)
            _data[i] = *(bin + i);
        /*Finished ACK*/
        _data[4] = motor_control.state == Control_State_Finish ? 1 : 0;
        txHeader.StdId = (canNodeId << 7) | 0x23;
        CAN_Send(&txHeader, _data);
    }
        break;
    case 0x24: /*Get Offset*/
    {
        _int_val = Move_Home_Offset;
        uint8_t * bin = (uint8_t *)&_int_val;
        for (int i = 0; i < 4; i++)
            _data[i] = *(bin + i);
        txHeader.StdId = (canNodeId << 7) | 0x24;
        CAN_Send(&txHeader, _data);
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
