/**
 * @file tim.h
 *
 */

#ifndef __TIM_H__
#define __TIM_H__

/*********************
 *      INCLUDES
 *********************/

#include "stm32f1xx_hal.h"
#include "main.h"

/*********************
 *      DEFINES
 *********************/

extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim2;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void x42_TIM4_init();
void x42_TIM2_init();

#endif
