/**
 * @file _485_protocol.h
 *
 */

#ifndef __485_PROTOCOL_H__
#define __485_PROTOCOL_H__

/*********************
 *      INCLUDES
 *********************/

#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

#define CMD_REG_ID_START 0U
#define CMD_REG_ID_END 1U

#define CMD_REG_VAL_START 1U
#define CMD_REG_VAL_END 7U

#define WR_REG_ID_START 7U
#define WR_REG_ID_END 8U

#define REQ_REG_CUR_START 8U
#define REQ_REG_CUR_END 10U

#define REQ_REG_VEL_START 10U
#define REQ_REG_VEL_END 12U

#define REQ_REG_POS_START 12U
#define REQ_REG_POS_END 14U

#define REQ_REG_HOME_START 14U
#define REQ_REG_HOME_END 16U

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Construct a device descriptor that contains all the information
 * Flash Is written by 4 bytes at a time, 
 * and if it is a structure, make sure each member 
 * is 4-byte aligned
 */
typedef struct {
	uint16_t _id;
	int16_t val[6];
} _cmd_reg_t;

/**
 * Construct a device descriptor that contains all the information
 * Flash Is written by 4 bytes at a time, 
 * and if it is a structure, make sure each member 
 * is 4-byte aligned
 */
typedef struct {
	uint16_t slaveid __attribute__((aligned(4))); /**< 0x2A*/
} _write_reg_t;

/**
 * Construct a device descriptor that contains all the information
 * Flash Is written by 4 bytes at a time, 
 * and if it is a structure, make sure each member 
 * is 4-byte aligned
 */
typedef struct {
	int16_t home[2];
	int16_t cur[2];
	int16_t vel[2];
	int16_t pos[2];
} _req_reg_t;

/**
 * Construct a device descriptor that contains all the information
 * Flash Is written by 4 bytes at a time, 
 * and if it is a structure, make sure each member 
 * is 4-byte aligned
 */
typedef struct {
	uint16_t slaveid __attribute__((aligned(4))); /**< 0x2A*/
	uint16_t start;
	uint16_t _cal;
	int16_t cur[2];
	int16_t vel[2];
	int16_t pos[2];
	int16_t posTimeLim[4];
	int16_t posVelLim[4];
	int16_t curLim[2];
	int16_t velLim[2];
	int16_t velAcc[2];
	int16_t home;
	int16_t dcekp, dcekv, dceki, dcekd;
	int16_t stall;
	int16_t reset;
	int16_t reboot;
} _reg_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

extern _cmd_reg_t _cmd;
extern _write_reg_t _wri;
extern _req_reg_t _req;

/**
 * Rs485 is configured in identifier masking mode, 
 * all packets are received, and the corresponding ID packets are filtered 
 * out in the form of software masks, which is commonly 
 * referred to as software filtering
 */
void dev_rs485_cmd(uint8_t _cmd, uint8_t * RxData, uint32_t _len);

/**
 * Rs485 is configured in identifier masking mode, 
 * all packets are received, and the corresponding ID packets are filtered 
 * out in the form of software masks, which is commonly 
 * referred to as software filtering
 */
uint32_t dev_rs485_apply();

/**
 * Rs485 is configured in identifier masking mode, 
 * all packets are received, and the corresponding ID packets are filtered 
 * out in the form of software masks, which is commonly 
 * referred to as software filtering
 */
uint32_t dev_rs485_refer();

#endif /*__485_PROTOCOL_H__*/
