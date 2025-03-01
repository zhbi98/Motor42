/**
 * @file btn_doing.h
 *
 */

#ifndef __BTN_DOING_H__
#define __BTN_DOING_H__

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

void btn_doing_start();
void btn_doing_tick_work();
void btn_doing_tick_inc(uint32_t tick_period);

#endif /*__BTN_DOING_H__*/
