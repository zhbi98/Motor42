/**
 * @file dev_key.h
 *
 */

#ifndef __KEY_H__
#define __KEY_H__

/*********************
 *      INCLUDES
 *********************/

#include <stdbool.h>
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

#define KEY0_EVT ('1')
#define KEY1_EVT ('2')
#define RELEASE  ('0')

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    bool phold;
    uint32_t dura; /**< duration pressed time*/
    uint32_t interval; /**< pressed active speed*/
} _key_event_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#endif /*__KEY_H__*/
