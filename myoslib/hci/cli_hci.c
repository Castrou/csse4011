/** 
 **************************************************************
 * @file myoslib/hci/cli_hci.c
 * @author Cameron Stroud - 44344968
 * @date 21042020
 * @brief HCI Commands source file
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

#include "hci_packet.h"
#include "hal_hci.h"
#include "lib_hci.h"
#include "cli_hci.h"
#include "os_hci.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define     COMMAND			1
#define     SID				2
#define     REGADDR			3
#define     REGVAL			4

#define		READ			'r'

#define		X_HREGADDR		0x29
#define		X_LREGADDR		0x28
#define 	Y_HREGADDR		0x2B
#define 	Y_LREGADDR		0x2A
#define 	Z_HREGADDR		0x2D
#define 	Z_LREGADDR		0x2C

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
CLI_Command_Definition_t i2cCmd = {
	"i2creg",
	"i2creg <r/w> <sid> <regaddr> [<regval>]: Read from or write to a given I2C Address on the SCU.\r\n",
	i2cCommand,
	-1
};

CLI_Command_Definition_t imuCmd = {
	"lsm6dsl",
	"lsm6dsl r <x/y/z/a>: Read a given axis' acceleration data from IMU.\r\n",
	imuCommand,
	2
};

/* Private function prototypes -----------------------------------------------*/

/*----------------------------------------------------------------------------*/

/**
* @brief  Initialise hci commands
* @param  None
* @retval None
*/
extern void cli_hci_init( void ) {

    FreeRTOS_CLIRegisterCommand(&i2cCmd);
	FreeRTOS_CLIRegisterCommand(&imuCmd);

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
BaseType_t i2cCommand(char * pcWriteBuffer, size_t xWriteBufferLen, 
						const char * pcCommandString) 
{
    int argNum = 0; 
	uint8_t sid;
	uint8_t regaddr;
	uint8_t regval;
	long lParam_len;
	const char *cCmd_string;
    char commandString[10] = "";
    char sidString[10] = "";
    char regaddrString[10] = "";
    char regvalString[10] = "";
    char *token;
	Packet commandPacket;
	Datafield commandDatafield;

	commandPacket.dataCnt = 0;
	commandPacket.type = 0x01;

	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

	/* Extract Strings */
    token = strtok((char *)cCmd_string, " ");
    while (token != NULL) {
		argNum++;
        switch(argNum) {
            case COMMAND:
                strcpy(commandString, token);
                break;
            case SID:
                strcpy(sidString, token);
                break;
            case REGADDR:
                strcpy(regaddrString, token);
                break;
            case REGVAL:
                strcpy(regvalString, token);
                break;
            default:
                break;
        }
        token = strtok(NULL, " ");
    }

	/* Argument check */
	if(commandString[0] != 'r' && commandString[0] != 'w') {
		os_log_print(LOG_ERROR, "Invalid usage: requires 'r' or 'w' command");
	}

	/* Convert to number */
	sid = strtol(sidString, NULL, 10);
	regaddr = strtol(regaddrString, NULL, 16);
	regval = strtol(regvalString, NULL, 16);

	/* Build and add datafields to packet */
	commandDatafield = hal_hci_build_datafield(commandString[0], sid, regaddr, regval);
	hal_hci_addDatafield(&commandPacket, commandDatafield);

	/* Write Packet */
	if (xSemaphoreTake(SemaphoreUart, (TickType_t) 10) == pdTRUE) {
		os_hci_write(commandPacket);
	}
	UNUSED(pcWriteBuffer);
	UNUSED(xWriteBufferLen);    

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Command for reading from IMU
* @param  pcWriteBuffer: output buffer of command
* @param  xWriteBufferLen: size of output buffer
* @param  pcCommandString: pointer to command string
* @retval pdFalse: indicates command is finished
*/
BaseType_t imuCommand(char * pcWriteBuffer, size_t xWriteBufferLen, const char * pcCommandString) {
    
	long lParam_len;
	const char *cCmd_string;
	const char *axisString;

	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
	axisString = FreeRTOS_CLIGetParameter(pcCommandString, 2, &lParam_len);

	if(cCmd_string[0] == READ) {
		lib_hci_request_accel(axisString[0]);
	}

	UNUSED(pcWriteBuffer);
	UNUSED(xWriteBufferLen);    

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/*----------------------------------------------------------------------------*/