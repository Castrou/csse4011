/** 
 **************************************************************
 * @file myoslib/log/os_log.h
 * @author Cameron Stroud - 44344968
 * @date 04042020
 * @brief Log OS header file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */

#ifndef S4434496_OS_LOG_H
#define S4434496_OS_LOG_H

#include "semphr.h"

/* Global Variables ----------------------------------------------------------*/
SemaphoreHandle_t SemaphoreErrorLog;
SemaphoreHandle_t SemaphoreInfoLog;
SemaphoreHandle_t SemaphoreDebugLog;
SemaphoreHandle_t SemaphoreOtherLog;

/* Function prototypes -------------------------------------------------------*/
extern void os_log_print( LogLevel_t type, const char *payload, ... );
extern void os_log_init( void );
extern void os_log_deinit( void );
extern void os_log_enterCRITICAL( void );
extern void os_log_exitCRITICAL( void );


#endif // S4434496_OS_LOG_H