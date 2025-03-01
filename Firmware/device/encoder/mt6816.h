/**
 * @file mt6816.h
 *
 */

#ifndef __MT6816_H__
#define __MT6816_H__

/*********************
 *      INCLUDES
 *********************/

#include <stdbool.h>
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/**
 * Resolution is (2^14 = 16384), each state will use an uint16 data
 * as map, thus total need 32K-flash for calibration.
 */
#define ENC_BIT ((int32_t)(14)) /**Encoder bit width of 14-bit output accuracy*/
#define RESOLUTION ((int32_t)((0x00000001U) << ENC_BIT))

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Describes a data sampling controller,
 * And data back-end filtering.
 */
typedef struct {
    uint16_t data; /**< SPI 读取到的数据*/
    uint16_t angle; /**< SPI 输出的角度*/
    uint8_t no_mag; /**< 磁铁数据有效标志*/
    uint8_t checked; /**< 奇偶校验位*/
} _magval_t;

/**
 * Describes a data sampling controller,
 * And data back-end filtering.
 */
typedef struct {
    uint16_t raw; /**< 原始角度数据*/
    uint16_t rectified; /**< 校准的角度数据*/
    uint8_t rectify_valid; /**< 数据可信标志*/
} _angle_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
/**
 * Initializes the CAN peripheral according to the specified.
 * @param hcan: pointer to a CAN_HandleTypeDef structure that contains.
 * @return HAL status.
 */
void _enc_dev_init();

/**
 * Initializes the CAN peripheral according to the specified.
 * @param hcan: pointer to a CAN_HandleTypeDef structure that contains.
 * @return HAL status.
 */
void _enc_dev_tick_work();

/**
 * Initializes the CAN peripheral according to the specified.
 * @param hcan: pointer to a CAN_HandleTypeDef structure that contains.
 * @return HAL status.
 */
uint8_t _enc_dev_calibrated();

#endif
