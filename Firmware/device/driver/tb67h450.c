/**
 * @file tb67h450.c
 *
 */

/**
 * TB67H450 做到 256 细分的原理：
 * (1) TB67H450 支持使用电压参考点来设置 TB67H450 的输出电流，而为了做到细分控制，需要控制 TB67H450 输出电流呈 
 *     sine 函数的变化规律。并且将电流的变化梯度分割为 256 个变化梯度。
 * (2) 所以只要提供给 TB67H450 的参考电压遵循 sine 函数的变化规律，那么 TB67H450 的输出电流
 *     也就呈现 sine 函数的变化规律，并且将参考电压分割为 256 个梯度，即 将 0-3.3V 分割为 256 份。
 * (3) 而控制电压的变化可以采用查 sine 函数表的方式，从表中查出相应值后换算为 PWM 或 DAC 的电压输出。 
 */

/*********************
 *      INCLUDES
 *********************/

#include "tb67h450.h"
#include "sin_map.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void tb_coils_set_current(uint16_t _IA_3300mV_in12bits, uint16_t _IB_3300mV_in12bits);
static void tb_dac_outvolt(uint16_t _VA_3300mV_in12bits, uint16_t _VB_3300mV_in12bits);
static void tb_set_inputA(uint8_t _state_Ap, uint8_t _state_Am);
static void tb_set_inputB(uint8_t _state_Bp, uint8_t _state_Bm);

/**********************
 *  STATIC VARIABLES
 **********************/

static _sindac_t _phaA = {0};
static _sindac_t _phaB = {0};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Sets H-bridge PWM duty cycle for dual-channel motor driver.
 * @param _VA_3300mV_in12bits 12-bit PWM value for 
 * channel A target voltage (0-3300mV range).
 * @param _VB_3300mV_in12bits 12-bit PWM value for 
 * channel B target voltage (0-3300mV range).
 */
static void tb_dac_outvolt(uint16_t _VA_3300mV_in12bits, uint16_t _VB_3300mV_in12bits)
{
    TB_PWMA((_VA_3300mV_in12bits >> 2));
    TB_PWMB((_VB_3300mV_in12bits >> 2));
}

/**
 * Sets coil current using shunt resistor voltage mapping
 * @param _IA_3300mV_in12bits 0-3300mA target current for channel A target voltage.
 * @param _IB_3300mV_in12bits 0-3300mA target current for channel B target voltage.
 */
static void tb_coils_set_current(uint16_t _IA_3300mV_in12bits, 
    uint16_t _IB_3300mV_in12bits)
{
    /*
     * After SetFocCurrentVector calculation a 12bits value 
     * was mapped to 0~3300mA, And due to used 0.1Ohm 
     * shank resistor, 0~3300mV V-ref means 0~3300mA 
     * current set point, For more details, 
     * see TB67H450 Datasheet page.10 .
     */

    tb_dac_outvolt(_IA_3300mV_in12bits, 
        _IB_3300mV_in12bits);
}

/**
 * Sets H-bridge phase state for motor channel A, Requires 
 * TB_AP_H/TB_AP_L macros for physical GPIO control.
 * @param phase_h High-side gate state (0=low, 1=high).
 * @param phase_l Low-side gate state (0=low, 1=high).
 */
static void tb_set_inputA(uint8_t _state_Ap, uint8_t _state_Am)
{
    _state_Ap ? TB_AP_H() : TB_AP_L();
    _state_Am ? TB_AM_H() : TB_AM_L();
}

/**
 * Sets H-bridge phase state for motor channel B, Requires 
 * TB_AP_H/TB_AP_L macros for physical GPIO control.
 * @param phase_h High-side gate state (0=low, 1=high).
 * @param phase_l Low-side gate state (0=low, 1=high).
 */
static void tb_set_inputB(uint8_t _state_Bp, uint8_t _state_Bm)
{
    _state_Bp ? TB_BP_H() : TB_BP_L();
    _state_Bm ? TB_BM_H() : TB_BM_L();
}

/**
 * Sets FOC current vector using sine-wave modulation, the drive current is updated at the same time.
 * @param _dir_inCNT Phase control counter (0-1023 cyclic) for current waveform.
 * @param _I_mA Target current magnitude in milliamps (±3300mA range).
 */
