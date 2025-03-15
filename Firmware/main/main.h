/**
 * @file main.h
 *
 */

#ifndef __MAIN_H__
#define __MAIN_H__

/*********************
 *      INCLUDES
 *********************/

#include "stm32f1xx_hal.h"

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void SystemClock_Config();
void _TIM2_callback_20kHz();
void _TIM1_callback_100Hz();
void Error_Handler();

#endif /*__MAIN_H__*/
