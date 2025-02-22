/**
 * @file main.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include <stdio.h>
#include <string.h>
#include "main.h"

#include "motor_control.h"
#include "retarget.h"
#include "time.h"
#include "log.h"
#include "ftos.h"

#include "mt6816.h"
#include "tb67h450.h"
#include "enc_cali.h"

#include "usart.h"
#include "can.h"
#include "spi.h"
#include "tim.h"

#include "led.h"
#include "key.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

int32_t main()
{
    SystemClock_Config();
    HAL_Init();
    RetargetInit(&huart2);

    x42_can_init();
    x42_spi_init();
    x42_usart1_init();
    x42_usart2_init();

    x42_TIM4_init();
    x42_TIM2_init();

    led_msp_init();
    key_msp_init();

    Motor_Control_Init();

    cali._start = (
        key_get_state() == 0
    ) ? true : false;

    motor_control.mode_run = \
        Motor_Mode_Digital_Location;
    Motor_Control_Write_Goal_Location(51200 * 10);
    /*10 Cycle position*/

    /*motor_control.mode_run = \
        Motor_Mode_Digital_Speed;*/
    /*Motor_Control_Write_Goal_Speed(51200 * 0.1);*/
    /*0.1 Cycle/s*/

    for (;;) {
        /* Insert delay 100 ms */
        HAL_Delay(100);
        _enc_cali_solve();

        printf("angle:%d", _angle.rectified);
        led_blink_controller_handler(&red_led);
    }
    return 0;
}

void _TIM2_callback_20khz()
{
    __HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);

    _mag_tick_work();

    if (cali._start) _enc_cali_tick_work();
    else Motor_Control_Callback();

    /*if (encoderCalibrator.isTriggered)*/
    /*    encoderCalibrator.Tick20kHz();*/
    /*else motor.Tick20kHz();*/

}

void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef clkinitstruct = {0};
    RCC_OscInitTypeDef oscinitstruct = {0};
    
    /* Configure PLL ------------------------------------------------------*/
    /* PLL configuration: PLLCLK = (HSI / 2) * PLLMUL = (8 / 2) * 16 = 64 MHz */
    /* PREDIV1 configuration: PREDIV1CLK = PLLCLK / HSEPredivValue = 64 / 1 = 64 MHz */
    /* Enable HSI and activate PLL with HSi_DIV2 as source */
    oscinitstruct.OscillatorType = RCC_OSCILLATORTYPE_HSE/*RCC_OSCILLATORTYPE_HSI*/;
    oscinitstruct.HSEState = RCC_HSE_ON/*RCC_HSE_OFF*/;
    oscinitstruct.LSEState = RCC_LSE_OFF;
    oscinitstruct.HSIState = RCC_HSI_ON;
    oscinitstruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    oscinitstruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    oscinitstruct.PLL.PLLState = RCC_PLL_ON;
    oscinitstruct.PLL.PLLSource = RCC_PLLSOURCE_HSE/*RCC_PLLSOURCE_HSI_DIV2*/;
    oscinitstruct.PLL.PLLMUL = RCC_PLL_MUL9/*RCC_PLL_MUL16*/;
    if (HAL_RCC_OscConfig(&oscinitstruct) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
         clocks dividers */
    clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK 
                            | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
    clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;
    if (HAL_RCC_ClockConfig(&clkinitstruct, 
        FLASH_LATENCY_2) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }
}
