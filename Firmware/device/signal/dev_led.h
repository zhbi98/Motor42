/**
 * @file led.h
 *
 */

#ifndef __LED_H__
#define __LED_H__

/*********************
 *      INCLUDES
 *********************/

#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/*LED flash default light time(ms).*/
#define LED_ONTIME_DEF  500U

/*LED flash default off time(ms).*/
#define LED_OFFTIME_DEF 500U

/*When the LED device repeatedly updates its status, 
the update interval is.*/
#define LED_REPEAT  10U /*ms*/

/*Converts the specified time 
to a system count value.*/
#define MS_TO_TICKS(ms, base) \
    ((uint32_t)((ms) / (base)))

/**********************
 *      TYPEDEFS
 **********************/

/*LED device status*/
enum {
    LED_OFF = 0,
    LED_ON,
};

typedef uint8_t led_state_t;

/**
 * Construct an LED state 
 * control object.
 */
typedef struct _led_obj_t {
    uint8_t state;
    uint8_t last;
    uint32_t on_tick;
    uint32_t off_tick;
    uint16_t _on_time;
    uint16_t _off_time;
    void (* refer)(struct 
        _led_obj_t * led_p);
} led_obj_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void _led_tick_work(led_obj_t * led_p);
void _led_set_val(led_obj_t * led_p, 
    uint8_t val);

#endif /*__LED_H__*/
