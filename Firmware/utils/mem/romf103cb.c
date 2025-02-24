/**
 * @file romf103cb.c
 *
 */

/*****
  ** @file     : stockpile_f103cb.c/h
  ** @brief    : Flash存储库
  ** @versions : newest
  ** @time     : newest
  ** @reviser  : unli (WuHu China)
  ** @explain  : null
*****/

/*********************
 *      INCLUDES
 *********************/

#include "stm32f1xx_hal.h"
#include "romf103cb.h"

/*LL_Driver*/
/*#include "stm32f0xx_ll_flash_ex.h"*/

/**********************
 *      TYPEDEFS
 **********************/

/************************ Flash_Start *********************************/
/************************ Flash_Start *********************************/
/************************ Flash_Start *********************************/

/*Flash 分区表实例*/
_f103_rom_t _appfw = {APP_FIRMWARE_ADDR, APP_FIRMWARE_SIZE, 
    (APP_FIRMWARE_SIZE / ROM_PAGE_SIZE), 0};
_f103_rom_t _quick_cali = {APP_CALI_ADDR, APP_CALI_SIZE, 
    (APP_CALI_SIZE / ROM_PAGE_SIZE), 0};
_f103_rom_t stockpile_data = {APP_DATA_ADDR, APP_DATA_SIZE, 
    (APP_DATA_SIZE / ROM_PAGE_SIZE), 0};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Flash 数据清空
 * @param rom_p Flash 分区表实例
 * @return NULL
 */
void rom_data_clear(_f103_rom_t * rom_p)
{
    uint32_t page_error = 0;
    uint32_t count = 0;
    uint32_t _addr = 0;

    HAL_FLASH_Unlock(); /*LL_FLASH_Unlock();*/

    for (count = 0; count < rom_p->page_num; count++) {
        FLASH_EraseInitTypeDef erase_config = {0};
        erase_config.TypeErase = FLASH_TYPEERASE_PAGES; /*Page erasure*/
        _addr = rom_p->begin_add + (count * ROM_PAGE_SIZE);
        erase_config.PageAddress = _addr; /*Page start address*/
        erase_config.NbPages = 1;                                                                                                           //擦除页数量
        HAL_FLASHEx_Erase(&erase_config, &page_error);
        FLASH_WaitForLastOperation(HAL_MAX_DELAY);
        CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
    }

    HAL_FLASH_Lock(); /*LL_FLASH_Lock();*/
}

/**
 * Flash 数据开始写入
 * @param rom_p Flash分区表实例
 * @return NULL
 */
void rom_data_begin(_f103_rom_t * rom_p)
{
    HAL_FLASH_Unlock(); /*LL_FLASH_Unlock();*/
    rom_p->asce_write_add = rom_p->begin_add;
}

/**
 * Flash 数据结束写入
 * @param rom_p Flash 分区表实例
 * @return NULL
 */
void rom_data_end(_f103_rom_t * rom_p)
{
    HAL_FLASH_Lock(); /*LL_FLASH_Lock();*/
}

/**
 * Flash 设置写地址
 * @param rom_p Flash分区表实例
 * @param write_add 写地址
 * @return NULL
 */
void rom_write_set_addr(_f103_rom_t * rom_p, 
    uint32_t write_add)
{
    uint32_t _start = rom_p->begin_add;
    uint32_t _end = rom_p->begin_add + rom_p->area_size;

    if (write_add < _start) return;
    if (write_add > _end) return;
    rom_p->asce_write_add = write_add;
}

/**
 * Flash_16 位数据写入
 * @param rom_p Flash 分区表实例
 * @param data 半字数据缓冲区
 * @param num 半字数量
 * @return NULL
 */
void rom_write_data16(_f103_rom_t * rom_p, 
    uint16_t * data, uint32_t num)
{
    HAL_StatusTypeDef _res = HAL_ERROR;

    uint32_t _start = rom_p->begin_add;
    uint32_t _end = rom_p->begin_add + rom_p->area_size;

    if (rom_p->asce_write_add < _start) return;
    if ((rom_p->asce_write_add + num * 2) > _end) return;
    
    for (uint32_t i = 0; i < num; i++) {
        _res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 
            rom_p->asce_write_add, (uint64_t)data[i]);
        if (_res == HAL_OK)
            rom_p->asce_write_add += 2;
    }
}

/**
 * Flash_32 位数据写入
 * @param rom_p Flash分区表实例
 * @param data 字数据缓冲区
 * @param num 字数量
 * @return NULL
 */
void rom_write_data32(_f103_rom_t * rom_p, 
    uint32_t * data, uint32_t num)
{
    HAL_StatusTypeDef _res = HAL_ERROR;
    uint32_t _start = rom_p->begin_add;
    uint32_t _end = rom_p->begin_add + rom_p->area_size;

    if (rom_p->asce_write_add < _start) return;
    if ((rom_p->asce_write_add + num * 4) > _end) return;
    
    for (uint32_t i = 0; i < num; i++) {
        _res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 
            rom_p->asce_write_add, (uint64_t)data[i]);

        if (_res == HAL_OK)
            rom_p->asce_write_add += 4;
    }
}

/**
 * Flash_64 位数据写入
 * @param rom_p Flash 分区表实例
 * @param data 双字数据缓冲区
 * @param num 双字数量
 * @return NULL
 */
void rom_write_data64(_f103_rom_t * rom_p, 
    uint64_t * data, uint32_t num)
{
    HAL_StatusTypeDef _res = HAL_ERROR;
    uint32_t _start = rom_p->begin_add;
    uint32_t _end = rom_p->begin_add + rom_p->area_size;

    if (rom_p->asce_write_add < _start) return;
    if ((rom_p->asce_write_add + num * 8) > _end) return;

    for (uint32_t i = 0; i < num; i++) {
        _res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, 
            rom_p->asce_write_add, (uint64_t)data[i]);
        if (_res == HAL_OK)
            rom_p->asce_write_add += 8;
    }
}

/****************** Flash_End *****************************/
/****************** Flash_End *****************************/
/****************** Flash_End *****************************/
