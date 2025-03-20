/**
 * @file temp.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "temp.h"
#include "adc.h"

/**********************
 * GLOBAL FUNCTIONS
 **********************/

extern uint16_t whole_adc_data[2][12];

/**
 * Get the current temperature of the motor drive, 
 * It is used for motor over-temperature protection.
 */
uint16_t _overtemp()
{
    uint16_t * _adc_p = NULL;
    _adc_p = whole_adc_data[0];
    return _adc_p[1];
}
