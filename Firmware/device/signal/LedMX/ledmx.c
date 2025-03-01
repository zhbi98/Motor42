/**
 * @file led.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "ledmx.h"
#include <string.h>
#include <stdarg.h>

/*********************
 *      DEFINES
 *********************/

#define FILL_STATE_BITS(state, sbit, ebit) { \
    int32_t bit = 0; \
    for (bit = (sbit); bit < (ebit); bit++) \
        (state)[bit / 8] |= (0x01 << (bit % 8)); \
}

#define SET_STATE_BITS(state, bit) { \
    (state)[bit / 8] |= (0x01 << (bit % 8)); \
}

#define LED_ABS(a) ((a) > 0 ? (a) : -(a))

/*呼吸灯软件模拟PWM频率 (Hz)*/
#define BREATHE_PWM_RATE 50U
#define BREATHE_PWM_PERIOD (1000 / BREATHE_PWM_RATE)
#define TIME_MAX_VAL (uint32_t)0xFFFF

/**********************
 *  STATIC PROTOTYPES
 **********************/

static uint8_t _req_pwmled_state(led_attr_t *_attr_p);
static void _pwmled_state_update(led_attr_t *_attr_p);
static led_dev_t * registered_p = NULL;
static size_t register_nr = 0;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Register the LED device with the manager.
 * @param registered indicator array table pointer.
 * @param led_nr indicates the number of elements in the array of lights.
 * @return Status update was successful.
 */
int32_t led_dev_register(led_dev_t registered[], size_t led_nr)
{
    uint8_t _id = 0;

    registered_p = registered;
    register_nr = led_nr;

    for (_id = 0; _id < led_nr; _id++)
        memset(&registered_p[_id]._attr, 
            0, sizeof(led_attr_t));

    return 0;
}

/**
 * Set LED device default parameters.
 * @param _attr pointer to an 'led_attr_t' LED device attribute.
 */
void led_dev_attr_default_init(led_attr_t * _attr)
{
    uint8_t size =  LED_STATE_BYTE_NUM * sizeof(uint8_t);

    memset(_attr->data.state, 0, size);
    memset(&_attr->data.pwm, 0, size);

    _attr->interval = 100;
    _attr->tic = 0;
    _attr->offset = 0;
    _attr->valid_bits = 0;
    _attr->count = 0;
    _attr->modCNT = 0;
    _attr->statedef = 0;
    _attr->modPWM = 0;
}

/**
 * Set the LED on and off.
 * @param LED designation indicator.
 * @param _id Specify the LED device number.
 * @param state Sets the state of the specified LED device.
 * @return Status update was successful.
 */
int32_t led_dev_set_state(led_id_t _id, uint8_t state)
{
    if (registered_p == NULL) return - 1;
    if (register_nr == 0) return - 1;
    if (_id >= register_nr) return - 1;

    led_attr_t * _attr = &registered_p[_id]._attr;
    led_dev_attr_default_init(_attr);

    _attr->interval = 100;
    if (state != LED_OFF) _attr->data.state[0] = 0x01;
    else  _attr->data.state[0] = 0x00;
    _attr->valid_bits = 1;

    return 0;
}

/**
 * Set the LED on and off and the duration
 * @param _id Specify the LED device number.
 * @param state Sets the state of the specified LED device.
 * @param time Duration of the state.
 * @return Status update was successful.
 */
int32_t led_dev_set_state_by_time(led_id_t _id, uint8_t state, 
    uint16_t time)
{
    if (registered_p == NULL) return - 1;
    if (register_nr == 0) return - 1;
    if (_id >= register_nr) return - 1;

    led_attr_t * _attr = &registered_p[_id]._attr;
    led_dev_attr_default_init(_attr);
    
    if (state != LED_OFF) _attr->data.state[0] = 0x01;
    else _attr->data.state[0] = 0x00;

    if (state != LED_OFF) _attr->statedef = 0;
    else _attr->statedef = 1;

    _attr->interval = time;
    _attr->valid_bits = 1;
    _attr->count = 1;
    _attr->modCNT = 1;

    return 0;
}

