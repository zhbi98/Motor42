/**
 * @file led.h
 *
 */

#ifndef __LED_MX_H__
#define __LED_MX_H__

/*********************
 *      INCLUDES
 *********************/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/

/**
 * The number of bytes used by the state bits, 
 * and the time between bits is controlled 
 * by time granularity
 */
#define LED_STATE_BYTE_NUM 4U

/**********************
 *      TYPEDEFS
 **********************/

/*LED device status*/
enum{
    LED_OFF = 0, /**< LED OFF Light*/
    LED_ON, /**< LED ON Light*/
};

typedef uint8_t led_state_t;
typedef void (* led_refer_t)(uint8_t state);
typedef uint16_t led_id_t;

typedef struct {
    uint8_t valid_bits; /**< Status valid bits*/
    uint8_t offset; /**< The state bit bit is offset*/
    uint8_t count; /**< Control the number of times*/
    uint8_t modCNT : 1; /**< Whether the number of controls is set*/
    uint8_t statedef : 1; /**< Default state*/
    uint8_t _state : 1; /**< Current status*/
    uint8_t pwmdir : 1; /**< PWM increment or decrease direction*/
    uint8_t modPWM : 1; /**< PWM mode*/
    uint16_t tic; /**< Time counter*/
    uint16_t interval; /**< The time granularity of each control in 1ms*/
    union
    {
        struct
        {
            uint16_t on_time; /**< The length of time the LED is on*/
            uint16_t tic; /**< PWM timing*/
        } pwm;
        /*Note that this is a union*/
        uint8_t state[LED_STATE_BYTE_NUM]; /**< Status bits*/
    } data;
} led_attr_t;

typedef struct {
    led_attr_t _attr;
    led_refer_t refer;
} led_dev_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

int32_t led_dev_register(led_dev_t registered[], size_t led_nr);
int32_t led_dev_set_state(led_id_t _id, uint8_t state);
int32_t led_dev_set_state_by_time(led_id_t _id, uint8_t state, uint16_t time);
int32_t led_dev_set_on(led_id_t _id);
int32_t led_dev_set_off(led_id_t _id);
int32_t led_dev_set_toggle(led_id_t _id);
int32_t led_dev_set_twinkle(led_id_t _id, uint16_t time);
int32_t led_dev_twinkle_by_cnt(led_id_t _id, uint16_t time, uint8_t count, uint8_t statedef);
int32_t led_dev_set_breathe(led_id_t _id, uint16_t period);
int32_t led_dev_set_breathe_by_cnt(led_id_t _id, uint16_t period, uint8_t count, uint8_t statedef);
int32_t led_dev_custom(led_id_t _id, ...);
int32_t led_dev_custom_by_cnt(led_id_t _id, uint8_t count, uint8_t statedef, ...);
int32_t led_dev_marquee(led_id_t _id[], uint8_t led_nr, int32_t time);
int32_t led_dev_marquee_by_cnt(led_id_t _id[], uint8_t led_nr, int32_t time, uint8_t count, uint8_t statedef);
int32_t led_dev_waterfall(led_id_t _id[], uint8_t led_nr, int32_t time);
int32_t led_dev_waterfall_by_cnt(led_id_t _id[], uint8_t led_nr, int32_t time, uint8_t count, uint8_t statedef);
int32_t led_dev_task_handler();
void led_dev_tick_inc(uint32_t tick_period);

#endif /*__LED_MX_H__*/
