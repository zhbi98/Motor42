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
#include "Location_Tracker.h"
#include "Speed_Tracker.h"
#include "Current_Tracker.h"
#include "retarget.h"
#include "time.h"
#include "log.h"
#include "ftos.h"

#include "MultiTimer.h"
#include "enc_cali.h"
#include "mt6816.h"
#include "tb67h450.h"
#include "led_anim.h"
#include "btn_doing.h"
#include "ledmx.h"

#include "usart.h"
#include "gpio.h"
#include "can.h"
#include "spi.h"
#include "tim.h"
#include "adc.h"
#include "dma.h"
#include "setup.h"

#include "_485_protocol.h"
#include "mbtimer.h"
#include "mb.h"
#include "mbrtu.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *  STATIC VARIABLES
 **********************/

static volatile uint64_t _tim_tick = 0;
static MultiTimer _TIM_100Hz = {0};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void _TIM_callback_100Hz(MultiTimer * timer, void * userData);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Each task callback function is timed and used to 
 * execute the corresponding callback function at the end of time.
 */
static void tim_task_tick_inc(uint32_t tick_period)
{
    _tim_tick += tick_period;
}

/**
 * Each task callback function is timed and used to 
 * execute the corresponding callback function at the end of time.
 */
static uint64_t tim_task_get_tick()
{
    return _tim_tick;
}

/**
 * Magnetic encoder calibration, data acquisition program, 
 * open-loop state control motor turns left once, 
 * turn right turn in the process to collect 
 * and store encoder output values.
 */
int32_t main()
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();
    RetargetInit(&huart2);

    _enc_dev_init();

    /* Initialize all configured peripherals */
    x42_gpio_init();
    x42_can_init();
    x42_spi_init();
    x42_usart1_init();
    x42_usart2_init();

    x42_TIM4_init();
    x42_TIM2_init();
    /*x42_TIM1_Init();*/
    /*x42_dma_init();*/
    /*x42_adc1_init();*/

    multiTimerInstall(tim_task_get_tick);
    multiTimerStart(&_TIM_100Hz, 100, _TIM_callback_100Hz, NULL); /**5 ms repeating*/
    led_anim_start();
    btn_doing_start();

    read_file();

    Move_Home_Offset = _setup.home_ofs;
    Move_Rated_Speed = _setup.speed_rated;
    Move_Rated_UpAcc = _setup.speed_up_acc;
    Move_Rated_DownAcc = _setup.speed_down_acc;

    Motor_Control_SetStallSwitch(_setup.stall_protect);
    Speed_Tracker_Set_UpAcc(_setup.speed_up_acc);
    Speed_Tracker_Set_DownAcc(_setup.speed_down_acc);
    Location_Tracker_Set_UpAcc(_setup.speed_up_acc);
    Location_Tracker_Set_DownAcc(_setup.speed_down_acc);
    Motor_Control_Init();

    Current_Rated_Current = _setup.current_rated;
    Move_Rated_UpCurrentRate = _setup.current_up_acc;
    Move_Rated_DownCurrentRate = _setup.current_down_acc;

    /*Extract the saved settings and 
    apply them to the motor drive*/
    dce.kp = _setup.dce_kp;
    dce.kv = _setup.dce_kv;
    dce.ki = _setup.dce_ki;
    dce.kd = _setup.dce_kd;

    /*Initialize the RS485 Bus 
    corresponding Modbus controller*/
    mb_rtu_mode_init(42, 115200);
    _mb_rtu_xcall_register(0x03, 
        mb_rtu_read_reg_data);
    _mb_rtu_xcall_register(0x10, 
        mb_rtu_write_reg_data);

    HAL_Delay(100);
    /*Start close loop control tick work*/
    HAL_TIM_Base_Start_IT(&htim2);

    /*The scheduled interrupt can only be started 
    in the final stage of initialization to avoid 
    interrupting the initialization work*/

    for (;;) {
        /* Insert delay 100 ms */
        /*led_dev_task_handler();*/
        _enc_cali_solve();
        /*led_anim_tick_work();*/
        /*btn_doing_tick_work();*/
        file_tick_work();
        /*Process the protocol fields*/
        mb_rtu_pdu_field_deal();
        /*Apply the protocol fields*/
        dev_rs485_apply();
        /*Update Modbus register parameters*/
        dev_rs485_refer();
    }
    return 0;
}

extern _cali_attr_t cali;

/**
 * Magnetic encoder calibration, data acquisition program, 
 * open-loop state control motor turns left once, 
 * turn right turn in the process to collect 
 * and store encoder output values.
 */
void _TIM2_callback_20kHz()
{
    __HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);

    _enc_dev_tick_work();

    if (cali._start) _enc_cali_tick_work();
    else Motor_Control_Callback();
    multiTimerYield();

    led_anim_tick_inc(1);
    btn_doing_tick_inc(1);
    tim_task_tick_inc(1);

    /*if (encoderCalibrator.isTriggered)*/
    /*    encoderCalibrator.Tick20kHz();*/
    /*else motor.Tick20kHz();*/

}

/**
 * Magnetic encoder calibration, data acquisition program, 
 * open-loop state control motor turns left once, 
 * turn right turn in the process to collect 
 * and store encoder output values.
 */
void _TIM1_callback_100Hz()
{
    __HAL_TIM_CLEAR_IT(&htim1, TIM_IT_UPDATE);
}

/**
 * Magnetic encoder calibration, data acquisition program, 
 * open-loop state control motor turns left once, 
 * turn right turn in the process to collect 
 * and store encoder output values.
 */
static void _TIM_callback_100Hz(MultiTimer * timer, void * userData)
{
    led_dev_task_handler();
    led_anim_tick_work();
    btn_doing_tick_work();
    mb_timer_tick_callback();

    multiTimerStart(timer, 100, _TIM_callback_100Hz, NULL);
}

/**
 * Magnetic encoder calibration, data acquisition program, 
 * open-loop state control motor turns left once, 
 * turn right turn in the process to collect 
 * and store encoder output values.
 */
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

/**
 * Magnetic encoder calibration, data acquisition program, 
 * open-loop state control motor turns left once, 
 * turn right turn in the process to collect 
 * and store encoder output values.
 */
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
    if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }
}
