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
	
	xTdfTime_t xTime;
	tdf_range_mm_t xRange;
	UNUSED(ulUptime);

	vLedsToggle(LEDS_ALL);

	/* Print the current ultrasonic reading */
	double ultrasonicDist;
	ultrasonicDist = os_ultrasonic_read();
	os_log_print(LOG_INFO, "Ultrasonic Reading: %f", ultrasonicDist);

	/* Send ultrasonic value over bluetooth */
	xRange.range = (uint16_t)(ultrasonicDist);
	os_log_print(LOG_INFO, "Ultrasonic Send: %d", xRange.range);
	eTdfAddMulti(BLE_LOG, TDF_RANGE_MM, TDF_TIMESTAMP_RELATIVE_OFFSET_MS, &xTime, &xRange);

	/** 
	 * Force the BLE_LOG to send all data.
	 * If this is not called, data will be buffered until the TDF Logger fills up
	 * 
	 * Note that the payload sizes received at baselisten will be larger than those specified
	 * above due to the packet headers and data padding on bluetooth.
	 */
	eTdfFlushMulti(BLE_LOG);

}

/*-----------------------------------------------------------*/