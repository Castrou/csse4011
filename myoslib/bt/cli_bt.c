/** 
 **************************************************************
 * @file myoslib/bt/cli_bt.c
 * @author Cameron Stroud - 44344968
 * @date 03052020
 * @brief Bluetooth Command Line file
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

#include "tdf.h"
#include "tdfauto.h"
#include "unified_comms_bluetooth.h"
#include "bluetooth.h"

#include "leds.h"
#include "log.h"

#include "os_log.h"
#include "cli_bt.h"
#include "os_bt.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define 	SCAN		's'
#define 	DATA		'c'

#define		ON			'o'
#define		OFF			'f'
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
CLI_Command_Definition_t BTScanCmd = {
	"ble",
	"ble s <o/f>: Change BLE scanning state.\r\n",
	btScanCommand,
	1
};

CLI_Command_Definition_t BTdataCmd = {
	"ble",
	"ble c <TDFID>: Send latest TDF information.\r\n",
	btDataCommand,
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

    FreeRTOS_CLIRegisterCommand(&btScanCmd);
	FreeRTOS_CLIRegisterCommand(&btLogCmd);

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
* @brief  Command for scanning bluetooth devices
* @param  pcWriteBuffer: output buffer of command
* @param  xWriteBufferLen: size of output buffer
* @param  pcCommandString: pointer to command string
* @retval pdFalse: indicates command is finished
*/
BaseType_t btScanCommand(char * pcWriteBuffer, size_t xWriteBufferLen, const char * pcCommandString) {

	long lParam_len;
	long scan_len;
	const char *cCmd_string;
	const char *scan_string;

	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
	scan_string = FreeRTOS_CLIGetParameter(pcCommandString, 2, &scan_len);

	/* Process Argument */
	switch(cCmd_string[0]) {
		case SCAN:
			if(lParam_len > 1) {
				os_log_print(LOG_ERROR, "Invalid usage: ble s <o/f>");
				return pdFALSE;
			}
			break;
		default:
			return pdFalse;
	}

	/* Do the things */
	switch(scan_string[0]) {
		case ON:
			break;
		case OFF:
			break;
		default:
			return pdFALSE;
	}

	UNUSED(pcWriteBuffer);
	UNUSED(xWriteBufferLen);    

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Command for sending latest sensor readings over bluetooth
* @param  pcWriteBuffer: output buffer of command
* @param  xWriteBufferLen: size of output buffer
* @param  pcCommandString: pointer to command string
* @retval pdFalse: indicates command is finished
*/
BaseType_t btDataCommand(char * pcWriteBuffer, size_t xWriteBufferLen, const char * pcCommandString) {

	long lParam_len;
	long tdf_len;
	const char *cCmd_string;
	const char *tdf_string;
	int tdf_id;

	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
	tdf_string = FreeRTOS_CLIGetParameter(pcCommandString, 2, &tdf_len);

	/* Process Argument */
	switch(cCmd_string[0]) {
		case DATA:
			if(lParam_len > 1) {
				os_log_print(LOG_ERROR, "Invalid usage: ble s <o/f>");
				return pdFALSE;
			}
			break;
		default:
			return pdFalse;
	}

	tdf_id = strtol(tdf_string, NULL, 10);
	/* Do the things */
	switch(tdf_id) {
		case TDF_MAG_XYZ_SIGNED:
			break;

		case TDF_LSM6DSL:
			break;

		case TDF_RANGE_MM:
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