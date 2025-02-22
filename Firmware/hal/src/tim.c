/**
 * @file tim.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "tim.h"

/*********************
 *      DEFINES
 *********************/

TIM_HandleTypeDef htim4 = {0};
TIM_HandleTypeDef htim2 = {0};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
  * @brief  Initializes the TIM PWM Time Base according to the specified
  *         parameters in the TIM_HandleTypeDef and initializes the associated handle.
  * @note   Switching from Center Aligned counter mode to Edge counter mode (or reverse)
  *         requires a timer reset to avoid unexpected direction
  *         due to DIR bit readonly in center aligned mode.
  *         Ex: call @ref HAL_TIM_PWM_DeInit() before HAL_TIM_PWM_Init()
  * @param  htim TIM PWM handle
  * @retval HAL status
  */
void x42_TIM4_init()
{
    /*TIM初始化*/
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    /* Configure TIM */
    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 0;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP; /*向上计数*/
    htim4.Init.Period = (1024 - 1); /*10 位周期(70.312KHz)*/
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; /*不分频*/
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; /*禁用自动重新装载*/
    if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET; /*主机模式:复位*/
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE; /*禁用主机模式*/
    if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1; /*PWM 模式 1*/
    sConfigOC.Pulse = 0; /*预设 PWM 输出为 0*/
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH; /*有效电平:高*/
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE; /*禁用快速模式*/
    if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
}

/**
  * @brief  Initializes the TIM Time base Unit according to the specified
  *         parameters in the TIM_HandleTypeDef and initialize the associated handle.
  * @note   Switching from Center Aligned counter mode to Edge counter mode (or reverse)
  *         requires a timer reset to avoid unexpected direction
  *         due to DIR bit readonly in center aligned mode.
  *         Ex: call @ref HAL_TIM_Base_DeInit() before HAL_TIM_Base_Init()
  * @param  htim TIM Base handle
  * @retval HAL status
  */
void x42_TIM2_init()
{
    /* USER CODE BEGIN TIM2_Init 0 */

    /* USER CODE END TIM2_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    /* USER CODE BEGIN TIM2_Init 1 */

    /* USER CODE END TIM2_Init 1 */
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = (72 - 1);
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = (50 - 1);
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief  Initializes the TIM Base MSP.
  * @param  htim TIM PWM handle
  * @retval None
  */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if(tim_baseHandle->Instance == TIM2)
    {
        /* USER CODE BEGIN TIM2_MspInit 0 */

        /* USER CODE END TIM2_MspInit 0 */
        /* TIM4 clock enable */
        __HAL_RCC_TIM2_CLK_ENABLE();

        /* TIM4 interrupt Init */
        HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM2_IRQn);
        /* USER CODE BEGIN TIM2_MspInit 1 */

        /* USER CODE END TIM2_MspInit 1 */
    }
}

/**
  * @brief  Initializes the TIM PWM MSP.
  * @param  htim TIM PWM handle
  * @retval None
  */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* tim_pwmHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if(tim_pwmHandle->Instance == TIM2)
    {
        /* USER CODE BEGIN TIM2_MspInit 0 */

        /* USER CODE END TIM2_MspInit 0 */
        /* TIM2 clock enable */
        __HAL_RCC_TIM2_CLK_ENABLE();
        /* USER CODE BEGIN TIM2_MspInit 1 */

        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**TIM4 GPIO Configuration

        /*Configure GPIO pins TIM4_CH1*/
        GPIO_InitStruct.Pin = GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        /*Configure GPIO pins TIM4_CH2*/
        GPIO_InitStruct.Pin = GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* USER CODE END TIM2_MspInit 1 */
    }
}

/**
  * @brief  DeInitializes TIM Base MSP.
  * @param  htim TIM Base handle
  * @retval None
  */
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef * tim_baseHandle)
{
    if(tim_baseHandle->Instance==TIM2)
    {
        /* USER CODE BEGIN TIM2_MspDeInit 0 */

        /* USER CODE END TIM2_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_TIM2_CLK_DISABLE();

        /* TIM4 interrupt Deinit */
        HAL_NVIC_DisableIRQ(TIM2_IRQn);
        /* USER CODE BEGIN TIM2_MspDeInit 1 */

        /* USER CODE END TIM2_MspDeInit 1 */
    }
}

/**
  * @brief  DeInitializes PWM MSP.
  * @param  htim TIM Base handle
  * @retval None
  */
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef * tim_pwmHandle)
{
    if(tim_pwmHandle->Instance==TIM4)
    {
        /* USER CODE BEGIN TIM2_MspDeInit 0 */

        /* USER CODE END TIM2_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_TIM4_CLK_DISABLE();
        /* USER CODE BEGIN TIM2_MspDeInit 1 */

        /* USER CODE END TIM2_MspDeInit 1 */
    }
}
