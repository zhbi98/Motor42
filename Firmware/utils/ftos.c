/**
 * @file ftos.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "ftos.h"

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Prints a double value with specified number of decimal digits.
 * @param value The double-precision floating-point value to be printed.
 * @param digs Number of digits after the decimal point (positive integer).
 */
void double_printf(double value, int32_t digs)
{
    int32_t int_part = 0;
    int32_t fract_part = 0;
    int32_t i = 1;

    for (; digs != 0; i *= 10, digs--);
    int_part = (int32_t)value;

    fract_part = (int32_t)(
        (value - (double)(int32_t)value) * i
    );

    if (fract_part < 0) fract_part *= -1;
    printf("%d.%d", int_part, fract_part);
}

/**
 * Prints a double value with specified number of decimal digits.
 * @param value The double-precision floating-point value to be printed.
 * @param digs Number of digits after the decimal point (positive integer).
 * @return str.
 */
int8_t * double_string(double value, int32_t digs)
{
    static int8_t str[16] = {0};

    int32_t int_part = 0;
    int32_t fract_part = 0;
    int32_t i = 1;

    for (; digs != 0; i *= 10, digs--);
    int_part = (int32_t)value;

    fract_part = (int32_t)(
        (value - (double)(int32_t)value) * i
    );

    if (fract_part < 0) fract_part *= -1;
    /*printf("%i.%i", int_part, fract_part);*/

    snprintf((int8_t *)str, 16, 
        "%d.%d", int_part, fract_part);

    return str;
}
