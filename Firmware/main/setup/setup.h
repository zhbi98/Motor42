/**
 * @file setup.h
 *
 */

#ifndef __SETUP_H__
#define __SETUP_H__

/*********************
 *      INCLUDES
 *********************/

#include <stdbool.h>
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint8_t head[5 + 1];
    int32_t current_down_acc;
    int32_t current_up_acc;
    int32_t current_rated;

    int32_t speed_down_acc;
    int32_t speed_up_acc;
    int32_t speed_rated;

    int32_t dce_kp;
    int32_t dce_kv;
    int32_t dce_ki;
    int32_t dce_kd;

    int32_t cali_current;

    uint32_t can_id;
    uint32_t modedef;
    int32_t home_ofs;

    bool motor_onboot;
    bool stall_protect;
    uint8_t ssid[8 + 1];
} _setup_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Writes the changed parameters to the file system, next time can 
 * extract the settings from the file system 
 * to update to the hardware drive
 */
void operate_file(uint8_t operate);

/**
 * Writes the changed parameters to the file system, next time can 
 * extract the settings from the file system 
 * to update to the hardware drive
 */
void file_tick_work();

/**
 * Writes the changed parameters to the file system, next time can 
 * extract the settings from the file system 
 * to update to the hardware drive
 */
void write_file();

/**
 * Extract the settings from the file system 
 * to update to the hardware drive
 */
void read_file();

/**
 * Clears and restores all user settings 
 * to their default values.
 * At the same time, the data used for 
 * observation also needs to be restored 
 * to the default values.
 */
void reset_file();

extern _setup_t _setup;

#endif /*__SETUP_H__*/
