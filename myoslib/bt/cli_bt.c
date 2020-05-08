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
#include "stdint.h"
#include "stdlib.h"

#include "board.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "FreeRTOS_CLI.h"

#include "rtc.h"
#include "tdf.h"
#include "tdf_auto.h"

#include "unified_comms_bluetooth.h"
#include "bluetooth.h"

#include "leds.h"
#include "log.h"
#include "os_log.h"

#include "os_bt.h"
#include "lib_bt.h"
#include "cli_bt.h"

#include "hci_packet.h"
#include "lib_hci.h"
#include "os_hci.h"

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
	btCommand,
	2
};

CLI_Command_Definition_t BTDataCmd = {
	"ble",
	"ble c <TDFID>: Send latest TDF information.\r\n",
	btCommand,
	2
};

/* Private function prototypes -----------------------------------------------*/

/*----------------------------------------------------------------------------*/

/**
* @brief  Initialise Bluetooth commands
* @param  None
* @retval None
*/
extern void cli_bt_init( void ) {

    FreeRTOS_CLIRegisterCommand(&BTScanCmd);
	FreeRTOS_CLIRegisterCommand(&BTDataCmd);

}

/*----------------------------------------------------------------------------*/

/**
* @brief  Does nothing
* @param  None
* @retval None
*/
extern void cli_bt_deinit( void ) {


}

/*----------------------------------------------------------------------------*/

/**
* @brief  Command for Bluetooth processes
* @param  pcWriteBuffer: output buffer of command
* @param  xWriteBufferLen: size of output buffer
* @param  pcCommandString: pointer to command string
* @retval pdFalse: indicates command is finished
*/
BaseType_t btCommand(char * pcWriteBuffer, size_t xWriteBufferLen, const char * pcCommandString) {

	long lParam_len;
	long tdf_len;
	const char *cCmd_string;
	const char *tdf_string;
	int tdf_id;
	char dataSign;
	
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
	tdf_string = FreeRTOS_CLIGetParameter(pcCommandString, 2, &tdf_len);

	/* Process Argument */
	if (cCmd_string[0] == DATA) {
		tdf_id = strtol(tdf_string, NULL, 10);

		/* Do the data things */
		switch(tdf_id) {
			case TDF_UPTIME:
				lib_bt_tdf_uptime();
				dataSign = 'u';
				break;
				
			case TDF_LSM6DSL:
				lib_bt_tdf_lsm6dsl();
				dataSign = 'i';
				break;

			case TDF_3D_POSE:
				lib_bt_tdf_3dpose();
				dataSign = 'i';
				break;
			
			case TDF_HEIGHT_MSL:
				lib_bt_tdf_height_msl();
				dataSign = 'u';
				break;

			default:
				break;
		}

		send_bt(tdf_id, dataSign);
		
	} else if (cCmd_string[0] == SCAN) {
		switch(tdf_string[0]) {
			case ON:
				xEventGroupSetBits(EventBT, SCAN_BIT);
				break;
			case OFF:
				xEventGroupClearBits(EventBT, SCAN_BIT);
				break;
			default:
				break;

		}
	} else {
		os_log_print(LOG_ERROR, "Invalid usage");
	}

	UNUSED(pcWriteBuffer);
	UNUSED(xWriteBufferLen);    

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/*----------------------------------------------------------------------------*/