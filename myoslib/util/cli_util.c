/** 
 **************************************************************
 * @file myoslib/util/cli_util.c
 * @author Cameron Stroud - 44344968
 * @date 04042020
 * @brief Utility Commands
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

#include "cli_util.h"
#include "lib_util.h"
#include "os_util.h"
#include "lib_log.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FORMAT			"f"
#define NOFORMAT		"\0"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
CLI_Command_Definition_t Time = {
	"time",
	"time [f]: Prints System Uptime.\r\n",
	timeCommand,
	-1
};

CLI_Command_Definition_t LEDUtil = {
	"led",
	"led <o/f/t> <r/g/b/c>: Controls onboard LEDs.\r\n",
	ledUtilCommand,
	2
};

/* Private function prototypes -----------------------------------------------*/

/*----------------------------------------------------------------------------*/

/**
* @brief  Initalise function for CLI Utility Commands
* @param  None
* @retval None
*/
extern void cli_util_init( void ) {

    FreeRTOS_CLIRegisterCommand(&Time);
	FreeRTOS_CLIRegisterCommand(&LEDUtil);

}

/*----------------------------------------------------------------------------*/

/**
* @brief  Does nothing
* @param  None
* @retval None
*/
extern void cli_util_deinit( void ) {


}

/*----------------------------------------------------------------------------*/

/**
* @brief  Command for printing System Uptime
* @param  pcWriteBuffer: output buffer of command
* @param  xWriteBufferLen: size of output buffer
* @param  pcCommandString: pointer to command string
* @retval pdFalse: indicates command is finished
*/
BaseType_t timeCommand(char * pcWriteBuffer, size_t xWriteBufferLen, const char * pcCommandString) {

	long lParam_len;
	const char *cCmd_string;
	uint32_t time = ulApplicationUptime();	
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

	UNUSED(pcWriteBuffer);
	UNUSED(xWriteBufferLen);

	if (!strcmp(cCmd_string, FORMAT)) {
		int hrs  = time/3600;
        int min = (time - (hrs*3600))/60;
        int sec = (time - (hrs*3600) - (min*60));
		log_print(LOG_INFO, "Time (hh:mm:ss): %02d:%02d:%02d", hrs, min, sec);
	} else if (!strcmp(cCmd_string, NOFORMAT)) {
		log_print(LOG_INFO, "Time (seconds): %d", time);
	} else {
		log_print(LOG_ERROR, "ERROR: Invalid arguments - usage: time [f]");
	}

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Command for controlling onboard LEDs
* @param  pcWriteBuffer: output buffer of command
* @param  xWriteBufferLen: size of output buffer
* @param  pcCommandString: pointer to command string
* @retval pdFalse: indicates command is finished
*/
BaseType_t ledUtilCommand(char * pcWriteBuffer, size_t xWriteBufferLen, const char * pcCommandString) {

	long cCmdParam_len;
	long colourParam_len;
	const char *cCmd_string;
	const char *arg_colour;
	const char *ledColour;
	eLEDs_t ledVar;
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &cCmdParam_len);
	arg_colour = FreeRTOS_CLIGetParameter(pcCommandString, 2, &colourParam_len);

	UNUSED(pcWriteBuffer);
	UNUSED(xWriteBufferLen);

	/* Check parameter length */
	if (cCmdParam_len > 1 || colourParam_len > 1 ||
			set_led_colour(&ledVar, &ledColour, arg_colour)) 
	{
		/* Invalid Arguments */
		log_print(LOG_ERROR, "ERROR: Invalid Arguments - usage: led <o/f/t> <r/g/b/c>");
		return pdFALSE;
	}
	
	/* Check valid arguments */
	switch(cCmd_string[0]) {
		case LEDS_ON:
		case LEDS_OFF:
		case LEDS_TOGGLE:
			break;
		default:
			log_print(LOG_ERROR, "ERROR: Invalid Arguments - usage: led <o/f/t> <r/g/b/c>");
			return pdFALSE;
	}

	os_util_queue_led(ledVar, cCmd_string[0]); // Send to queue

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;	
}

/*----------------------------------------------------------------------------*/