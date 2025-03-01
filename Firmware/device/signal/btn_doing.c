/**
 * @file btnctl.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "motor_control.h"
#include "multi_button.h"
#include <stddef.h>
#include "btn_doing.h"
#include "gpio.h"
#include "ledmx.h"
#include "control_config.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void _driver_refer(uint8_t state);
static uint32_t btn_doing_get_tick();
static void _motor_btn1_long();
static void _motor_btn1_click();
static void _motor_btn2_click();

/**********************
 *  STATIC VARIABLES
 **********************/

static Motor_Mode modedef = Motor_Mode_Digital_Location;
static volatile uint32_t _btn_tick = 0;
static struct Button btn1 = {0};
static struct Button btn2 = {0};

/**********************
 * GLOBAL FUNCTIONS
 **********************/

/**
 * Reads physical button states via GPIO, must match hardware schematic configuration.
 * @param btn_id Button identifier (0: PC14, 1: PC15).
 * @return 0.
 */
static uint8_t _read_driver(uint8_t btn_id)
{
    switch (btn_id) {
    case 0: return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14); break;
    case 1: return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15); break;
    default: return 0; break;
    }
}

#define TICK_TIMES 1U

/**
 * Calibration mode entry callback, Triggers LED matrix special pattern.
 */
static void _motor_btn1_long()
{
    uint16_t time = 300 * TICK_TIMES;
    uint8_t led_id = 0;

    led_dev_twinkle_by_cnt(led_id, 
        time, 3, LED_OFF);
}

/**
 * Calibration mode entry callback, Triggers LED matrix special pattern.
 */
static void _motor_btn1_click()
{
    uint16_t time = 200 * TICK_TIMES;
    uint8_t led_id = 0;

    Motor_Mode * _rmode_p = \
            &motor_control.mode_run;

    if (*_rmode_p != Control_Mode_Stop) {
        modedef = *_rmode_p;
        *_rmode_p = Control_Mode_Stop;
    } else *_rmode_p = modedef;

    led_dev_set_state_by_time(led_id, 
        LED_ON, time);
}

/**
 * Calibration mode entry callback, Triggers LED matrix special pattern.
 */
static void _motor_btn2_click()
{
    uint16_t time = 200 * TICK_TIMES;
    uint8_t led_id = 0;

    switch (motor_control.mode_run) {
    case Motor_Mode_Digital_Location:
    case Motor_Mode_PWM_Location:
        Motor_Control_Write_Goal_Location(0);
        break;
    case Motor_Mode_Digital_Speed:
    case Motor_Mode_PWM_Speed:
        Motor_Control_Write_Goal_Speed(0);
        break;
    case Motor_Mode_Digital_Current:
    case Motor_Mode_PWM_Current:
        Motor_Control_Write_Goal_Current(0);
        break;
    }

    led_dev_set_state_by_time(led_id, 
        LED_ON, time);
}

/**
 * Initializes button control subsystem, Registers button 
 * instances with GPIO read function, binds event callbacks to hardware buttons, 
 * Btn1 Long-press triggers calibration, short-press changes mode, 
 * Btn2 Short-press modifies motor running mode parameter values.
 */
void btn_doing_start()
{
    button_init(&btn1, _read_driver, 0, 0);
    button_init(&btn2, _read_driver, 1, 1);

    button_attach(&btn1, PRESS_UP, _motor_btn1_click);
    button_attach(&btn1, LONG_PRESS_START, _motor_btn1_long);
    button_attach(&btn2, PRESS_UP, _motor_btn2_click);

    button_start(&btn1);
    button_start(&btn2);
}

/**
 * State machine scheduler (20ms base), 
 * Processes button state transitions and 
 * triggers LED feedback patterns.
 */
void btn_doing_tick_work()
{
    static uint32_t _last_tick = 0;
    static uint32_t _tick = 0;

    _tick = btn_doing_get_tick();

    if (_tick - _last_tick > 100) {
        _last_tick = btn_doing_get_tick();
        button_ticks();
    }
}

/**
 * Tick counter incrementer Called by hardware timer task
 */
void btn_doing_tick_inc(uint32_t tick_period)
{
    _btn_tick += tick_period;
}

/**
 * Each task callback function is timed and used to 
 * execute the corresponding callback function at the end of time.
 */
static uint32_t btn_doing_get_tick()
{
    return _btn_tick;
}
