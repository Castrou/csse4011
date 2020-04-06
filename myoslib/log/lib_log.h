/** 
 **************************************************************
 * @file myoslib/log/lib_log.h
 * @author Cameron Stroud - 44344968
 * @date 05042020
 * @brief Log Library header file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */


#ifndef S4434496_LIB_LOG_H
#define S4434496_LIB_LOG_H

#include "log.h"

/* Function prototypes -------------------------------------------------------*/
extern void log_print( LogLevel_t type, const char *string, ... );


#endif // S4434496_LIB_LOG_H