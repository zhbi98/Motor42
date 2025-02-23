/**
 * @file tb67h450.h
 *
 */

#ifndef __TB67H450FNG_H__
#define __TB67H450FNG_H__

/*********************
 *      INCLUDES
 *********************/

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "tim.h"

/*********************
 *      DEFINES
 *********************/

#define TB_PWMA(value) (__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, value))
#define TB_PWMB(value) (__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, value))

#define TB_AP_H() (GPIOB->BSRR = GPIO_PIN_10)
#define TB_AP_L() (GPIOB->BRR  = GPIO_PIN_10)
#define TB_AM_H() (GPIOB->BSRR = GPIO_PIN_11)
#define TB_AM_L() (GPIOB->BRR  = GPIO_PIN_11)
#define TB_BP_H() (GPIOB->BSRR = GPIO_PIN_12)
#define TB_BP_L() (GPIOB->BRR  = GPIO_PIN_12)
#define TB_BM_H() (GPIOB->BSRR = GPIO_PIN_13)
#define TB_BM_L() (GPIOB->BRR  = GPIO_PIN_13)

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
	uint16_t mapptr; /**< Array pointer to the Sine function*/
	int16_t mapval; /**< Sine function to convert the numeric value*/
	uint16_t bit12val; /**< 12-bit DAC value*/
} _sindac_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initializes the GPIOx peripheral according to the specified parameters in the GPIO_Init.
 * @param GPIOx: where x can be (A..G depending on device used) to select the GPIO peripheral
 * @param GPIO_Init: pointer to a GPIO_InitTypeDef structure that contains
 * @return None
 */
void tb_foc_set_current_vector(uint32_t _dir_inCNT, 
    int32_t _I_mA);

/**
 * Initializes the GPIOx peripheral according to the specified parameters in the GPIO_Init.
 * @param GPIOx: where x can be (A..G depending on device used) to select the GPIO peripheral
 * @param GPIO_Init: pointer to a GPIO_InitTypeDef structure that contains
 * @return None
 */
void tb_driver_sleep();

/**
 * Initializes the GPIOx peripheral according to the specified parameters in the GPIO_Init.
 * @param GPIOx: where x can be (A..G depending on device used) to select the GPIO peripheral
 * @param GPIO_Init: pointer to a GPIO_InitTypeDef structure that contains
 * @return None
 */
void tb_driver_brake();

#endif
