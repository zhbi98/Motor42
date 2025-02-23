/**
 * @file can.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "can.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

CAN_TxHeaderTypeDef TxHeader = {0};
CAN_RxHeaderTypeDef RxHeader = {0};
uint32_t TxMailbox = 0;
uint8_t TxData[8] = {0};
uint8_t RxData[8] = {0};

CAN_HandleTypeDef hcan = {0};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief  Initializes the CAN peripheral according to the specified
 *         parameters in the CAN_InitStruct.
 * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
 *         the configuration information for the specified CAN.
 * @retval HAL status
 */
void x42_can_init()
{
    /* USER CODE BEGIN CAN_Init 0 */

    /* USER CODE END CAN_Init 0 */

    /* USER CODE BEGIN CAN_Init 1 */

    /* USER CODE END CAN_Init 1 */

    hcan.Instance = CAN1;
    hcan.Init.Prescaler = 8; /*36MHz/(5tq+3tq+1tq)/prescaler = 500KHz*/
    hcan.Init.Mode = CAN_MODE_NORMAL;
    hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan.Init.TimeSeg1 = CAN_BS1_5TQ;
    hcan.Init.TimeSeg2 = CAN_BS2_3TQ;
    hcan.Init.TimeTriggeredMode = DISABLE;
    hcan.Init.AutoBusOff = DISABLE;
    hcan.Init.AutoWakeUp = DISABLE;
    hcan.Init.AutoRetransmission = DISABLE;
    hcan.Init.ReceiveFifoLocked = DISABLE;
    hcan.Init.TransmitFifoPriority = DISABLE;
    if (HAL_CAN_Init(&hcan) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN CAN_Init 2 */
    CAN_FilterTypeDef sFilterConfig;
    /*Filter one (stack light blink)*/
    sFilterConfig.FilterBank = 0;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.SlaveStartFilterBank = 14;
    if (HAL_CAN_ConfigFilter(&hcan, 
        &sFilterConfig) != HAL_OK)
    {
        /*Filter configuration Error*/
        Error_Handler();
    }

    HAL_CAN_Start(&hcan); /*Start CAN*/

    HAL_CAN_ActivateNotification(&hcan,
        CAN_IT_TX_MAILBOX_EMPTY |
        CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO1_MSG_PENDING |
        /*We probably only want this*/
        CAN_IT_RX_FIFO0_FULL | CAN_IT_RX_FIFO1_FULL |
        CAN_IT_RX_FIFO0_OVERRUN | CAN_IT_RX_FIFO1_OVERRUN |
        CAN_IT_WAKEUP | CAN_IT_SLEEP_ACK |
        CAN_IT_ERROR_WARNING | CAN_IT_ERROR_PASSIVE |
        CAN_IT_BUSOFF | CAN_IT_LAST_ERROR_CODE |
        CAN_IT_ERROR);
}

/**
 * @brief  Initializes the GPIOx peripheral according to the specified parameters in the GPIO_Init.
 * @param  GPIOx: where x can be (A..G depending on device used) to select the GPIO peripheral
 * @param  GPIO_Init: pointer to a GPIO_InitTypeDef structure that contains
 *         the configuration information for the specified GPIO peripheral.
 * @retval None
 */
void HAL_CAN_MspInit(CAN_HandleTypeDef *canHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if(canHandle->Instance == CAN1) {
        /* USER CODE BEGIN CAN1_MspInit 0 */

        /* USER CODE END CAN1_MspInit 0 */
        /* CAN1 clock enable */
        __HAL_RCC_CAN1_CLK_ENABLE();
        __HAL_RCC_AFIO_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**CAN GPIO Configuration
        PB8 ------> CAN_RX
        PB9 ------> CAN_TX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_8;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        __HAL_AFIO_REMAP_CAN1_2();

        /* CAN1 interrupt Init */
        HAL_NVIC_SetPriority(USB_HP_CAN1_TX_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);

        HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);

        HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);

        HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
        /* USER CODE BEGIN CAN1_MspInit 1 */

        /* USER CODE END CAN1_MspInit 1 */
    }
}

/**
  * @brief  De-initializes the GPIOx peripheral registers to their default reset values.
  * @param  GPIOx: where x can be (A..G depending on device used) to select the GPIO peripheral
  * @param  GPIO_Pin: specifies the port bit to be written.
  *         This parameter can be one of GPIO_PIN_x where x can be (0..15).
  * @retval None
  */
void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{
    if(canHandle->Instance == CAN1) {
        /* USER CODE BEGIN CAN1_MspDeInit 0 */

        /* USER CODE END CAN1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_CAN1_CLK_DISABLE();

        /**CAN GPIO Configuration
        PB8     ------> CAN_RX
        PB9     ------> CAN_TX
        */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8 | GPIO_PIN_9);

        /* CAN1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);
        HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
        HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
        HAL_NVIC_DisableIRQ(CAN1_SCE_IRQn);
        /* USER CODE BEGIN CAN1_MspDeInit 1 */

        /* USER CODE END CAN1_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

/**
  * @brief  Add a message to the first free Tx mailbox and activate the
  *         corresponding transmission request.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @param  pHeader pointer to a CAN_TxHeaderTypeDef structure.
  * @param  aData array containing the payload of the Tx frame.
  * @param  pTxMailbox pointer to a variable where the function will return
  *         the TxMailbox used to store the Tx message.
  *         This parameter can be a value of @arg CAN_Tx_Mailboxes.
  * @retval HAL status
  */
void CAN_Send(CAN_TxHeaderTypeDef* pHeader, uint8_t* data)
{
    if (HAL_CAN_AddTxMessage(&hcan, pHeader, 
        data, &TxMailbox) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief  Get an CAN frame from the Rx FIFO zone into the message RAM.
  * @param  hcan pointer to an CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @param  RxFifo Fifo number of the received message to be read.
  *         This parameter can be a value of @arg CAN_receive_FIFO_number.
  * @param  pHeader pointer to a CAN_RxHeaderTypeDef structure where the header
  *         of the Rx frame will be stored.
  * @param  aData array where the payload of the Rx frame will be stored.
  * @retval HAL status
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{
    /* Get RX message */
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
    {
        /* Reception Error */
        Error_Handler();
    }

    uint8_t id = (RxHeader.StdId >> 7); // 4Bits ID & 7Bits Msg
    uint8_t cmd = RxHeader.StdId & 0x7F; // 4Bits ID & 7Bits Msg
    if (id == 0 || id == 0/*boardConfig.canNodeId*/)
    {
        /*OnCanCmd(cmd, RxData, RxHeader.DLC);*/
    }
}
/* USER CODE END 1 */
