
/*
* Copyright (c) 2020, Cameron Stroud
* Organisation (Cameron StroudTM)
* All rights reserved.
*/
/* Includes -------------------------------------------------*/

#include "FreeRTOS.h"
#include "task.h"

#include "board.h"

#include "gpio.h"
#include "leds.h"
#include "log.h"
#include "rtc.h"
#include "watchdog.h"

#include "test_reporting.h"
#include "tiny_printf.h"

#include "device_nvm.h"

#include "unified_comms_serial.h"

/* Private Defines ------------------------------------------*/
/* Type Definitions -----------------------------------------*/
/* Function Declarations ------------------------------------*/
/* Private Variables ----------------------------------------*/

/*-----------------------------------------------------------*/

void vCustomSerialHandler(xCommsInterface_t *pxComms,
						  xUnifiedCommsIncomingRoute_t *pxCurrentRoute,
						  xUnifiedCommsMessage_t *pxMessage)
{
	char pcLocalString[60] = {0};
	UNUSED(pxCurrentRoute);
	UNUSED(pxComms);
	/* 
	 * Copy the string to a local buffer so it can be NULL terminated properly
	 * The %s format specifier does not respect provided lengths
	 */
	pvMemcpy(pcLocalString, pxMessage->pucPayload, pxMessage->usPayloadLen);

	eLog(LOG_APPLICATION, LOG_INFO, "\r\nReceived PKT:\r\n");
	eLog(LOG_APPLICATION, LOG_INFO, "\t  Type: %02X\r\n", pxMessage->xPayloadType);
	eLog(LOG_APPLICATION, LOG_INFO, "\tString: %s\r\n\r\n", pcLocalString);

	vLedsSet(LEDS_ALL);

}

/*-----------------------------------------------------------*/

/**
 * @brief intialise CLI Task
 * @param None
 * @retval None
 */
extern void s4434496_cli_task_init( void ) {

    /* Setup our serial receive handler */
	xSerialComms.fnReceiveHandler = vCustomSerialHandler;
	vUnifiedCommsListen(&xSerialComms, COMMS_LISTEN_ON_FOREVER);

}

/*-----------------------------------------------------------*/

/**
 * @brief deintialise CLI Task
 * @param None
 * @retval None
 */
extern void s4434496_cli_task_deinit( void ) {

	while(false);

}

/*-----------------------------------------------------------*/