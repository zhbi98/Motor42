/**
 * @file led.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "motor_control.h"
#include "led_anim.h"
#include "ledmx.h"
#include "gpio.h"
#include <stddef.h>
#include "mt6816.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void _driver_refer(uint8_t state);
static void _stall();
static void _overload();
static void _uncalibrated();
static void _system_ready();

/**********************
 *  STATIC VARIABLES
 **********************/

static Motor_State _stateM = Control_State_Stop;
static volatile uint32_t _led_tick = 0;
static uint8_t _caliOK = 0xFF;
static uint8_t _readyOK = 0xFF;

static led_dev_t led_device[1] = {
    {.refer = _driver_refer},
};

/**********************
 * GLOBAL FUNCTIONS
 **********************/

/**
 * Direct GPIO control for LED hardware, uses PB0 pin for LED output.
 * @param state Boolean value 0 LED OFF 1 LED ON.
 */
static void _driver_refer(uint8_t state)
{
    if (state) {
        HAL_GPIO_WritePin(GPIOB, 
            GPIO_PIN_12, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(GPIOB, 
            GPIO_PIN_12, GPIO_PIN_RESET);
    }
}

/**
 * State machine tick handler (20ms cycle) 
 * motor state transitions and calibration status, 
 * triggers LED pattern callbacks based on system events.
 */
void led_anim_tick_work()
{
    Motor_State stateM = motor_control.state;
    uint8_t caliOK = _enc_dev_calibrated();
    uint8_t readyOK = 1;

    if (stateM != _stateM) {
        if (stateM == Control_State_Overload) _overload();
        else if (stateM == Control_State_Stall) _stall();
        _stateM = stateM;
    }
    else
    if (caliOK != _caliOK) {
        if (!caliOK) _uncalibrated();
        _caliOK = caliOK;
    }
    else
    if (_readyOK != readyOK) {
        if (readyOK) _system_ready();
        _readyOK = readyOK;
    }
}

/**
 * Each task callback function is timed 
 * and used to execute the corresponding 
 * callback function at the end of time.
 */
void led_anim_tick_inc(uint32_t tick_period)
{
    static uint32_t _last_tick = 0;
    static uint32_t _tick = 0;

    _led_tick += tick_period;
    _tick = _led_tick;

    if (_tick - _last_tick > 20) {
        _last_tick = _led_tick;
        led_dev_tick_inc(1); 
    }
}

/**
 * Initializes LED device in control system, 
 * registers LED control instance with device manager.
 */
void led_anim_start()
{
    led_dev_register(led_device, 1);
}

#define TICK_TIMES 1U

/**
 * Stall indication pattern (300ms period, 2 blinks)
 * error state visualization through timed toggling.
 */
static void _stall()
{
    uint16_t time = 300 * TICK_TIMES;
    uint8_t led_id = 0;

    led_dev_twinkle_by_cnt(led_id, 
        time, 3, LED_OFF);
}

/**
 * Over load indication pattern (500ms period, 3 blinks)
 * error state visualization through timed toggling.
 */
static void _overload()
{
    uint16_t time = 300 * TICK_TIMES;
    uint8_t led_id = 0;

    led_dev_twinkle_by_cnt(led_id, 
        time, 2, LED_OFF);
}

/**
 * Magnetic encoders calibrated indication pattern (500ms period, 1 blinks)
 * error state visualization through timed toggling.
 */
static void _uncalibrated()
{
    uint16_t time = 500 * TICK_TIMES;
    uint8_t led_id = 0;

    led_dev_twinkle_by_cnt(led_id, 
        time, 10, LED_OFF);
}

/**
 * System start indication pattern (300ms period, 2 blinks)
 * error state visualization through timed toggling.
 */
static void _system_ready()
{
    uint16_t time = 1500 * TICK_TIMES;
    uint8_t led_id = 0;

    led_dev_set_state_by_time(led_id, 
        LED_ON, time);
}
