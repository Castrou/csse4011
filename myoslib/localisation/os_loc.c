/** 
 **************************************************************
 * @file myoslib/localisation/os_loc.c
 * @author Cameron Stroud - 44344968
 * @date 18052020
 * @brief Localisation OS source file
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

#include "leds.h"
#include "log.h"

#include "os_log.h"

#include "os_loc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define RSSI_PRIORITY (tskIDLE_PRIORITY + 3)
#define RSSI_STACK_SIZE (configMINIMAL_STACK_SIZE * 1)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TaskHandle_t RSSIHandler;
QueueHandle_t QueueNode;

Node RxNode;
Node *NodeArr;

/* Private function prototypes -----------------------------------------------*/
void RSSI_Task( void );

/*----------------------------------------------------------------------------*/

/**
* @brief  Initalises all HCI drivers
* @param  None
* @retval None
*/
extern void os_loc_init( void ) {

	/* Create Semaphore */
	
    /* Create task */
    xTaskCreate((void *) &RSSI_Task, "RSSI Ranging Task",
                    RSSI_STACK_SIZE, NULL, RSSI_PRIORITY, &RSSIHandler);

}

/*----------------------------------------------------------------------------*/

/**
* @brief  Initalises all Log drivers
* @param  None
* @retval None
*/
extern void os_loc_deinit( void ) {

	/* Delete Semaphore */

    /* Delete Task */
    vTaskDelete(RSSIHandler);
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Checks if Node exists in array
* @param  None
* @retval None
*/
uint8_t node_check(Node node) {
    UNUSED(node);
    // Loop through array and check to see if address exists

    return 0xFF; // Return position if it exists, 0xFF otherwise
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Adds Node to the array
* @param  None
* @retval None
*/
void add_node(Node node) {

    UNUSED(node);
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Sends node info to base
* @param  None
* @retval None
*/
void send_node(Node node) {

    UNUSED(node);

}

/*----------------------------------------------------------------------------*/

/**
* @brief  Localisation Task
* @param  None
* @retval None
*/
void RSSI_Task( void ) {

    QueueNode = xQueueCreate(5, sizeof(RxNode));

    for ( ;; ) {

        if (xQueueReceive(QueueNode, &RxNode, 10) == pdTRUE) {   
            // Check if node exists, add it to array otherwise

            // Update RSSI on array

            // Send to base host

        }

        vTaskDelay(5);
    }
}

/*----------------------------------------------------------------------------*/