/**
 * Light up the LED.
 * @param _id Specify the LED device number.
 * @return Status update was successful.
 */
int32_t led_dev_set_on(led_id_t _id)
{
    return led_dev_set_state(_id, LED_ON);
}

/**
 * Light OFF the LED.
 * @param _id Specify the LED device number.
 * @return Status update was successful.
 */
int32_t led_dev_set_off(led_id_t _id)
{
    return led_dev_set_state(_id, LED_OFF);
}

/**
 * Flip the specified LED on and off state.
 * @param _id Specify the LED device number.
 * @return Status update was successful.
 */
int32_t led_dev_set_toggle(led_id_t _id)
{
    if (registered_p == NULL) return - 1;
    if (register_nr == 0) return - 1;
    if (_id >= register_nr) return - 1;

    led_attr_t * _attr = &registered_p[_id]._attr;
    uint8_t _state = _attr->data.state[0];

    led_dev_attr_default_init(_attr);

    if (_state == 0) _attr->data.state[0] = 0x01;
    else _attr->data.state[0] = 0x00;

    _attr->interval = 100;
    _attr->valid_bits = 1;

    return 0;
}

/**
 * Set the LED to flashing state.
 * @param _id Specify the LED device number.
 * @param time between flashing on and off, in milliseconds.
 * @return Status update was successful.
 */
int32_t led_dev_set_twinkle(led_id_t _id, uint16_t time)
{
    if (registered_p == NULL) return - 1;
    if (register_nr == 0) return - 1;
    if (_id >= register_nr) return - 1;

    led_attr_t * _attr = &registered_p[_id]._attr;
    led_dev_attr_default_init(_attr);

    _attr->interval = time;
    _attr->data.state[0] = 0x01;
    _attr->valid_bits = 2;

    return 0;
}

/**
 * Set the LED to flashing state.
 * @param _id Specify the LED device number.
 * @param time between flashing on and off, in milliseconds.
 * @return Status update was successful.
 */
int32_t led_dev_twinkle_by_cnt(led_id_t _id, uint16_t time, 
    uint8_t count, uint8_t statedef)
{
    if (registered_p == NULL) return - 1;
    if (register_nr == 0) return - 1;
    if (_id >= register_nr) return - 1;

    led_attr_t * _attr = &registered_p[_id]._attr;
    led_dev_attr_default_init(_attr);

    /*根据默认状态最后一次闪烁时状态设置相反状态达到较好的闪烁效果*/
    if (statedef != LED_OFF) _attr->data.state[0] = 0x01;
    else _attr->data.state[0] = 0x02;

    if (statedef != LED_OFF) _attr->statedef = 1;
    else _attr->statedef = 0;

    _attr->valid_bits = 2;
    _attr->count = count;
    _attr->modCNT = 1;
    _attr->interval = time;

    return 0;
}

/**
 * Set as a marquee for multiple LEDs.
 * @param _id LED ID group.
 * @param led_nr Number of LEDs.
 * @param time LED light on time.
 * @return Status update was successful.
 */
int32_t led_dev_marquee(led_id_t _id[], uint8_t led_nr, int32_t time)
{
    led_attr_t * _attr = NULL;

    if (registered_p == NULL) return - 1;
    if (register_nr == 0) return - 1;

    for (uint8_t i = 0; i < led_nr; i++) {
        if (_id[i] < register_nr) {
            _attr = &registered_p[_id[i]]._attr;
            led_dev_attr_default_init(_attr);
            _attr->interval = time;
            SET_STATE_BITS(_attr->data.state, i);
            _attr->valid_bits = led_nr;
        }
    }

    return 0;
}

/**
 * Set the marquee and the number of times for multiple LEDs, 
 * and the last time is on and off after the last number of times is completed.
 * @param _id LED ID group.
 * @param led_nr Number of LEDs.
 * @param time LED light on time.
 * @param ount the number of marquees.
 * @param statedef The state of the marquee after the number of times it is completed.
 * @return Status update was successful.
 */
