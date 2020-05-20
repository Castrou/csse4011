/* Includes ***************************************************/
#include "string.h"
#include "stdio.h"

#include "argon.h"
#include "board.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "event_groups.h"
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
#include "lib_bt.h"
#include "os_bt.h"
#include "os_loc.h"

/* Private Defines ------------------------------------------*/
#define BUTTON_BIT 	(1 << 0)

/* Private Macros -------------------------------------------*/
/* Type Definitions -----------------------------------------*/
/* Function Declarations ------------------------------------*/
void vCustomSerialHandler(xCommsInterface_t *pxComms,
						  xUnifiedCommsIncomingRoute_t *pxCurrentRoute,
						  xUnifiedCommsMessage_t *pxMessage);
void vCustomBluetoothHandler( const uint8_t *pucAddress, eBluetoothAddressType_t eAddressType, 
							int8_t cRssi, bool bConnectable, 
							uint8_t *pucData, uint8_t ucDataLen );
/* Private Variables ----------------------------------------*/
EventGroupHandle_t EventISR;
TickType_t prevTime = 0;
uint8_t count = 0;

/*-----------------------------------------------------------*/

void vButtonInterruptHandler(void)
{
	TickType_t currTime = xTaskGetTickCount();
	EventBits_t isrBits;
	BaseType_t xHigherPriorityTaskWoken, xResult;

	/* xHigherPriorityTaskWoken must be initialised to pdFALSE. */
	xHigherPriorityTaskWoken = pdFALSE;
	isrBits = xEventGroupGetBitsFromISR(EventISR);

	if(currTime - prevTime > 100) {
		vLedsSet(LEDS_ALL);
		isrBits ^= BUTTON_BIT;
		xEventGroupClearBitsFromISR(EventISR, 0xFF);
    	xResult = xEventGroupSetBitsFromISR(
                              EventISR,   /* The event group being updated. */
                              isrBits, /* The bits being set. */
                              &xHigherPriorityTaskWoken );
		prevTime = currTime;		
		/* Was the message posted successfully? */
		
	}

	if( xResult != pdFAIL ) {
		/* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
		switch should be requested.  The macro used is port specific and will
		be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
		the documentation page for the port being used. */
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
}

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
	vUnifiedCommsListen(&xSerialComms, COMMS_LISTEN_ON_FOREVER);

	/* Setup our Bluetooth receive handler */
	vUnifiedCommsBluetoothCustomHandler(vCustomBluetoothHandler);
	vUnifiedCommsListen(&xBluetoothComms, COMMS_LISTEN_ON_FOREVER);

	/* Setup Interrupt Callback */
	vGpioSetup( BUTTON_1, GPIO_INPUTPULL, NRF_GPIO_PIN_PULLDOWN); 
	eGpioConfigureInterrupt(BUTTON_1, true, GPIO_INTERRUPT_RISING_EDGE, vButtonInterruptHandler); 
	EventISR = xEventGroupCreate();

    /* Init functions */
	cli_task_init();
    os_log_init();
    os_hci_init();
	os_util_init();
	os_bt_init();
	os_loc_init();

    /* Clear for takeoff */
    vLedsSet(LEDS_NONE);
}

/*-----------------------------------------------------------*/

void vApplicationTickCallback( uint32_t ulUptime ) {
	
	// xTdfTime_t xTime;
	// bRtcGetTdfTime(&xTime);	

	// tdf_datetime_t xTdfTime;
	// xTdfTime.count = ulUptime;

	// eTdfAddMulti(BLE_LOG, TDF_DATETIME, TDF_TIMESTAMP_GLOBAL, &xTime, &xTdfTime);
	// eTdfFlushMulti(BLE_LOG);
	vLedsToggle(LEDS_BLUE);

	EventBits_t buttonIsr = xEventGroupGetBitsFromISR(EventISR);

	if ((buttonIsr & BUTTON_BIT) == BUTTON_BIT) {
		vLedsToggle(LEDS_RED);
		if (count == 2) {
			lib_bt_tdf_height_msl();
			send_bt(TDF_HEIGHT_MSL, 'u');
		}

		if (count == 4) {
			lib_bt_tdf_3dpose();
			send_bt(TDF_3D_POSE, 'i');
			count = 0;
		}

		count++;
	}

	UNUSED(ulUptime);

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

void vCustomBluetoothHandler( const uint8_t *pucAddress, eBluetoothAddressType_t eAddressType, 
							int8_t cRssi, bool bConnectable, 
							uint8_t *pucData, uint8_t ucDataLen )
{
	EventBits_t btBits = xEventGroupGetBitsFromISR(EventBT);
	Node TxNode;
	UNUSED(eAddressType);
	UNUSED(bConnectable);
	UNUSED(pucData);
	UNUSED(ucDataLen);

	/* Limit printed devices based on RSSI */
	if (cRssi < -60) {
		return;
	}

	if ((btBits & SCAN_BIT) == SCAN_BIT) {
		os_log_print(LOG_INFO, "Bluetooth Recv:\n\r\tAddress: 0x%x\n\r\tRSSI: %3d dBm", 
					pucAddress,
					cRssi);
	}

	// Add to node to be sent to queue
	for(int i = 0; i < NODE_ADDR_SIZE; i++) {
		TxNode.address[i] = pucAddress[i];
	}
	TxNode.prevRssi = cRssi;

	os_loc_sendNode(TxNode);
}

/*-----------------------------------------------------------*/
