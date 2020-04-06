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

/* Global Variables ----------------------------------------------------------*/
SemaphoreHandle_t SemaphoreLED;

/* Function prototypes -------------------------------------------------------*/
extern void os_util_init( void );
extern void os_util_deinit( void );

#endif // S4434496_OS_UTIL_H