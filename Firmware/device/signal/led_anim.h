/**
 * @file led.h
 *
 */

#ifndef __LED_ANIM_H__
#define __LED_ANIM_H__

/*********************
 *      INCLUDES
 *********************/

#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void led_anim_start();
void led_anim_tick_work();
void led_anim_tick_inc(uint32_t tick_period);

#endif /*__LED_ANIM_H__*/
