/** 
 **************************************************************
 * @file myoslib/bt/os_bt.c
 * @author Cameron Stroud - 44344968
 * @date 02052020
 * @brief Bluetooth Comms OS file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */


/* Includes ***************************************************/
#include <string.h>

#include "board.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "leds.h"
#include "log.h"
#include "os_log.h"

#include "tdf.h"
#include "tdf_auto.h"

#include "unified_comms_bluetooth.h"

#include "hci_packet.h"
#include "os_hci.h"

#include "os_bt.h"
#include "cli_bt.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct BTMessage {
    eTdfIds_t tdfType;
    xTdfTime_t xTime;
    eTdfTimestampType_t tdfTimestamp;
	uint8_t data[MAX_DF];
} BTMessage_t;

/* Private define ------------------------------------------------------------*/
#define BT_PRIORITY (tskIDLE_PRIORITY + 2)
#define BT_STACK_SIZE (configMINIMAL_STACK_SIZE * 5)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
QueueHandle_t QueueBluetooth;
TaskHandle_t BTHandler;
BTMessage_t BTmsg;

/* Private function prototypes -----------------------------------------------*/
void BT_Task( void );

/*----------------------------------------------------------------------------*/

/**
* @brief  Queues Bluetooth message to be sent
* @param  input: string of Serial input
* @retval None
*/
extern void os_bt_send_unsigned( eTdfIds_t type, eTdfTimestampType_t timestamp, 
						xTdfTime_t time, uint8_t sendData[MAX_DF] ) {

	BTmsg.tdfType = type;
	BTmsg.xTime = time;
	BTmsg.tdfTimestamp = timestamp;
	
	for (int i = 0; i < MAX_DF; i++) {
		BTmsg.data[i] = sendData[i];
	}

    if(xQueueSendToBack(QueueBluetooth, (void *) &BTmsg, 
        (portTickType) 10) != pdPASS) 
    {
        // portENTER_CRITICAL();
        // // debug_printf("Failed to post the message, after 10 ticks.\n\r");
        // portEXIT_CRITICAL();
    }

}

/*----------------------------------------------------------------------------*/

/**
* @brief  Queues Bluetooth message to be sent
* @param  input: string of Serial input
* @retval None
*/
extern void os_bt_send_signed( eTdfIds_t type, eTdfTimestampType_t timestamp, 
						xTdfTime_t time, int8_t sendData[MAX_DF] ) {

	BTmsg.tdfType = type;
	BTmsg.xTime = time;
	BTmsg.tdfTimestamp = timestamp;
	
	for (int i = 0; i < MAX_DF; i++) {
		BTmsg.data[i] = sendData[i];
	}

    if(xQueueSendToBack(QueueBluetooth, (void *) &BTmsg, 
        (portTickType) 10) != pdPASS) 
    {
        // portENTER_CRITICAL();
        // // debug_printf("Failed to post the message, after 10 ticks.\n\r");
        // portEXIT_CRITICAL();
    }

}

/*----------------------------------------------------------------------------*/

/**
* @brief  Initalises all Log drivers
* @param  None
* @retval None
*/
extern void os_bt_init( void ) {

    /* Driver initialisation functions */
	cli_bt_init();

    /* Create Event Groups */
    EventBT = xEventGroupCreate();

    /* Create task */
    xTaskCreate((void *) &BT_Task, "Bluetooth Task", \
                    BT_STACK_SIZE, NULL, BT_PRIORITY, &BTHandler);
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Initalises all Log drivers
* @param  None
* @retval None
*/
extern void os_bt_deinit( void ) {

    /* Driver deinitialisation functions */
    /* Remove Semaphores */
    /* Remove task */
    vTaskDelete(BTHandler);
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Log printing task
* @param  None
* @retval None
*/
void BT_Task( void ) {

    BTMessage_t IncomingBT;

    /* Create Queue for retrieving from Serial ISR */
    QueueBluetooth = xQueueCreate(10, sizeof(IncomingBT));

    for ( ;; ) {
        // vLedsToggle(LEDS_GREEN);
        if (xQueueReceive(QueueBluetooth, &IncomingBT, 10) == pdTRUE) {   
            eTdfAddMulti(BLE_LOG, IncomingBT.tdfType, IncomingBT.tdfTimestamp, &IncomingBT.xTime,  &IncomingBT.data);
	        eTdfFlushMulti(BLE_LOG);
        }

        vTaskDelay(5);
    }
}

/*----------------------------------------------------------------------------*/