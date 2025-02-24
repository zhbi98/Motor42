/**
 * @file spi.h
 *
 */

#ifndef __SPI_H__
#define __SPI_H__

/*********************
 *      INCLUDES
 *********************/

#include "main.h"

/*********************
 *      DEFINES
 *********************/

extern SPI_HandleTypeDef hspi1;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void x42_spi_init();

#endif /*__SPI_H__*/