int32_t led_dev_marquee_by_cnt(led_id_t _id[], uint8_t led_nr, 
    int32_t time, uint8_t count, uint8_t statedef)
{
    led_attr_t * _attr = NULL;

    if (registered_p == NULL) return - 1;
    if (register_nr == 0) return - 1;

    for (uint8_t i = 0; i < led_nr; i++) {
        if (_id[i] < register_nr) {
            _attr = &registered_p[_id[i]]._attr;
            led_dev_attr_default_init(_attr);

            _attr->interval = time;
            SET_STATE_BITS(_attr->data.state, i);

            _attr->valid_bits = led_nr;
            _attr->count = count;
            _attr->modCNT = 1;

            if (statedef != LED_OFF) _attr->statedef = 1;
            else _attr->statedef = 0;
        }
    }

    return 0;
}

/**
 * Set up running lights for multiple LEDs.
 * @param _id LED ID group.
 * @param led_nr Number of LEDs.
 * @param time LED light on time.
 * @return Status update was successful.
 */
int32_t led_dev_waterfall(led_id_t _id[], uint8_t led_nr, int32_t time)
{
    led_attr_t * _attr = NULL;

    if (registered_p == NULL) return - 1;
    if (register_nr == 0) return - 1;

    for (uint8_t i = 0; i < led_nr; i++) {
        if (_id[i] < register_nr) {
            _attr = &registered_p[_id[i]]._attr;
            led_dev_attr_default_init(_attr);
            _attr->interval = time;
            FILL_STATE_BITS(_attr->data.state, i, led_nr);
            _attr->valid_bits = led_nr + 1;
        }
    }

    return 0;
}

/**
 * Set the running light and the number of times for multiple LEDs, 
 * and the state will be on and off after the last number of times is completed.
 * @param _id LED ID group.
 * @param led_nr Number of LEDs.
 * @param time LED light on time.
 * @param count the number of running lights.
 * @param statedef The state of the number of times the LED is completed.
 * @return Status update was successful.
 */
int32_t led_dev_waterfall_by_cnt(led_id_t _id[], uint8_t led_nr, 
    int32_t time, uint8_t count, uint8_t statedef)
{
    led_attr_t * _attr = NULL;

    if (registered_p == NULL) return - 1;
    if (register_nr == 0) return - 1;

    for (uint8_t i = 0; i < led_nr; i++) {
        if (_id[i] < register_nr) {
            _attr = &registered_p[_id[i]]._attr;
            led_dev_attr_default_init(_attr);

            _attr->interval = time;
            FILL_STATE_BITS(_attr->data.state, i, led_nr);

            _attr->valid_bits = led_nr + 1;
            _attr->count = count;
            _attr->modCNT = 1;
            
            if (statedef != LED_OFF) _attr->statedef = 1;
            else _attr->statedef = 0;
        }
    }

    return 0;
}

/**
 * Set the designated LED to breathing light mode
 * @param _id Specify the LED device number.
 * @param period respiration cycle.
 * @return Status update was successful.
 */
int32_t led_dev_set_breathe(led_id_t _id, uint16_t period)
{
    if (registered_p == NULL) return - 1;
    if (register_nr == 0) return - 1;

    led_attr_t * _attr = &registered_p[_id]._attr;
    led_dev_attr_default_init(_attr);

    if (_id < register_nr) {
        registered_p[_id]._attr.interval = period / 2 / BREATHE_PWM_PERIOD;
        /*根据默认状态最后一次闪烁时状态设置相反状态达到较好的闪烁效果*/
        _attr->valid_bits = 2;
        _attr->modPWM = 1;
        _attr->pwmdir = 1;
    }

    return 0;
}

/**
 * Set the specified LED to the breathing light mode and the number of times, 
 * and the last time it will be on and off
 * @param _id LED ID group.
 * @param period respiration cycle.
 * @param count times.
 * @param statedef on and off state after the number of times.
 * @return Status update was successful.
 */
