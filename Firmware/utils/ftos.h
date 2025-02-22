/**
 * @file ftos.h
 *
 */

#ifndef __FTOS_H__
#define __FTOS_H__

/*********************
 *      INCLUDES
 *********************/

#include <stdio.h>
#include <stdint.h>

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void double_printf(double value, int32_t digs);
int8_t * double_string(double value, int32_t digs);

#endif /*__FTOS_H__*/
