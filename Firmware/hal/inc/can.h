/**
 * @file can.h
 *
 */

#ifndef __CAN_H__
#define __CAN_H__

/*********************
 *      INCLUDES
 *********************/

#include "main.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/* USER CODE BEGIN Includes */
extern CAN_HandleTypeDef hcan;
extern CAN_TxHeaderTypeDef TxHeader;
extern CAN_RxHeaderTypeDef RxHeader;
extern uint8_t TxData[8];
extern uint8_t RxData[8];
extern uint32_t TxMailbox;
/* USER CODE END Includes */

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void x42_can_init();
void CAN_Send(CAN_TxHeaderTypeDef* pHeader, uint8_t* data);

#endif /*__CAN_H__*/
