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

#include "unified_comms_bluetooth.h"

#include "os_log.h"
#include "os_bt.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct BTMessage {
    const char *message;
} BTMessage;

/* Private define ------------------------------------------------------------*/
#define BT_PRIORITY (tskIDLE_PRIORITY + 2)
#define BT_STACK_SIZE (configMINIMAL_STACK_SIZE * 5)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
QueueHandle_t QueueBluetooth;
TaskHandle_t BTHandler;
BTMessage BTmsg;

/* Private function prototypes -----------------------------------------------*/
void BT_Task( void );

/*----------------------------------------------------------------------------*/

/**
* @brief  Queues Log to be printed
* @param  input: string of Serial input
* @retval None
*/
extern void os_bt_print() {


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


    /* Create Semaphores */


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

    BTMessage IncomingBT;

    /* Create Queue for retrieving from Serial ISR */
    QueueBluetooth = xQueueCreate(10, sizeof(IncomingBT));

    for ( ;; ) {
        // vLedsToggle(LEDS_GREEN);
        if (xQueueReceive(QueueBluetooth, &IncomingBT, 10) == pdTRUE) {   
            
        }
        vTaskDelay(5);
    }
}

/*----------------------------------------------------------------------------*/