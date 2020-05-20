/** 
 **************************************************************
 * @file myoslib/localisation/cli_loc.c
 * @author Cameron Stroud - 44344968
 * @date 19052020
 * @brief Localisation Commands source file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */


/* Includes ***************************************************/
#include "string.h"
#include "stdlib.h"

#include "board.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "FreeRTOS_CLI.h"

#include "leds.h"
#include "log.h"
#include "os_log.h"

#include "os_loc.h"
#include "cli_loc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
CLI_Command_Definition_t nodePosCmd = {
	"loc",
	"loc <address> <x/y> <val>: Set x or y coordinates of given Node address.\r\n",
	locCommand,
	-1
};

CLI_Command_Definition_t nodeTypeCmd = {
	"loc",
	"loc <address> <m/s>: Set given Node to mobile or static.\r\n",
	locCommand,
	-1
};

/* Private function prototypes -----------------------------------------------*/

/*----------------------------------------------------------------------------*/

/**
* @brief  Initialise hci commands
* @param  None
* @retval None
*/
extern void cli_hci_init( void ) {

    FreeRTOS_CLIRegisterCommand(&nodePosCmd);
    FreeRTOS_CLIRegisterCommand(&nodeTypeCmd);

}

/*----------------------------------------------------------------------------*/

/**
* @brief  Does nothing
* @param  None
* @retval None
*/
extern void cli_hci_deinit( void ) {


}

/*----------------------------------------------------------------------------*/

/**
* @brief  Command for using SCU I2C peripherals
* @param  pcWriteBuffer: output buffer of command
* @param  xWriteBufferLen: size of output buffer
* @param  pcCommandString: pointer to command string
* @retval pdFalse: indicates command is finished
*/
BaseType_t locCommand(char * pcWriteBuffer, size_t xWriteBufferLen, 
						const char * pcCommandString) 
{
	long lParam_len;
	const char *cCmd_string;

	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    switch(cCmd_string[0]) {
        case 'x':

            break;
        case 'y':

            break;

    }

	UNUSED(pcWriteBuffer);
	UNUSED(xWriteBufferLen);    

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/*----------------------------------------------------------------------------*/