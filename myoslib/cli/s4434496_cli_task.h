/**
 *
 * @file    myoslib/cli/s4434496_cli_task.h
 * @author  Cameron Stroud
 * @date    02/04/2020
 * @brief   CLI Task driver
 *************************************************************** 
 *      EXTERNAL FUNCTIONS
 *************************************************************** 
 * s4434496_cli_task_init() - initialise CLI Task
 * s4434496_cli_task_deinit() - deinitialise CLI Task
 ***************************************************************
 */

#include "unified_comms_serial.h"

#ifndef S4434496_CLI_TASK_H
#define S4434496_CLI_TASK_H

void vCustomSerialHandler(xCommsInterface_t *pxComms,
						  xUnifiedCommsIncomingRoute_t *pxCurrentRoute,
						  xUnifiedCommsMessage_t *pxMessage);
extern void s4434496_cli_task_init(void);
extern void s4434496_cli_task_deinit(void);

#endif // S4434496_CLI_TASK_H