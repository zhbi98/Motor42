/**
 * @file key.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "key.h"

/**********************
 * GLOBAL FUNCTIONS
 **********************/

uint8_t key_get_state()
{
    return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15);
}
