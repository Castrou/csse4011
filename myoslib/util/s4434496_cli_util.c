/**
 *
 * @file    myoslib/util/s4434496_cli_util.c
 * @author  Cameron Stroud
 * @date    02/04/2020
 * @brief   Utility functions
 *************************************************************** 
 *      EXTERNAL FUNCTIONS
 *************************************************************** 
 * 
 ***************************************************************
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "leds.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
CLI_Command_Definition_t printTime = {
	"time",
	"time <h/m/s/ms>: Prints elapsed time in given format.\r\n",
	timeCommand,
	1
};

/*-----------------------------------------------------------*/

BaseType_t timeCommand(char * pcWriteBuffer, 
                        size_t xWriteBufferLen, 
                        const char * pcCommandString) 
{
	long lParam_len;
	const char *cCmd_string;
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    vLedsSet( LEDS_BLUE );

    xWriteBufferLen = sprintf((char *) pcWriteBuffer, "\r");
    
	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}