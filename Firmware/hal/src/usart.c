/**
 * @file usart.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "usart.h"

/*********************
 *      DEFINES
 *********************/

UART_HandleTypeDef huart1 = {0};
UART_HandleTypeDef huart2 = {0};
uint8_t aRxBuffer[32] = {0};
uint8_t RevByte = 0;
uint8_t Revcnt = 0;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
  * @brief  Initializes the UART mode according to the specified parameters in
  *         the UART_InitTypeDef and create the associated handle.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
void x42_usart1_init()
{
    /* USER CODE BEGIN USART1_Init 0 */

    /* USER CODE END USART1_Init 0 */

    /* USER CODE BEGIN USART1_Init 1 */

    /* USER CODE END USART1_Init 1 */

    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;

    if (HAL_UART_DeInit(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }

    /* USER CODE BEGIN USART1_Init 2 */
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    /* USER CODE END USART1_Init 2 */
}

/**
  * @brief  Initializes the UART mode according to the specified parameters in
  *         the UART_InitTypeDef and create the associated handle.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
void x42_usart2_init()
{
    /* USER CODE BEGIN USART1_Init 0 */

    /* USER CODE END USART1_Init 0 */

    /* USER CODE BEGIN USART1_Init 1 */

    /* USER CODE END USART1_Init 1 */

    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;

    if (HAL_UART_DeInit(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }

    /* USER CODE BEGIN USART1_Init 2 */
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
    /* USER CODE END USART1_Init 2 */
}

/**
  * @brief  Initializes the UART mode according to the specified parameters in
  *         the UART_InitTypeDef and create the associated handle.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
void HAL_UART_MspInit(UART_HandleTypeDef * uartHandle)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    if(uartHandle->Instance==USART1)
    {
        /* USART1 clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();

        /*##-1- Enable peripherals and GPIO Clocks*/
        /* Enable GPIO TX/RX clock */
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /*##-2- Configure peripheral GPIO*/  
        /* UART TX GPIO pin configuration*/
        GPIO_InitStruct.Pin = GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* UART RX GPIO pin configuration  */
        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /*##-3- Configure the NVIC for UART*/
        /* NVIC for USART */
        HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
    }
    else if(uartHandle->Instance==USART2)
    {
        /* USART2 clock enable */
        __HAL_RCC_USART2_CLK_ENABLE();

        /*##-1- Enable peripherals and GPIO Clocks*/
        /* Enable GPIO TX/RX clock */
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /*##-2- Configure peripheral GPIO*/  
        /* UART TX GPIO pin configuration  */
        GPIO_InitStruct.Pin = GPIO_PIN_2;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* UART RX GPIO pin configuration  */
        GPIO_InitStruct.Pin = GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /*##-3- Configure the NVIC for UART*/
        /* NVIC for USART */
        HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
    }
}

/**
  * @brief  Initializes the UART mode according to the specified parameters in
  *         the UART_InitTypeDef and create the associated handle.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{
    if(uartHandle->Instance==USART1)
    {
        /* USER CODE BEGIN USART1_MspDeInit 0 */

        /* USER CODE END USART1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART1_CLK_DISABLE();

        /**USART1 GPIO Configuration
        PA9     ------> USART1_TX
        PA10     ------> USART1_RX
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

        /* USART1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USART1_IRQn);
        /* USER CODE BEGIN USART1_MspDeInit 1 */

        /* USER CODE END USART1_MspDeInit 1 */
    }
    else if(uartHandle->Instance==USART2)
    {
        /* USER CODE BEGIN USART1_MspDeInit 0 */

        /* USER CODE END USART1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART2_CLK_DISABLE();

        /**USART1 GPIO Configuration
        PA2     ------> USART2_TX
        PA3     ------> USART2_RX
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

        /* USART1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USART2_IRQn);
        /* USER CODE BEGIN USART1_MspDeInit 1 */

        /* USER CODE END USART1_MspDeInit 1 */
    }
}
