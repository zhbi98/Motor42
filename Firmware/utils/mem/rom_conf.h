/**
 * @file can_dev.h
 *
 */

#ifndef __ROM_CONF_H__
#define __ROM_CONF_H__

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/*ROM sizes*/
/*ROM sizes*/
/*ROM sizes*/

/*DAPLINK_ROM_BL*/
#define DAPLINK_ROM_BL_START (0x08000000) /*起始地址*/
#define DAPLINK_ROM_BL_SIZE  (0x0000BC00) /*Flash 容量 47K DAPLink_BL(DAPLINK_ROM_BL)*/

/*DAPLINK_ROM_CONFIG_ADMIN*/
#define DAPLINK_ROM_CONFIG_ADMIN_START (0x0800BC00) /*起始地址*/
#define DAPLINK_ROM_CONFIG_ADMIN_SIZE  (0x00000400) /*Flash容量 1K DAPLink_BL(DAPLINK_ROM_CONFIG_ADMIN)*/

/*APP_FIRMWARE*/
#define APP_FIRMWARE_ADDR    (0x08000000) /*(0x0800C000) 起始地址*/
#define APP_FIRMWARE_SIZE    (0x0000BC00) /*Flash容量 47K XDrive(APP_FIRMWARE)*/

/*APP_CALI*/
#define APP_CALI_ADDR        (0x08017C00) /*起始地址*/
#define APP_CALI_SIZE        (0x00008000) /*Flash 容量 32K XDrive(APP_CALI)(可容纳16K-2byte校准数据-即最大支持14位编码器的校准数据)*/

/*APP_DATA*/
#define APP_DATA_ADDR        (0x0801FC00) /*起始地址*/
#define APP_DATA_SIZE        (0x00000400) /*Flash容量 1K XDrive(APP_DATA)*/

/*RAM sizes*/
/*RAM sizes*/
/*RAM sizes*/

#define APP_RAM_START    (0x20000000)
#define APP_RAM_SIZE     (0x00004F00) /*19K768字节*/

#define SHARED_RAM_START (0x20004F00)
#define SHARED_RAM_SIZE  (0x00000100) /*256 字节*/

#endif /*__ROM_CONF_H__*/
