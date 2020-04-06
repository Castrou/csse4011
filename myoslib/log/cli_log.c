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
#include "FreeRTOS_CLI.h"

#include "lib_log.h"
#include "cli_log.h"
#include "os_log.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MAX_LOG_ARG     2
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
CLI_Command_Definition_t Log = {
	"log",
	"log <e/l/d/a> [<e/l/d>]: Changes what log types are displayed.\r\n",
	logCommand,
	-1
};

/* Private function prototypes -----------------------------------------------*/

/**
* @brief  Initialise log commands
* @param  None
* @retval None
*/
extern void cli_log_init( void ) {

    FreeRTOS_CLIRegisterCommand(&Log);

}


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

    /* Check length of arguments */
    for(int i = 0; i < MAX_LOG_ARG; i++) {
	    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, i, &lParam_len);
        if (lParam_len > 1) {
            log_print(LOG_ERROR, "ERROR: Invalid arguments - usage: time [f]\r\n");
        }
    }
	UNUSED(cCmd_string);

	UNUSED(pcWriteBuffer);
	UNUSED(xWriteBufferLen);

    

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}