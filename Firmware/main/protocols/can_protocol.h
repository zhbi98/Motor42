/**
 * @file can_protocol.h
 *
 */

#ifndef __CAN_PROTOCOL_H__
#define __CAN_PROTOCOL_H__

/*********************
 *      INCLUDES
 *********************/

#include <stdint.h>

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void dev_can_cmd(uint8_t _cmd, uint8_t * _data, uint32_t _len);

#endif /*__PORT_CAN_H__*/
