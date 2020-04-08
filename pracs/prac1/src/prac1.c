/* Includes ***************************************************/
#include "string.h"

#include "board.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "FreeRTOS_CLI.h"

#include "gpio.h"
#include "leds.h"
#include "log.h"
#include "rtc.h"
#include "watchdog.h"

#include "test_reporting.h"
#include "tiny_printf.h"

#include "device_nvm.h"

#include "unified_comms_serial.h"

#include "cli_task.h"
#include "cli_util.h"
#include "os_util.h"
#include "os_log.h"
#include "lib_log.h"

/* Private Defines ------------------------------------------*/
/* Private Macros -------------------------------------------*/
#define SET_LED(ledcolour)		os_util_queue_led(ledcolour, LEDS_SET)
/* Type Definitions -----------------------------------------*/
/* Function Declarations ------------------------------------*/
void vCustomSerialHandler(xCommsInterface_t *pxComms,
						  xUnifiedCommsIncomingRoute_t *pxCurrentRoute,
						  xUnifiedCommsMessage_t *pxMessage);

/* Private Variables ----------------------------------------*/
eLEDs_t led_colour;

/*-----------------------------------------------------------*/

void vApplicationSetLogLevels(void)
{
	eLogSetLogLevel(LOG_RESULT, LOG_DEBUG);
	eLogSetLogLevel(LOG_APPLICATION, LOG_DEBUG);
}

/*-----------------------------------------------------------*/

void vApplicationStartupCallback( void ) {

    WatchdogReboot_t *  pxRebootData;
    
    /* Initiate startup sequence */
    vLedsSet(LEDS_ALL);

    /* Get Reboot reason */
	pxRebootData = xWatchdogRebootReason();
	if (pxRebootData != NULL)
	{
		vWatchdogPrintRebootReason(LOG_APPLICATION, LOG_INFO, pxRebootData);
	}

	/* Setup our serial receive handler */
	xSerialComms.fnReceiveHandler = vCustomSerialHandler;
	vUnifiedCommsListen(&xSerialComms, COMMS_LISTEN_ON_FOREVER);

    /* Init functions */
    os_log_init();
    os_util_init();
    cli_util_init();
    cli_task_init();

    /* Clear for takeoff */
    led_colour = LEDS_RED;
    // SET_LED(led_colour);
}

/*-----------------------------------------------------------*/

void vApplicationTickCallback( uint32_t ulUptime ) {

    UNUSED(ulUptime);
    /* Flush comms? */
    tdf_uptime_t xUptime = {ulUptime};
	eTdfAddMulti(BLE_LOG, TDF_UPTIME, TDF_TIMESTAMP_NONE, NULL, &xUptime);
	eTdfFlushMulti(BLE_LOG);

	vLedsToggle(LEDS_BLUE);

	switch(led_colour) {
		case LEDS_RED:
			led_colour = LEDS_GREEN;
			break;
		case LEDS_GREEN:
			led_colour = LEDS_BLUE;
			break;
		case LEDS_BLUE:
			led_colour = LEDS_RED;
			break;
		default:
			led_colour = LEDS_RED;
			break;
	}

	SET_LED(led_colour);
}

/*-----------------------------------------------------------*/

void vCustomSerialHandler(xCommsInterface_t *pxComms,
						  xUnifiedCommsIncomingRoute_t *pxCurrentRoute,
						  xUnifiedCommsMessage_t *pxMessage)
{
	char pcLocalString[100] = {0};
    
	UNUSED(pxCurrentRoute);
	UNUSED(pxComms);
	/* 
	 * Copy the string to a local buffer so it can be NULL terminated properly
	 * The %s format specifier does not respect provided lengths
	 */
	pvMemcpy(pcLocalString, pxMessage->pucPayload, pxMessage->usPayloadLen);
    
    cli_task_queue_serial(pcLocalString);
}