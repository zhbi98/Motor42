/**
  ******************************************************************************
  * @file    Templates/Src/stm32f1xx.c
  * @author  MCD Application Team
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
#include "usart.h"
#include "can.h"
#include "tim.h"
#include "adc.h"

/** @addtogroup STM32F1xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

/**
  * @brief This function handles TIM1 global interrupt.
  */
void TIM1_UP_IRQHandler()
{
    /* USER CODE BEGIN TIM1_IRQn 0 */
    _TIM1_callback_100Hz(); return;
    /* USER CODE END TIM1_IRQn 0 */
    HAL_TIM_IRQHandler(&htim1);
    /* USER CODE BEGIN TIM1_IRQn 1 */
    
    /* USER CODE END TIM1_IRQn 1 */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler()
{
    /* USER CODE BEGIN TIM2_IRQn 0 */
    _TIM2_callback_20kHz(); return;
    /* USER CODE END TIM2_IRQn 0 */
    HAL_TIM_IRQHandler(&htim2);
    /* USER CODE BEGIN TIM2_IRQn 1 */
    
    /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel1 global interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel4 global interrupt.
  */
void DMA1_Channel4_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel4_IRQn 0 */

  /* USER CODE END DMA1_Channel4_IRQn 0 */
  /*HAL_DMA_IRQHandler(NULL);*/
  /* USER CODE BEGIN DMA1_Channel4_IRQn 1 */

  /* USER CODE END DMA1_Channel4_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel5 global interrupt.
  */
void DMA1_Channel5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel5_IRQn 0 */

  /* USER CODE END DMA1_Channel5_IRQn 0 */
  /*HAL_DMA_IRQHandler(NULL);*/
  /* USER CODE BEGIN DMA1_Channel5_IRQn 1 */

  /* USER CODE END DMA1_Channel5_IRQn 1 */
}

/**
 * @brief This function handles USB high priority or CAN TX interrupts.
 */
void USB_HP_CAN1_TX_IRQHandler(void)
{
    /* USER CODE BEGIN USB_HP_CAN1_TX_IRQn 0 */

    /* USER CODE END USB_HP_CAN1_TX_IRQn 0 */
    HAL_CAN_IRQHandler(&hcan);
    /* USER CODE BEGIN USB_HP_CAN1_TX_IRQn 1 */

    /* USER CODE END USB_HP_CAN1_TX_IRQn 1 */
}

/**
 * @brief This function handles USB low priority or CAN RX0 interrupts.
 */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    /* USER CODE BEGIN USB_LP_CAN1_RX0_IRQn 0 */

    /* USER CODE END USB_LP_CAN1_RX0_IRQn 0 */
    HAL_CAN_IRQHandler(&hcan);
    /* USER CODE BEGIN USB_LP_CAN1_RX0_IRQn 1 */

    /* USER CODE END USB_LP_CAN1_RX0_IRQn 1 */
}

/**
 * @brief This function handles CAN RX1 interrupt.
 */
void CAN1_RX1_IRQHandler(void)
{
    /* USER CODE BEGIN CAN1_RX1_IRQn 0 */
    /* USER CODE END CAN1_RX1_IRQn 0 */
    HAL_CAN_IRQHandler(&hcan);
    /* USER CODE BEGIN CAN1_RX1_IRQn 1 */

    /* USER CODE END CAN1_RX1_IRQn 1 */
}

/**
 * @brief This function handles CAN SCE interrupt.
 */
void CAN1_SCE_IRQHandler(void)
{
    /* USER CODE BEGIN CAN1_SCE_IRQn 0 */

    /* USER CODE END CAN1_SCE_IRQn 0 */
    HAL_CAN_IRQHandler(&hcan);
    /* USER CODE BEGIN CAN1_SCE_IRQn 1 */

    /* USER CODE END CAN1_SCE_IRQn 1 */
}

/**
 * @brief This function handles USART1 interrupt.
 */
void USART1_IRQHandler(void)
{
    /* USER CODE BEGIN USART1_IRQn 0 */
    HAL_UART_IRQHandler(&huart1);
    /* USER CODE END USART1_IRQn 0 */
}

/**
 * @brief This function handles USART2 interrupt.
 */
void USART2_IRQHandler(void)
{
    /* USER CODE BEGIN USART1_IRQn 0 */
    HAL_UART_IRQHandler(&huart2);
    /* USER CODE END USART1_IRQn 0 */
}

/******************************************************************************/
/*                 STM32F1xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f1xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/


/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
