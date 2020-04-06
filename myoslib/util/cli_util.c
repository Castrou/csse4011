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

#define LED_ON			'o'
#define LED_OFF			'f'
#define LED_TOGGLE		't'
#define LED_CYCLE		'c'
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
		log_print(LOG_INFO, "Time (hh:mm:ss): %02d:%02d:%02d\r\n", hrs, min, sec);
	} else if (!strcmp(cCmd_string, NOFORMAT)) {
		log_print(LOG_INFO, "Time (seconds): %d\r\n", time);
	} else {
		log_print(LOG_ERROR, "ERROR: Invalid arguments - usage: time [f]\r\n");
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
	if (cCmdParam_len > 1 || colourParam_len > 1) 
	{
		/* Invalid Arguments */
		log_print(LOG_ERROR, "ERROR: Invalid Arguments - usage: led <o/f/t> <r/g/b/c>\r\n");
		return pdFALSE;
	}
	
	/* Check valid arguments */
	if (set_led_colour(&ledVar, &ledColour, arg_colour)) {
		/* Not a colour - is it cycle command? */
		if (arg_colour[0] == LED_CYCLE) {
			switch(cCmd_string[0]) {
				case LED_ON:
					xSemaphoreGive(SemaphoreLED);
					return pdFALSE;
				case LED_OFF:
					xSemaphoreTake(SemaphoreLED, (TickType_t) 10);
					return pdFALSE;
				case LED_TOGGLE:
					if(xSemaphoreTake(SemaphoreLED, (TickType_t) 10) == pdTRUE) { // If it takes semaphore, leave it that way
					} else {
						xSemaphoreGive(SemaphoreLED); // If not, give it back!
					}
					return pdFALSE;
				default:
					log_print(LOG_ERROR, "ERROR: Invalid Arguments - usage: led <o/f/t> <r/g/b/c>\r\n");
					return pdFALSE;
			}
		} else {
			/* Invalid Arguments */
			log_print(LOG_ERROR, "ERROR: Invalid Arguments - usage: led <o/f/t> <r/g/b/c>\r\n");
			return pdFALSE;
		}
	}

	/* Check control command */
	switch(cCmd_string[0]) {
		case LED_ON:
			log_print(LOG_DEBUG, "Switching on: %s\r\n", ledColour);
			vLedsOn(ledVar);
			break;
		case LED_OFF:
			log_print(LOG_DEBUG, "Switching off: %s\r\n", ledColour);
			vLedsOff(ledVar);
			break;
		case LED_TOGGLE:
			log_print(LOG_DEBUG, "Toggling: %s\r\n", ledColour);
			vLedsToggle(ledVar);
			break;
		default:
			log_print(LOG_ERROR, "ERROR: Invalid Arguments - usage: led <o/f/t> <r/g/b/c>\r\n");
			return pdFALSE;
	}

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;	
}

/*----------------------------------------------------------------------------*/