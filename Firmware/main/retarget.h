/**
 * @file retarget.h
 *
 */

#ifndef __RETARGET_H__
#define __RETARGET_H__

/*********************
 *      INCLUDES
 *********************/

#include "usart.h"
#include <sys/stat.h>
#include <stdio.h>
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void RetargetInit(UART_HandleTypeDef *huart);

int _isatty(int fd);

int _write(int fd, char *ptr, int len);

int _close(int fd);

int _lseek(int fd, int ptr, int dir);

int _read(int fd, char *ptr, int len);

int _fstat(int fd, struct stat *st);

#endif /*__RETARGET_H__*/
