/**
 * @file sin_map.h
 *
 */

#ifndef __SIN_MAP_H__
#define __SIN_MAP_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/*sin_pi_m2 (对应原始 sin 函数图形 0-2PI)*/
#define SIN_PI_M2_DPIX    1024U /*水平分辨率*/
#define SIN_PI_M2_DPIYBIT 12U   /*垂直分辨率位数 (12bit 即 4096)*/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

extern const int16_t sin_pi_m2[SIN_PI_M2_DPIX + 1];

#ifdef __cplusplus
}
#endif

#endif /*__SIN_MAP_H__*/

