/** 
 **************************************************************
 * @file myoslib/log/cli_log.c
 * @author Cameron Stroud - 44344968
 * @date 06042020
 * @brief Log Library header file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */


/* Includes ***************************************************/
#include "string.h"

#include "board.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "FreeRTOS_CLI.h"

#include "leds.h"
#include "log.h"

#include "lib_log.h"
#include "cli_log.h"
#include "os_log.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ARG_ERROR		'e'
#define ARG_INFO		'l'
#define ARG_DEBUG		'd'
#define ARG_ALL			'a'
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
CLI_Command_Definition_t LogCmd = {
	"log",
	"log <e/l/d/a>: Changes what log types are displayed.\r\n",
	logCommand,
	1
};

/* Private function prototypes -----------------------------------------------*/

/*----------------------------------------------------------------------------*/

/**
* @brief  Initialise log commands
* @param  None
* @retval None
*/
extern void cli_log_init( void ) {

    FreeRTOS_CLIRegisterCommand(&LogCmd);

}

/*----------------------------------------------------------------------------*/

/**
* @brief  Does nothing
* @param  None
* @retval None
*/
extern void cli_log_deinit( void ) {


}

/*----------------------------------------------------------------------------*/

/**
* @brief  Command for printing System Uptime
* @param  pcWriteBuffer: output buffer of command
* @param  xWriteBufferLen: size of output buffer
* @param  pcCommandString: pointer to command string
* @retval pdFalse: indicates command is finished
*/
BaseType_t logCommand(char * pcWriteBuffer, size_t xWriteBufferLen, const char * pcCommandString) {

	long lParam_len;
	const char *cCmd_string;

	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
    /* Check length of arguments */
	if (lParam_len > 1) {
		log_print(LOG_ERROR, "ERROR: Invalid arguments - usage: log <e/l/d/a>\r\n");
		return pdFALSE;
	}

	/* Take Semaphores */
	xSemaphoreTake(SemaphoreErrorLog, (TickType_t) 10);
	xSemaphoreTake(SemaphoreInfoLog, (TickType_t) 10);
	xSemaphoreTake(SemaphoreDebugLog, (TickType_t) 10);

	/* Process Argument */
	switch(cCmd_string[0]) {
		case ARG_ERROR:
			xSemaphoreGive(SemaphoreErrorLog);
			break;
		case ARG_INFO:
			xSemaphoreGive(SemaphoreInfoLog);
			break;
		case ARG_DEBUG:
			xSemaphoreGive(SemaphoreDebugLog);
			break;
		case ARG_ALL:
			xSemaphoreGive(SemaphoreErrorLog);
			xSemaphoreGive(SemaphoreInfoLog);
			xSemaphoreGive(SemaphoreDebugLog);
			break;
	}

	UNUSED(pcWriteBuffer);
	UNUSED(xWriteBufferLen);    

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/*----------------------------------------------------------------------------*/