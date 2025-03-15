/**
 * @file adc.h
 *
 */

#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "main.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void x42_adc1_init(void);

/* USER CODE BEGIN Prototypes */
extern uint16_t whole_adc_data[2][12];

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */
