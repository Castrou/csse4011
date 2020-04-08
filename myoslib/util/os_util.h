/** 
 **************************************************************
 * @file myoslib/util/os_util.h
 * @author Cameron Stroud - 44344968
 * @date 06042020
 * @brief Utility OS header file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */

#ifndef S4434496_OS_UTIL_H
#define S4434496_OS_UTIL_H

#include "semphr.h"

/* Global Defines ------------------------------------------------------------*/
#define LEDS_SET        's'
#define LEDS_ON         'o'
#define LEDS_OFF        'f'
#define LEDS_TOGGLE     't'

/* Global Variables ----------------------------------------------------------*/
SemaphoreHandle_t SemaphoreLED;

/* Function prototypes -------------------------------------------------------*/
extern void os_util_init( void );
extern void os_util_deinit( void );
void os_util_queue_led(eLEDs_t ledColour, char function);

#endif // S4434496_OS_UTIL_H