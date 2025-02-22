/**
 * @file led.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "dev_led.h"

/*********************
 *      DEFINES
 *********************/

#define LED_ONTIME  200U /*ms*/
#define LED_OFFTIME 200U /*ms*/
#define LED_REPEAT  100U /*ms*/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void led_set_state(uint8_t state);

/**********************
 *      TYPEDEFS
 **********************/

led_obj_t led1 = {
    .on_tick = MS_TO_TICKS(
        LED_ONTIME, LED_REPEAT),
    .off_tick = MS_TO_TICKS(
        LED_OFFTIME, LED_REPEAT),

    .last_state = LED_OFF,
    .state = LED_OFF,
    .color = GRAY,
    .led_cb = led_set_state,
};

/**********************
 * GLOBAL FUNCTIONS
 **********************/

/**
 * Non-blocking LED state machine driver
 * @param led Pointer to LED control object (must be pre-initialized)
 */
void led_light_work(led_obj_t * led)
{
    if (led == NULL) return;

    if (led->state == LED_ON) {
        led->on_tick--;
        if (led->on_tick <= 0) {
            led->state = LED_OFF;
            led->off_tick = MS_TO_TICKS(
                LED_OFFTIME, LED_REPEAT);
        }
    } else {
        led->off_tick--;
        if (led->off_tick <= 0) {
            led->state = LED_ON;
            led->on_tick = MS_TO_TICKS(
                LED_ONTIME, LED_REPEAT);
        }
    }

    if (led->state != led->last_state) {
        led->led_cb(led->state);
    }

    led->last_state = led->state;
}

/**
 * An interface function that controls the hardware status of the LED.
 * @param state Target state (0: LED off, non-0: LED on).
 */
static void led_set_state(uint8_t state)
{
    if (state) {}
    else {}
}
