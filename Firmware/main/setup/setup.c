/**
 * @file setup.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "control_config.h"
#include "motor_control.h"
#include <string.h>
#include "romf103cb.h"
#include "setup.h"
#include "time.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

const _setup_t _setup_def = {
    .head = {".X42."},

    .can_id = 0x01,
    .home_ofs = 0,
    .modedef = Motor_Mode_Digital_Location,

    .speed_down_acc = 100 * Move_Pulse_NUM,
    .speed_up_acc = 100 * Move_Pulse_NUM,
    .speed_rated = 30 * Move_Pulse_NUM,

    .current_down_acc = 2 * 1000, /*(mA/s)*/
    .current_up_acc = 2 * 1000, /*(mA/s)*/
    .current_rated = 1000,
    .cali_current = 2000,

    .dce_kp = 200,
    .dce_kv = 80,
    .dce_ki = 300,
    .dce_kd = 250,

    .motor_onboot = false,
    .stall_protect = false,

    .ssid = {".Bin-X42"},
};

/**********************
 *  STATIC VARIABLES
 **********************/

static bool _refer = false;
static uint8_t _operate = 0;
static _setup_t setup_verify = {0};
_setup_t _setup = {0};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static bool verify(void * p1, void * p2, uint32_t n);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * A content observer to see if the data has changed, 
 * returning an error if the content is different 
 * from the original, or the correct result.
 * @param p1 pointer to the source data.
 * @param p2 pointer to the verify data.
 * @param n data size.
 * @return ture or false.
 */
static bool verify(void * p1, void * p2, uint32_t n)
{
    uint8_t * data_p1 = (uint8_t *)p1;
    uint8_t * data_p2 = (uint8_t *)p2;

    for (uint32_t i = 0; i < n; i++)
        if (data_p1[i] != data_p2[i])
            return false;
    return true;
}

/**
 * Writes the changed parameters to the file system, next time can 
 * extract the settings from the file system 
 * to update to the hardware drive
 */
void operate_file(uint8_t operate)
{
    if (_refer != true) _refer = true;
    _operate = operate;
}

/**
 * Writes the changed parameters to the file system, next time can 
 * extract the settings from the file system 
 * to update to the hardware drive
 */
void file_tick_work()
{
    if (!_refer) return;

    if (!_operate) write_file();
    if (_operate) reset_file();

    _refer = false;
}

/**
 * Writes the changed parameters to the file system, next time can 
 * extract the settings from the file system 
 * to update to the hardware drive
 */
void write_file()
{
    void * data_p = (void *)(&_setup);

    __disable_irq();
    sleep_ms(50);

    /*Observe whether the data has changed*/
    bool ify = verify(&_setup, &setup_verify, 
        sizeof(_setup_t));

    /*The file has not changed and does not need 
    to be written to memory to save*/
    if (ify) {
        sleep_ms(50);
        __enable_irq();
        return;
    }

    /*Erase the data area*/
    rom_data_clear(&stockpile_data);
    /*Start writing the data area*/
    rom_data_begin(&stockpile_data);

    rom_write_data16(&stockpile_data, 
        (uint16_t *)data_p, 
        sizeof(_setup_t) / 2);

    /*Finish writing the data area*/
    rom_data_end(&stockpile_data);

    sleep_ms(50);
    __enable_irq();
}

/**
 * Extract the settings from the file system 
 * to update to the hardware drive
 */
void read_file()
{
    uint16_t file_size = sizeof(_setup_t);
    bool inv_file = 1;

    uint8_t * data_p = (uint8_t *)APP_DATA_ADDR;
    memcpy((uint8_t *)&_setup, 
        data_p, sizeof(_setup_t));

    const uint8_t * head_p = ".X42.";
    const uint8_t * ssid_p = ".Bin-X42";

    bool res1 = verify(
        &_setup.head[0], head_p, 5);
    bool res2 = verify(
        &_setup.ssid[0], ssid_p, 8);

    if ((res1 != true) || 
        (res2 != true)) {
        memcpy(&_setup, &_setup_def, 
            file_size);

        __disable_irq();
        /*Erase the data area*/
        rom_data_clear(&stockpile_data);
        /*Start writing the data area*/
        rom_data_begin(&stockpile_data);

        rom_write_data16(&stockpile_data, 
            (uint16_t *)&_setup, 
            sizeof(_setup_t) / 2);

        /*Finish writing the data area*/
        rom_data_end(&stockpile_data);

        __enable_irq();
    }

    /*Reads data from a permanent storage 
    device into dynamic memory*/
    uint8_t * _data_p = (uint8_t *)(&_setup);
    /*The maximum amount of data stored is 65kbyte*/
    uint16_t byte_addr = 0;
    uint16_t _verify = 0;

    /*Reads data from a permanent storage 
    device into dynamic memory*/
    while (byte_addr < file_size) {
        if (_data_p[byte_addr] != 0xFF)
            _verify++;
        byte_addr++;
    }

    if (_verify > (file_size / 3)) 
        inv_file = 0;

    /*Make the calibration mark valid 
      after performing the calibration*/
    /*Data in permanent storage is valid*/
    if (inv_file) {
        memcpy(&_setup, &_setup_def, 
            file_size);

        __disable_irq();
        /*Erase the data area*/
        rom_data_clear(&stockpile_data);
        /*Start writing the data area*/
        rom_data_begin(&stockpile_data);

        rom_write_data16(&stockpile_data, 
            (uint16_t *)&_setup, 
            sizeof(_setup_t) / 2);

        /*Finish writing the data area*/
        rom_data_end(&stockpile_data);

        __enable_irq();
    }

    memcpy(&setup_verify, &_setup, file_size);
}

/**
 * Clears and restores all user settings 
 * to their default values.
 * At the same time, the data used for 
 * observation also needs to be restored 
 * to the default values.
 */
void reset_file()
{
    memcpy(&_setup, &_setup_def, sizeof(_setup_t));
    write_file();

    HAL_NVIC_SystemReset();
}
