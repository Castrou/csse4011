/** 
 **************************************************************
 * @file myoslib/cli/cli_task.h
 * @author Cameron Stroud - 44344968
 * @date 04042020
 * @brief CLI Task header file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */

#ifndef S4434496_CLI_TASK_H
#define S4434496_CLI_TASK_H

#include "FreeRTOS.h"

/* Function prototypes -------------------------------------------------------*/
extern void cli_task_init(void);
extern void cli_task_queue_serial(const char *input);

#endif // S4434496_CLI_TASK_H