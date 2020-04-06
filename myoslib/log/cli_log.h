/** 
 **************************************************************
 * @file myoslib/log/cli_log.h
 * @author Cameron Stroud - 44344968
 * @date 06042020
 * @brief Log CLI header file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */

#ifndef S4434496_CLI_LOG_H
#define S4434496_CLI_LOG_H

/* Function prototypes -------------------------------------------------------*/
extern void cli_log_init( void );
extern void cli_log_deinit( void );

/* Commands ------------------------------------------------------------------*/
BaseType_t logCommand(char * pcWriteBuffer, size_t xWriteBufferLen, \
                        const char * pcCommandString);


#endif // S4434496_CLI_LOG_H