/** 
 **************************************************************
 * @file myoslib/util/cli_util.h
 * @author Cameron Stroud - 44344968
 * @date 04042020
 * @brief Utility CLI header file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */

#ifndef S4434496_CLI_UTIL_H
#define S4434496_CLI_UTIL_H

/* Function prototypes -------------------------------------------------------*/
extern void cli_util_init(void);
extern void cli_util_deinit(void);

/* Commands ------------------------------------------------------------------*/
BaseType_t timeCommand(char * pcWriteBuffer, size_t xWriteBufferLen, \
                        const char * pcCommandString);
BaseType_t ledUtilCommand(char * pcWriteBuffer, size_t xWriteBufferLen, \
                        const char * pcCommandString);


#endif // S4434496_CLI_UTIL_H