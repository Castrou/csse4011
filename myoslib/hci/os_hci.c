/** 
 **************************************************************
 * @file myoslib/hci/os_hci.c
 * @author Cameron Stroud - 44344968
 * @date 20042020
 * @brief HCI OS source file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */


/* Includes ***************************************************/
#include "string.h"
#include "stdio.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "leds.h"
#include "log.h"

#include "unified_comms_serial.h"

#include "os_log.h"

#include "hal_hci.h"
#include "cli_hci.h"
#include "os_hci.h"


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define HCI_PRIORITY (tskIDLE_PRIORITY + 2)
#define HCI_STACK_SIZE (configMINIMAL_STACK_SIZE * 1)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TaskHandle_t HCIHandler;
QueueHandle_t QueueHCI;
char *HCI;

/* Private function prototypes -----------------------------------------------*/
void HCI_Task( void );

/**
* @brief  Initalises all HCI drivers
* @param  None
* @retval None
*/
extern void os_hci_init( void ) {

    /* Initialise HCI */
    hal_hci_init();
    cli_hci_init();

    /* Create task */
    xTaskCreate((void *) &HCI_Task, "HCI Task",
                    HCI_STACK_SIZE, NULL, HCI_PRIORITY, &HCIHandler);

}

/*----------------------------------------------------------------------------*/

/**
* @brief  Initalises all Log drivers
* @param  None
* @retval None
*/
extern void os_hci_deinit( void ) {

    /* Delete Task */
    vTaskDelete(HCIHandler);
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Add to LED Queue
* @param  ledColour: Colour of LED
* @param  function: LED Control function
* @retval None
*/
void os_hci_queue_write(const char *payload) {

    uint32_t payloadLen = strlen(payload);
    pvMemcpy(HCI, payload, payloadLen);

    if(xQueueSendToBack(QueueHCI, HCI, 
        (portTickType) 10) != pdPASS) 
    {
        // portENTER_CRITICAL();
        // // debug_printf("Failed to post the message, after 10 ticks.\n\r");
        // portEXIT_CRITICAL();
    }
}

/*----------------------------------------------------------------------------*/

/**
* @brief  HCI task
* @param  None
* @retval None
*/
void HCI_Task( void ) {
    
    char hciPayload[80];
    vLedsToggle(LEDS_ALL);

    QueueHCI = xQueueCreate(10, sizeof(hciPayload));

    vLedsSet(LEDS_NONE);

    for ( ;; ) {
        // if (xQueueReceive(QueueHCI, &hciPayload, 10) == pdTRUE) {   
        //         // hal_hci_writetest();
        // }
        // hal_hci_writetest();
        vLedsToggle(LEDS_RED);
        vTaskDelay(1000);
    }
}

/*----------------------------------------------------------------------------*/