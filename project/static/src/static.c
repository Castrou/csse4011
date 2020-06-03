/* Includes ***************************************************/
#include "FreeRTOS.h"
#include "task.h"

#include "board.h"

#include "gpio.h"
#include "leds.h"
#include "log.h"
#include "rtc.h"
#include "watchdog.h"

#include "tdf.h"

#include "os_log.h"

#include "os_ultrasonic.h"

/* Private Defines ------------------------------------------*/
/* Private Macros -------------------------------------------*/
/* Type Definitions -----------------------------------------*/
/* Function Declarations ------------------------------------*/
/* Private Variables ----------------------------------------*/

/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

void vApplicationSetLogLevels(void)
{
	eLogSetLogLevel(LOG_RESULT, LOG_DEBUG);
	eLogSetLogLevel(LOG_APPLICATION, LOG_DEBUG);
	eLogSetLogLevel( LOG_BLUETOOTH_GAP, LOG_ERROR );
	eLogSetLogLevel( LOG_BLUETOOTH_GATT, LOG_INFO );
}

/*-----------------------------------------------------------*/

void vApplicationStartupCallback( void ) {

    WatchdogReboot_t *pxRebootData;

	vLedsOn(LEDS_ALL);

	/* Get Reboot reason */
	pxRebootData = xWatchdogRebootReason();
	if (pxRebootData != NULL) {
		vWatchdogPrintRebootReason(LOG_APPLICATION, LOG_INFO, pxRebootData);
	}

	os_log_init();
	os_ultrasonic_init();

	vLedsOff(LEDS_ALL);
}

/*-----------------------------------------------------------*/

void vApplicationTickCallback( uint32_t ulUptime ) {
	
	UNUSED(ulUptime);

	vLedsToggle(LEDS_ALL);

	/* Print the current ultrasonic reading */
	double ultrasonicDist;
	ultrasonicDist = os_ultrasonic_read();
	os_log_print(LOG_INFO, "Ultrasonic Reading: %f", ultrasonicDist);

}

/*-----------------------------------------------------------*/