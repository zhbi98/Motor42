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

/**
 * CAN is configured in identifier masking mode, 
 * all packets are received, and the corresponding ID packets are filtered 
 * out in the form of software masks, which is commonly 
 * referred to as software filtering
 */
void dev_can_cmd(uint8_t _cmd, uint8_t * _data, uint32_t _len);

#endif /*__CAN_PROTOCOL_H__*/