int32_t led_dev_set_breathe_by_cnt(led_id_t _id, uint16_t period, 
    uint8_t count, uint8_t statedef)
{
    if (registered_p == NULL) return - 1;
    if (register_nr == 0) return - 1;

    led_attr_t * _attr = &registered_p[_id]._attr;
    led_dev_attr_default_init(_attr);

    if (_id < register_nr) {
        if (statedef != LED_OFF) _attr->statedef = 1;
        else _attr->statedef = 0;
        _attr->interval = period / 2 / BREATHE_PWM_PERIOD;
        /*根据默认状态最后一次闪烁时状态设置相反状态达到较好的闪烁效果*/
        _attr->valid_bits = 2;
        _attr->count = count;
        _attr->modCNT = 1;
        _attr->modPWM = 1;
        _attr->pwmdir = 1;
    }

    return 0;
}

/**
 * Set the custom state of the LED.
 * @param _id LED ID group.
 * @return Status update was successful.
 */
int32_t led_dev_custom(led_id_t _id, ...)
{
    uint8_t bits;
    uint8_t offset = 0;
    uint8_t valid_bits = 0;
    va_list timeList;
    int32_t time;
    uint16_t interval = 0xFFFF;

    if (registered_p == NULL) return - 1;
    if (register_nr == 0) return - 1;
    if (_id >= register_nr) return - 1;

    led_attr_t * _attr = &registered_p[_id]._attr;
    led_dev_attr_default_init(_attr);

    va_start(timeList, _id);

    do {
        time = va_arg(timeList, int32_t);
        if (time != 0 && LED_ABS(time) < interval)
            interval = (uint16_t)LED_ABS(time);
    } while (time != 0);
    
    va_end(timeList);
    
    valid_bits = 0;
    offset = 0;

    va_start(timeList, _id);

    do {
        time = va_arg(timeList, int32_t);
        if (LED_ABS(time) != 0) {
            bits = (uint16_t)LED_ABS(time) / interval;
            
            if (time > 0) {
                for (int32_t i = 0; i < bits; i++) {
                    _attr->data.state[offset / 8] |= 0x01 << (offset % 8);
                    offset++;
                }
            }
            else {
                for (int32_t i = 0; i < bits; i++) {
                    _attr->data.state[offset / 8] |= 0x00 << (offset % 8);
                    offset++;
                }
            }
            
            valid_bits += bits;
        }
    } while (time != 0);
    
    va_end(timeList);
    _attr->interval = interval;
    _attr->valid_bits = valid_bits;

    return 0;
}


/**
 * Set the custom status and number of times of the LED, and the state 
 * of on and off after the last time is completed.
 * @param _id LED ID group.
 * @param count the number of times you can customize the status.
 * @param statedef The state of the number of custom state times is completed.
 * @return Status update was successful.
 */
int32_t led_dev_custom_by_cnt(led_id_t _id, uint8_t count, 
    uint8_t statedef, ...)
{
    uint8_t bits;
    uint8_t offset = 0;
    uint8_t valid_bits = 0;
    va_list timeList;
    int32_t time;
    uint16_t interval = 0xFFFF;
    
    if (registered_p == NULL) return - 1;
    if (register_nr == 0) return - 1;
    if (_id >= register_nr) return - 1;

    led_attr_t * _attr = &registered_p[_id]._attr;
    led_dev_attr_default_init(_attr);

    va_start(timeList, statedef);

    do {
        time = va_arg(timeList, int32_t);
        if (time != 0 && LED_ABS(time) < interval)
            interval = (uint16_t)LED_ABS(time);
    } while (time != 0);
    
    va_end(timeList);
    
    valid_bits = 0;
    offset = 0;

    va_start(timeList, statedef);

    do {
        time = va_arg(timeList, int32_t);
        if (LED_ABS(time) != 0) {
            bits = (uint16_t)LED_ABS(time) / interval;
            
            if (time > 0) {
                for (int32_t i = 0; i < bits; i++) {
                    _attr->data.state[offset / 8] |= 0x01 << (offset % 8);
                    offset++;
                }
            }
            else {
                for (int32_t i = 0; i < bits; i++) {
                    _attr->data.state[offset / 8] |= 0x00 << (offset % 8);
                    offset++;
                }
            }
            
            valid_bits += bits;
        }
    } while (time != 0);
    
    va_end(timeList);
    _attr->interval = interval;
    _attr->valid_bits = valid_bits;
    _attr->count = count;
    _attr->modCNT = count == 0 ? 0 : 1;
    _attr->statedef = statedef == LED_ON ? 1 : 0;

    return 0;
}

