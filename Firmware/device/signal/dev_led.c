/**
 * @file led.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "dev_led.h"
#include <stddef.h>

/*********************
 *      DEFINES
 *********************/

/*LED Maximum time between blinks.*/
#define LED_MAX 0xFFFFU

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void _led_on_tick(led_obj_t * led_p);
static void _led_off_tick(led_obj_t * led_p);

/**********************
 * GLOBAL FUNCTIONS
 **********************/

/**
 * Non-blocking LED state machine driver
 * @param led Pointer to LED control 
 * object (must be pre-initialized)
 */
static void _led_on_tick(led_obj_t * led_p)
{
    led_p->on_tick--;
    if (led_p->on_tick <= 0) {
        led_p->state = LED_OFF;
        led_p->off_tick = MS_TO_TICKS(
            led_p->_on_time, LED_REPEAT);
    }
}

/**
 * Non-blocking LED state machine driver
 * @param led Pointer to LED control 
 * object (must be pre-initialized)
 */
static void _led_off_tick(led_obj_t * led_p)
{
    led_p->off_tick--;
    if (led_p->off_tick <= 0) {
        led_p->state = LED_ON;
        led_p->on_tick = MS_TO_TICKS(
            led_p->_off_time, LED_REPEAT);
    }
}

/**
 * Non-blocking LED state machine driver
 * @param led Pointer to LED control 
 * object (must be pre-initialized)
 */
void _led_tick_work(led_obj_t * led_p)
{
    if (led_p == NULL) return;

    if (led_p->state != LED_ON) 
        _led_off_tick(led_p);
    else _led_on_tick(led_p);

    if (led_p->state != led_p->last) {
        if (led_p->refer != NULL)
            led_p->refer(led_p);
    }

    led_p->last = led_p->state;
}

/**
 * An interface function that controls 
 * the hardware status of the LED.
 * @param led Pointer to LED control 
 * object (must be pre-initialized)
 */
void _led_set_val(led_obj_t * led_p, uint8_t val)
{
    if (led_p == NULL) return;

    if (val > 100) val = 100;
    uint8_t _r_val = 100 - val;

    uint32_t range = LED_MAX - LED_REPEAT;
    uint16_t _start = LED_REPEAT;

    range *= _r_val;
    range /= 100;

    uint16_t _target = (uint16_t)(range / 2);

    led_p->_on_time = _start + _target;
    led_p->_off_time = led_p->_on_time;
    if (val != 0) led_p->state = LED_ON;
}
