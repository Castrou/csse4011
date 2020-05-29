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
#define		PRINT		'p'
#define		ADD			'a'
#define		TYPE		't'
#define		XY			'x'
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
CLI_Command_Definition_t nodeAddCmd = {
	"node",
	"node add <addr1>:<addr2>:<addr3>:<addr4>:<addr5>:<addr6>: Add a node to the array with the given address.\r\n",
	nodeCommand,
	-1
};

CLI_Command_Definition_t nodePrintCmd = {
	"node",
	"node print: Print list of nodes.\r\n",
	nodeCommand,
	-1
};

CLI_Command_Definition_t nodeTypeCmd = {
	"node",
	"node type <a:d:d:r:e:ss> <m/s/u>: Set given Node to mobile or static.\r\n",
	nodeCommand,
	-1
};

CLI_Command_Definition_t nodePosCmd = {
	"node",
	"node xy <a:d:d:r:e:ss> <val> <val>: Set x or y coordinates of given Node address.\r\n",
	nodeCommand,
	-1
};

/* Private function prototypes -----------------------------------------------*/

/*----------------------------------------------------------------------------*/

/**
* @brief  Initialise hci commands
* @param  None
* @retval None
*/
extern void cli_loc_init( void ) {

	FreeRTOS_CLIRegisterCommand(&nodeAddCmd);
	FreeRTOS_CLIRegisterCommand(&nodePrintCmd);
	FreeRTOS_CLIRegisterCommand(&nodeTypeCmd);
    FreeRTOS_CLIRegisterCommand(&nodePosCmd);

}

/*----------------------------------------------------------------------------*/

/**
* @brief  Does nothing
* @param  None
* @retval None
*/
extern void cli_loc_deinit( void ) {


}

/*----------------------------------------------------------------------------*/

/**
* @brief  Command for using SCU I2C peripherals
* @param  pcWriteBuffer: output buffer of command
* @param  xWriteBufferLen: size of output buffer
* @param  pcCommandString: pointer to command string
* @retval pdFalse: indicates command is finished
*/
BaseType_t nodeCommand(char * pcWriteBuffer, size_t xWriteBufferLen, 
						const char * pcCommandString) 
{
	long lParam_len;
	const char *cCmd_string;
	char command[60];
	char *addrBuffer;
	uint8_t address[NODE_ADDR_SIZE] = {0};
	char *valBuff;
	Node bufferNode;
	
	/* Fetch Command */
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
	pvMemcpy(command, cCmd_string, strlen(cCmd_string));
	strtok(command, " ");

	switch(cCmd_string[0]) {
		case PRINT:
			/* Just call print nodes function */
			os_loc_printNodes();
			break;

		case ADD:
			/* Build address */
			for (int i = 0; i < NODE_ADDR_SIZE; i++) {
				addrBuffer = strtok(NULL, ":");
				address[i] = strtol(addrBuffer, NULL, 16);
			}

			/* Add to array */
			os_loc_addNode(address);
			break;

		case TYPE:
			/* Build address */
			for (int i = 0; i < NODE_ADDR_SIZE; i++) {
				addrBuffer = strtok(NULL, ":");
				address[i] = strtol(addrBuffer, NULL, 16);
			}
			/* Extract values */
			cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 3, &lParam_len);
			pvMemcpy(command, cCmd_string, strlen(cCmd_string)); // const chars suck
			bufferNode.type = strtol(command, NULL, 10); // string to val
			os_loc_updateNode_type(address, bufferNode);
			break;

		case XY:
			/* Get given node address */
			for (int i = 0; i < NODE_ADDR_SIZE; i++) {
				addrBuffer = strtok(NULL, ":");
				address[i] = strtol(addrBuffer, NULL, 16);
			}
			/* Extract values */
			cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 3, &lParam_len);
			pvMemcpy(command, cCmd_string, strlen(cCmd_string)); // const chars suck
			bufferNode.x_pos = strtol(command, NULL, 10); // string to val
			valBuff = strtok(command, " ");	// y position
			bufferNode.y_pos = strtol(valBuff, NULL, 10); // string to val
			// os_log_print(LOG_DEBUG, "x val: %d", bufferNode.x_pos);
			// os_log_print(LOG_DEBUG, "y val: %d", bufferNode.y_pos);
			/* Update Node */
			os_loc_updateNode_xy(address, bufferNode);
			break;

		default:
			break;
	}

	UNUSED(pcWriteBuffer);
	UNUSED(xWriteBufferLen);    

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/*----------------------------------------------------------------------------*/