void tb_foc_set_current_vector(uint32_t _dir_inCNT, 
    int32_t _I_mA)
{
    /*The array pointer is obtained from the number of subdivisions*/
    _phaB.mapptr = (_dir_inCNT) & (0x000003FF); /*Balance on 1024 (0x3FF i.e. 1024)*/
    _phaA.mapptr = (_phaB.mapptr + (256)) & (0x000003FF); /*Balance on 1024 (0x3FF i.e. 1024)*/

    /*Obtaining Shaping Data from Data Pointers (Space-for-Time Scheme)*/
    _phaA.mapval = sin_pi_m2[_phaA.mapptr];
    _phaB.mapval = sin_pi_m2[_phaB.mapptr];

    /*sin_pi_m2[] LUT with 1024 12-bit sine values*/

    /*DAC register data (i.e., voltage) obtained from shaping data*/
    /*Voltage-current relationship of 1:1 (sense resistance of 0.1 ohms)*/
    uint32_t bit12val = (uint32_t)(abs(_I_mA) * 1U);

    /*12-bit DAC scaling with 0.1 Ohm shunt mapping*/

    /**
     * Enlarge data by 4096 times (bit12val * 4096 * 4095 / 3300) / 4096) 
     * where (4096 * 4095 / 3300) = 5083, shifted 12 bits to the right, i.e. 
     * divided by 4096 shrinks the data by a factor of 4096.
     */
    bit12val = (uint32_t)(bit12val * 5083) >> 12;
    /*(bit12val * 5083) >> 12 is a variant of (bit12val * 4095 / 3300).*/

    /*Take the remainder of 4096 and round it to the smallest, 
    discarding the sign bits*/
    bit12val = bit12val & (0x00000FFF);

    /*SIN_PI_M2_DPIYBIT defined for amplitude scaling*/

    /*Here is a variant of sin_1024_dpiy*/
    _phaA.bit12val = (uint32_t)(
        bit12val * abs(_phaA.mapval)) >> SIN_PI_M2_DPIYBIT;

    /*Here is a variant of sin_1024_dpiy*/
    _phaB.bit12val = (uint32_t)(
        bit12val * abs(_phaB.mapval)) >> SIN_PI_M2_DPIYBIT;

    /**DAC output, using PWM and filter circuit to 
    achieve the effect of DAC output*/
    tb_coils_set_current(_phaA.bit12val, 
        _phaB.bit12val);

    /*The forward and reverse differentiation of the driver chip 
    does not accept negative values, and commutation is performed 
    according to the IO of the TB67H450*/
    if (_phaA.mapval > 0) tb_set_inputA(true, false);
    else if (_phaA.mapval < 0) tb_set_inputA(false, true);
    else tb_set_inputA(true, true);

    /*The forward and reverse differentiation of the driver chip 
    does not accept negative values, and commutation is performed 
    according to the IO of the TB67H450*/
    if (_phaB.mapval > 0) tb_set_inputB(true, false);
    else if (_phaB.mapval < 0) tb_set_inputB(false, true);
    else tb_set_inputB(true, true);
}

/**
 * Deactivates motor driver outputs and enters standby mode, 
 * Zero current output via DAC registers, Disable H-bridge phase 
 * control signals,controls driver chip disenable states.
 */
void tb_driver_sleep()
{
    /*Clears the TB67H450 drive current, eliminating the torque*/
    _phaA.bit12val = 0;
    _phaB.bit12val = 0;

    /*Clears the TB67H450 drive current, eliminating the torque*/
    tb_coils_set_current(_phaA.bit12val, _phaB.bit12val);

    /*Toggle the two TB67H450 driver chips to standby*/
    tb_set_inputA(false, false);
    tb_set_inputB(false, false);
}

/**
 * Engages motor driver braking by short-circuiting windings, 
 * Zero current output via DAC to disable torque, 
 * Activate H-bridge low-side shunt for dynamic braking,
 * Immediate PWM termination before phase control, 
 * TB67H450 driver supports parallel MOSFET activation.
 */
void tb_driver_brake()
{
    /*Clears the TB67H450 drive current, eliminating the torque*/
    _phaA.bit12val = 0;
    _phaB.bit12val = 0;

    /*Clears the TB67H450 drive current, eliminating the torque*/
    tb_coils_set_current(_phaA.bit12val, _phaB.bit12val);

    /*Toggle the two TB67H450 driver chips to brake*/
    tb_set_inputA(true, true);
    tb_set_inputB(true, true);
}
