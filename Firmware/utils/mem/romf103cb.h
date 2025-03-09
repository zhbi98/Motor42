/**
 * @file romf103cb.h
 *
 */

#ifndef __ROM_F103CB_H__
#define __ROM_F103CB_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "main.h"
#include "rom_conf.h"
#include <stdint.h>

/*************************** FLASH_Start ******************************/
/*************************** FLASH_Start ******************************/
/*************************** FLASH_Start ******************************/
/******************页配置(更换芯片必须修改这个配置)***********************/

/*********************
 *      DEFINES
 *********************/

#define ROM_PAGE_SIZE 0x400U /*扇区大小(默认 1024 字节)*/
/*和 HAL 库获取的 Flash 页大小比较，检查配置是否有效*/
#if (ROM_PAGE_SIZE != FLASH_PAGE_SIZE)
    #error "ROM_PAGE_SIZE Error !!!"      
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Constructing a Flash partition 
 * table structure
 */
typedef struct{
    uint32_t begin_add; /**< Start address*/
    uint32_t area_size; /**< Storage area size*/
    uint32_t page_num;  /**< The number of physical chip pages*/
    uint32_t asce_write_add; /**< Write address*/
} _f103_rom_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void rom_data_clear(_f103_rom_t * rom_p);
void rom_data_begin(_f103_rom_t * rom_p);
void rom_data_end(_f103_rom_t * rom_p);
void rom_write_set_addr(_f103_rom_t * rom_p, uint32_t write_add);
void rom_write_data16(_f103_rom_t * rom_p, uint16_t *data, uint32_t num);
void rom_write_data32(_f103_rom_t * rom_p, uint32_t *data, uint32_t num);
void rom_write_data64(_f103_rom_t * rom_p, uint64_t *data, uint32_t num);

/********** Flash分区表实例 **********/
extern _f103_rom_t _appfw;
extern _f103_rom_t _quick_cali;
extern _f103_rom_t stockpile_data;

/********************** FLASH_End *******************************/
/********************** FLASH_End *******************************/
/********************** FLASH_End *******************************/

#ifdef __cplusplus
}
#endif

#endif /*__ROM_F103CB_H__*/
