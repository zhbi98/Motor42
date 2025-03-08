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
#include "enc_cali.h"
#include "can.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

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
    float tmpF;
    int32_t tmpI;

    switch (_cmd)
    {
        /*0x00~0x0F No Memory CMDs*/
        case 0x01: /*Enable Motor*/
            motor_control.mode_order = (*(uint32_t *)(RxData) == 1) ?
                Motor_Mode_Digital_Speed : Control_Mode_Stop;
            break;
        case 0x02: /*Do Calibration*/
            cali._start = 1;
            break;
        case 0x03: /*Set Current SetPoint*/
            if (motor_control.mode_run != Motor_Mode_Digital_Current)
                Motor_Control_SetMotorMode(Motor_Mode_Digital_Current);
            Motor_Control_Write_Goal_Current((int32_t) (*(float *)RxData * 1000));
            break;
        case 0x04: /*Set Velocity SetPoint*/
            if (motor_control.mode_run != Motor_Mode_Digital_Speed)
            {
                Move_Rated_Speed = 30U * Move_Pulse_NUM;
                Motor_Control_SetMotorMode(Motor_Mode_Digital_Speed);
            }
            Motor_Control_Write_Goal_Speed(
                (int32_t) (*(float *)RxData *
                           (float) Move_Pulse_NUM));
            break;
        case 0x05: /*Set Position SetPoint*/
            if (motor_control.mode_run != Motor_Mode_Digital_Location)
            {
                Move_Rated_Speed = 30U * Move_Pulse_NUM;
                Motor_Control_SetMotorMode(Motor_Mode_Digital_Location);
            }
            Motor_Control_Write_Goal_Location(
                (int32_t) (*(float *)RxData * (float) Move_Pulse_NUM));
            /*增加 HomeOffset 是为了调节电机零点，便于因适配结构需要调节机械臂关节零点位置*/
            if (_data[4]) /*Need Position & Finished ACK*/
            {
                tmpF = Motor_Control_Read_Goal_Position(0);
                uint8_t * b = (uint8_t *) &tmpF;
                for (int8_t i = 0; i < 4; i++)
                    _data[i] = *(b + i);
                _data[4] = motor_control.state == Control_State_Finish ? 1 : 0;
                txHeader.StdId = (0x01/*canNodeId*/ << 7) | 0x23;
                CAN_Send(&txHeader, _data);
            }
            break;
        case 0x06: /*Set Position with Time*/
            if (motor_control.mode_run != Motor_Mode_Digital_Location)
                Motor_Control_SetMotorMode(Motor_Mode_Digital_Location);
            Motor_Control_Write_Goal_Location_WithTime(
                (int32_t) (*(float *)RxData * (float)Move_Pulse_NUM),
                *(float*) (RxData + 4));
            if (_data[4]) /*Need Position & Finished ACK*/
            {
                tmpF = Motor_Control_Read_Goal_Position(0);
                uint8_t * b = (uint8_t *)&tmpF;
                for (int8_t i = 0; i < 4; i++)
                    _data[i] = *(b + i);
                _data[4] = motor_control.state == Control_State_Finish ? 1 : 0;
                txHeader.StdId = (0x01/*canNodeId*/ << 7) | 0x23;
                CAN_Send(&txHeader, _data);
            }
            break;
        case 0x07: /*Set Position with Velocity-Limit*/
        {
            if (motor_control.mode_run != Motor_Mode_Digital_Location)
            {
                Move_Rated_Speed = 30U * Move_Pulse_NUM;
                Motor_Control_SetMotorMode(Motor_Mode_Digital_Location);
            }
            Move_Rated_Speed =
                (int32_t) (*(float *) (RxData + 4) * (float) Move_Pulse_NUM);
            Motor_Control_Write_Goal_Location(
                (int32_t) (*(float *) RxData * (float) Move_Pulse_NUM));
            // Always Need Position & Finished ACK
            tmpF = Motor_Control_Read_Goal_Position(0);
            uint8_t * b = (uint8_t *)&tmpF;
            for (int8_t i = 0; i < 4; i++)
                _data[i] = *(b + i);
            _data[4] = motor_control.state == Control_State_Finish ? 1 : 0;
            txHeader.StdId = (0x01/*canNodeId*/ << 7) | 0x23;
            CAN_Send(&txHeader, _data);
        }
            break;

            // 0x10~0x1F CMDs with Memory
        case 0x11: /*Set Node-ID and Store to EEPROM*/
            /*boardConfig.canNodeId = *(uint32_t*)(RxData);*/
            if (_data[4]) {
                /*boardConfig.configStatus = CONFIG_COMMIT;*/
            }
            break;
        case 0x12:  // Set Current-Limit and Store to EEPROM
            Current_Rated_Current = (int32_t)(*(float *)RxData * 1000);
            /*boardConfig.currentLimit = Current_Rated_Current;*/
            if (_data[4]) {
                /*boardConfig.configStatus = CONFIG_COMMIT;*/
            }
            break;
        case 0x13: /*Set Velocity-Limit and Store to EEPROM*/
            Move_Rated_Speed =
                (int32_t)(*(float *)RxData *
                           (float)Move_Pulse_NUM);
            /*boardConfig.velocityLimit = Move_Rated_Speed;*/
            if (_data[4]) {
                /*boardConfig.configStatus = CONFIG_COMMIT;*/
            }
            break;
        case 0x14: /*Set Acceleration （and Store to EEPROM）*/
            tmpF = *(float *)RxData * (float)Move_Pulse_NUM;

            Move_Rated_UpAcc = (int32_t)tmpF;
            Move_Rated_DownAcc = (int32_t)tmpF;

            Speed_Tracker_Set_UpAcc((int32_t)tmpF);
            Speed_Tracker_Set_DownAcc((int32_t)tmpF);

            /*boardConfig.velocityAcc = Move_Rated_UpAcc;*/
            if (_data[4]) {
                /*boardConfig.configStatus = CONFIG_COMMIT;*/
            }
            break;
        case 0x15: /*Apply Home-Position and Store to EEPROM*/
            /*motor.controller->ApplyPosAsHomeOffset();*/
            /*boardConfig.encoderHomeOffset = motor.config.motionParams.encoderHomeOffset %*/
                                            /*Move_Pulse_NUM;*/
            /*boardConfig.configStatus = CONFIG_COMMIT;*/
            break;
        case 0x16: /*Set Auto-Enable and Store to EEPROM*/
            // boardConfig.enableMotorOnBoot = (*(uint32_t*) (RxData) == 1);
            // if (_data[4])
            //     boardConfig.configStatus = CONFIG_COMMIT;
            break;
        case 0x17: /*Set DCE Kp*/
            dce.kp = *(int32_t *)(RxData);
            /*boardConfig.dce_kp = dce.kp;*/
            if (_data[4]) {
                /*boardConfig.configStatus = CONFIG_COMMIT;*/
            }
            break;
        case 0x18: /*Set DCE Kv*/
            dce.kv = *(int32_t *)(RxData);
            /*boardConfig.dce_kv = dce.kv;*/
            if (_data[4]) {
                /*boardConfig.configStatus = CONFIG_COMMIT;*/
            }
            break;
        case 0x19: /*Set DCE Ki*/
            dce.ki = *(int32_t *)(RxData);
            /*boardConfig.dce_ki = dce.ki;*/
            if (_data[4]) {
                /*boardConfig.configStatus = CONFIG_COMMIT;*/
            }
            break;
        case 0x1A: /*Set DCE Kd*/
            dce.kd = *(int32_t *)(RxData);
            /*boardConfig.dce_kd = dce.kd;*/
            if (_data[4]) {
                /*boardConfig.configStatus = CONFIG_COMMIT;*/
            }
            break;
        case 0x1B: /*Set Enable Stall-Protect*/
            Motor_Control_SetStallSwitch((*(uint32_t*)(RxData) == 1));
            /*boardConfig.enableStallProtect = (*(uint32_t*)(RxData) == 1);*/
            if (_data[4]) {
                /*boardConfig.configStatus = CONFIG_COMMIT;*/
            }
            break;


            // 0x20~0x2F Inquiry CMDs
        case 0x21: /*Get Current*/
        {
            tmpF = Motor_Control_Read_Goal_FocCurrent();
            uint8_t * b = (uint8_t *)&tmpF;
            for (int8_t i = 0; i < 4; i++)
                _data[i] = *(b + i);
            _data[4] = (motor_control.state == Control_State_Finish ? 1 : 0);

            txHeader.StdId = (0x01/*canNodeId*/ << 7) | 0x21;
            CAN_Send(&txHeader, _data);
        }
            break;
        case 0x22: /*Get Velocity*/
        {
            tmpF = Motor_Control_Read_Goal_Speed();
            uint8_t * b = (uint8_t *)&tmpF;
            for (int8_t i = 0; i < 4; i++)
                _data[i] = *(b + i);
            _data[4] = (motor_control.state == Control_State_Finish ? 1 : 0);

            txHeader.StdId = (0x01/*canNodeId*/ << 7) | 0x22;
            CAN_Send(&txHeader, _data);
        }
            break;
        case 0x23: /*Get Position*/
        {
            tmpF = Motor_Control_Read_Goal_Position(0);
            uint8_t * b = (uint8_t *) &tmpF;
            for (int i = 0; i < 4; i++)
                _data[i] = *(b + i);
            /*Finished ACK*/
            _data[4] = motor_control.state == Control_State_Finish ? 1 : 0;
            txHeader.StdId = (0x01/*canNodeId*/ << 7) | 0x23;
            CAN_Send(&txHeader, _data);
        }
            break;
        case 0x24: /*Get Offset*/
        {
            tmpI = 0/*encoderHomeOffset*/;
            uint8_t * b = (uint8_t *)&tmpI;
            for (int i = 0; i < 4; i++)
                _data[i] = *(b + i);
            txHeader.StdId = (0x01/*canNodeId*/ << 7) | 0x24;
            CAN_Send(&txHeader, _data);
        }
            break;

        case 0x7e: /*Erase Configs*/
            /*boardConfig.configStatus = CONFIG_RESTORE;*/
            break;
        case 0x7f: /*Reboot*/
            HAL_NVIC_SystemReset();
            break;
        default:
            break;
    }

}
