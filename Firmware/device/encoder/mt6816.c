/**
 * @file mt6816.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "mt6816.h"
#include "rom_conf.h"
#include "spi.h"

/**********************
 *  STATIC PROTOTYPES
 **********************/

static _magval_t _magval = {0};
_angle_t _angle = {0};

/**********************
 *  STATIC VARIABLES
 **********************/

static uint16_t * caldata_p = NULL;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initializes magnetic calibration data 
 * and validates stored values.
 */
void _mag_init()
{
    caldata_p = (uint16_t *)APP_CALI_ADDR;

    /*Check if the stored calibration data are valid*/
    _angle.rectify_valid = true;
    for (uint32_t i = 0; i < 16384; i++) {
        if (caldata_p[i] == 0xFFFF)
            _angle.rectify_valid = false;
    }
}

/**
 * Executes 16-bit SPI full-duplex transaction with chip-select control.
 * @param data 16-bit data to transmit (MSB/LSB order depends on SPI config).
 * @return 16-bit received data from slave device.
 */
static uint16_t _mag_read_reg(uint16_t data)
{
    uint16_t val = 0x0000;

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 
        GPIO_PIN_RESET);

    HAL_SPI_TransmitReceive(&hspi1, 
        (uint8_t *)&data, (uint8_t *)&val, 1, 
        HAL_MAX_DELAY);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 
        GPIO_PIN_SET);

    return val;
}

/**
 * Reads 16-bit magnetic sensor value via SPI using two 8-bit registers.
 * @return Combined 16-bit magnetic measurement value.
 */
static uint16_t spi_read_magval()
{
    uint16_t tx_buf[2] = {0};
    uint16_t rx_buf[2] = {0};

    tx_buf[0] = (0x80 | 0x03) << 8; /*0x8300*/
    tx_buf[1] = (0x80 | 0x04) << 8; /*0x8400*/

    rx_buf[0] = _mag_read_reg(tx_buf[0]);
    rx_buf[1] = _mag_read_reg(tx_buf[1]);

    uint16_t val = ((rx_buf[0] & 0x00FF) << 8);
    val |= (rx_buf[1] & 0x00FF);

    return val;
}

/**
 * Polls magnetic encoder data with triple-read attempt 
 * and parity validation.updates _angle global structures.
 */
void _mag_tick_work()
{
    uint8_t high_cnt = 0;
    uint16_t checked = 0;
    uint16_t data = 0;

    /*Obtain data from the magnetic 
    encoder 3 times in cycles*/

    for (uint8_t i = 0; i < 3; i++) {
        data = spi_read_magval();
        high_cnt = 0;

        /*Parity check, which determines the 
        number of digits of 1 in the data*/
        for (uint8_t j = 0; j < 16; j++)
            if (data & (0x0001 << j))
                high_cnt++;

        checked = (high_cnt & 0x01) ? 0 : 1;
        if (checked) break;
    }

    _magval.checked = checked;
    if (!checked) return;

    _magval.data = data;
    _magval.angle = data >> 2;

    _magval.no_mag = (bool)(data & (0x0001 << 1));
    _angle.raw = _magval.angle;
    _angle.rectified = caldata_p[_angle.raw];
}

/**
 * Check if the magnetic 
 * encoder has been calibrated.
 * @return Rectify valid.
 */
uint8_t _mag_calibrated()
{
    return _angle.rectify_valid;
}
