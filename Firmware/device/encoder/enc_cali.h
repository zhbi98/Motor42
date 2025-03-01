/**
 * @file enc_cali.h
 *
 */

#ifndef __ENC_CALI_H__
#define __ENC_CALI_H__

/*********************
 *      INCLUDES
 *********************/

#include "control_config.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

/*********************
 *      DEFINES
 *********************/

#define READ_CNT 16U /*Data collection amount per hardware acquisition point*/

/**********************
 *      TYPEDEFS
 **********************/

enum {
    /**< The Mag calibration program is idle*/
    STATE_IDLE = 0x00,
    /**< Prepare the forward auto-calibration encoder*/
    STATE_FWD_READY,
    /**< Forward acquisition of encoder measurements*/
    STATE_FWD_START,
    /**< Back it up*/
    STATE_BWD_RETURN,
    /**< Reverse elimination of error*/
    STATE_BWD_GAP,
    /**< Backward acquisition of encoder measurements*/
    STATE_BWD_START,
    /**< Solving the data*/
    STATE_SOLVE,
};

/**
 * Describes a data sampling controller,
 * And data back-end filtering.
 */
typedef uint8_t _cali_state_t;

enum {
    /**< There is no error in the data*/
    ERR_NO = 0x00, 
    /**< Wrong direction of average value acquisition*/
    ERR_AVG_DIR, 
    /**< Error in continuity of mean*/
    ERR_AVG_CONTINUTY, 
    /**< Step number error, used to find the motor starting 
    position, because the motor does not necessarily start 
    from the 0 position to collect data*/
    ERR_PHASE_STEP, 
    /**< Error parsing the amount of data*/
    ERR_QUANTITY, 
};

/**
 * Describes a data sampling controller,
 * And data back-end filtering.
 */
typedef uint8_t _cali_err_t;

/**
 * Describes a data sampling controller,
 * And data back-end filtering.
 */
typedef struct {
    /**< Whether to start calibration*/
    uint8_t _start;
    /**< Ongoing calibration steps*/
    uint8_t state;
    /**< Calibration error data*/
    uint32_t errdata; 
    /**< Calibration error codes*/
    uint8_t errid;
    /**< Output the motor rotor 
    position and rotate the 
    motor rotor to the 
    specified position*/
    uint32_t _target;
    /**< Data from the same collection 
    point is collected multiple times*/
    uint16_t rawbuf[READ_CNT];
    /**< Raw data collection count*/
    uint16_t raw_num;
    /**< Forward calibration reads
    data, 201(0-200) data collected*/
    uint16_t forward[Move_Step_NUM + 1];
    /**< Backward calibration reads 
    data, 201(0-200) data collected*/
    uint16_t backward[Move_Step_NUM + 1];
    /**< Multiple acquisition 
    point average counts*/
    uint16_t avg_cnt;
    /**< The running direction of the motor
    and the different directions of the 
    collected data are arranged differently*/
    uint8_t _dir;
    int32_t rcd_x;
    int32_t rcd_y;
    uint32_t result_num;
} _cali_attr_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void _enc_cali_tick_work();
void _enc_cali_solve();

#endif /*__ENC_CALI_H__*/
