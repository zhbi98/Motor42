/**
 * @file dev_key.h
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "dev_key.h"

/**********************
 *      TYPEDEFS
 **********************/

static _key_event_t key_evt = {
    .phold = false,
    .dura = 400,
    .interval = 29,
};

/**********************
 * GLOBAL FUNCTIONS
 **********************/

/**
 * Generates keyboard drive code by 
 * reading physical key status.
 * @return 8-bit drive code pattern: Bit7-Bit6 Physical keys, 
 * Bit5-Bit0 Reserved bits (fixed to 1).     
 */
static uint8_t key_drive_code()
{
    uint8_t val = 0xFF;

    val = (val & 0x7F) | key0_level();
    val = (val & 0xBF) | key1_level();

    return val;
}

/**
 * Decrements timing counters for key event handling.
 * Should be called periodically (e.g., every 10ms).
 */
static void key_tick_work()
{
    if (key_evt.interval != 0) key_evt.interval--;
    if (key_evt.dura != 0) key_evt.dura--;
}

/**
 * Detects and processes keyboard events
 * @return Detected key event code: KEY0_EVT(0x7F): 
 * key0 pressed, KEY1_EVT(0xBF): key1 pressed, 
 * RELEASE(0x00): No key activity.
 */
uint8_t key_get_event()
{
    static uint8_t k_old = RELEASE;
    uint8_t drive_code = 0;
    uint8_t val = 0;

    drive_code = key_drive_code();

    switch (drive_code) {
    case 0x7F:
        val = KEY0_EVT;
        break;
    case 0xBF:
        val = KEY1_EVT;
        break;
    default:
        val = RELEASE;
        break;
    }

    if (val == RELEASE) key_evt.dura = 400;


    /*key2 Unsupport long press auto trigger active*/
    if (key_evt.dura <= 0) key_evt.phold = true;
    else key_evt.phold = false;

    key_tick_work();

    if (key_evt.phold == true) {
        if (key_evt.interval <= 0) {
            key_evt.interval = 29;
            return val;
        } else return RELEASE;
    }

    /**
     * Pressed event
     * if ((val != RELEASE) && (k_old == RELEASE)) {
     *     k_old = val;
     *     return val;
     * } else {
     *     k_old = val;
     *     return RELEASE;
     * }
     */

    /*Released event */
    if ((val == RELEASE) && (k_old != RELEASE)) {
        drive_code = k_old;
        k_old = RELEASE;
        return drive_code;
    } else {
        k_old = val;
        return RELEASE;
    }

    return 0;
}