/**
 * LED control cycle tasks.
 * @param sysTime system runtime, in milliseconds.
 * @return Status update was successful.
 */
static uint8_t _req_pwmled_state(led_attr_t *_attr_p)
{
    uint8_t _led_state;

    if (_attr_p->data.pwm.tic < _attr_p->data.pwm.on_time)
        _led_state = LED_ON;
    else _led_state = LED_OFF;

    if (_attr_p->data.pwm.tic >= BREATHE_PWM_PERIOD)
        _attr_p->data.pwm.tic = 0;

    return _led_state;
}

/**
 * LED control cycle tasks.
 * @param[in] sysTime system runtime, in milliseconds.
 * @return Status update was successful.
 */
static void _pwmled_state_update(led_attr_t *_attr_p)
{
    if (_attr_p->tic > _attr_p->interval) {
        _attr_p->tic = 0;

        if (_attr_p->pwmdir) {
            _attr_p->data.pwm.on_time++;
            if (_attr_p->data.pwm.on_time >= BREATHE_PWM_PERIOD)
                _attr_p->pwmdir = 0;
        }
        else {
            _attr_p->data.pwm.on_time--;
            if (_attr_p->data.pwm.on_time == 0) {
                _attr_p->pwmdir = 1;
                if (_attr_p->count > 0)
                    _attr_p->count--;
            }
        }
    }
}

/**
 * LED control cycle tasks.
 * @param[in] sysTime system runtime, in milliseconds.
 * @return Status update was successful.
 */
int32_t led_dev_task_handler()
{
    led_attr_t * _attr = NULL;
    uint16_t _id = 0;
    uint8_t state = 0;
    uint8_t _ofs = 0;
    uint8_t ofs = 0;

    if (registered_p == NULL) return - 1;
    if (register_nr == 0) return - 1;

    for (_id = 0; _id < register_nr; _id++) {
        _attr = &registered_p[_id]._attr;

        if (_attr->modCNT) {

            if (_attr->count != 0) {
                if (_attr->modPWM)
                    _attr->_state = _req_pwmled_state(_attr);
                else {
                    _ofs = _attr->offset / 8;
                    ofs = _attr->offset % 8;
                    state = _attr->data.state[_ofs];

                    _attr->_state = (uint8_t)((state >> (ofs)) & 0x01);
                }
            } else {
                state = _attr->statedef;
                _attr->_state = (state) ? LED_ON : LED_OFF;
            }
        } 
        else {
            if (_attr->modPWM)
                _attr->_state = _req_pwmled_state(_attr);
            else {
                _ofs = _attr->offset / 8;
                ofs = _attr->offset % 8;
                state = _attr->data.state[_ofs];

                _attr->_state = (uint8_t)((state >> (ofs)) & 0x01);
            }
        }

        if (_attr->modPWM)
            _pwmled_state_update(_attr);
        else {
            if (_attr->tic >= _attr->interval) {
                _attr->tic = 0;
                _attr->offset++;

                if (_attr->offset >= _attr->valid_bits) {
                    _attr->offset = 0;
                    if (_attr->count > 0) 
                        _attr->count--;
                }
            }
        }
    }
    
    for (_id = 0; _id < register_nr; _id++) {
        if (registered_p[_id].refer != NULL) {
            _attr = &registered_p[_id]._attr;
            registered_p[_id].refer(
                (_attr->_state) ? LED_ON : LED_OFF);
        }
    }

    return 0;
}

/**
 * Each task callback function is timed and used to 
 * execute the corresponding callback function at the end of time.
 */
void led_dev_tick_inc(uint32_t tick_period)
{
    led_attr_t * _attr = NULL;
    uint16_t _id = 0;

    for (; _id < register_nr; _id++) {
        _attr = &registered_p[_id]._attr;
        _attr->tic += tick_period;
    }
}
