/* Includes ***************************************************/
#include "string.h"
#include "stdio.h"

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
#include "tdf.h"
#include "watchdog.h"

#include "test_reporting.h"
#include "tiny_printf.h"

#include "device_nvm.h"

#include "unified_comms_serial.h"
#include "unified_comms_bluetooth.h"

#include "bluetooth.h"

#include "cli_task.h"
#include "os_log.h"
#include "os_hci.h"
#include "os_util.h"
#include "os_bt.h"

/* Private Defines ------------------------------------------*/
/* Private Macros -------------------------------------------*/
/* Type Definitions -----------------------------------------*/
/* Function Declarations ------------------------------------*/
void vCustomSerialHandler(xCommsInterface_t *pxComms,
						  xUnifiedCommsIncomingRoute_t *pxCurrentRoute,
						  xUnifiedCommsMessage_t *pxMessage);
void vCustomBluetoothHandler(xCommsInterface_t *pxComms,
						  		xUnifiedCommsIncomingRoute_t *pxCurrentRoute,
						  		xUnifiedCommsMessage_t *pxMessage);
/* Private Variables ----------------------------------------*/

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

    WatchdogReboot_t *  pxRebootData;
	tdf_watchdog_info_t xWatchdogInfo;

	/* Initiate startup sequence */
	vLedsOn( LEDS_ALL );

	/* Get Reboot reasons and clear */
	pxRebootData = xWatchdogRebootReason();
	if ( pxRebootData != NULL ) {
		vWatchdogPrintRebootReason( LOG_APPLICATION, LOG_INFO, pxRebootData );
		vWatchdogPopulateTdf( pxRebootData, &xWatchdogInfo );

		eTdfAddMulti( SERIAL_LOG, TDF_WATCHDOG_INFO_SMALL, TDF_TIMESTAMP_NONE, NULL, &xWatchdogInfo );
		eTdfFlushMulti( SERIAL_LOG );
	}

	/* Setup our receive handlers */
	xSerialComms.fnReceiveHandler = vCustomSerialHandler;
	xBluetoothComms.fnReceiveHandler = vCustomBluetoothHandler;

	/* Start serial listening */
	vUnifiedCommsListen(&xSerialComms, COMMS_LISTEN_ON_FOREVER);
	/* Start bluetooth scanning */
	vUnifiedCommsListen( &xBluetoothComms, COMMS_LISTEN_ON_FOREVER );

    /* Init functions */
	cli_task_init();
    os_log_init();
    os_hci_init();
	os_util_init();
	os_bt_init();

    /* Clear for takeoff */
    vLedsSet(LEDS_NONE);
}

/*-----------------------------------------------------------*/

void vApplicationTickCallback( uint32_t ulUptime ) {
	
	xTdfTime_t xTime;
	bRtcGetTdfTime(&xTime);	

	tdf_datetime_t xTdfTime;
	xTdfTime.count = ulUptime;

	eTdfAddMulti(BLE_LOG, TDF_DATETIME, TDF_TIMESTAMP_GLOBAL, &xTime, &xTdfTime);
	eTdfFlushMulti(BLE_LOG);

	vLedsToggle(LEDS_BLUE);

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
	
	cli_task_queue(pcLocalString);
}

/*-----------------------------------------------------------*/

void vCustomBluetoothHandler(xCommsInterface_t *pxComms,
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
	
	os_log_print(LOG_DEBUG, "BT Message Recv: %s", pcLocalString);
	cli_task_queue(pcLocalString);
}

/*-----------------------------------------------------------*/
