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

#ifndef S4434496_LOG_H
#define S4434496_LOG_H

/* Function prototypes -------------------------------------------------------*/
extern void os_log_queue_print( LogLevel_t type, const char *payload );
extern void os_log_init( void );


#endif // S4434496_LOG_H