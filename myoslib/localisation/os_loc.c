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
#include "math.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "leds.h"
#include "log.h"

#include "os_log.h"

#include "os_loc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define     NEW_NODE		0xFF
#define		MAX_NODES		10

#define		ENV_FACTOR		20.0
#define		MEAS_POWER		-60

#define RSSI_PRIORITY (tskIDLE_PRIORITY + 3)
#define RSSI_STACK_SIZE (configMINIMAL_STACK_SIZE * 1)

/* Private variables ---------------------------------------------------------*/
TaskHandle_t RSSIHandler;
QueueHandle_t QueueNode;

Node RxNode;
Node *NodeArr;

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void RSSI_Task( void );

/*----------------------------------------------------------------------------*/

/**
* @brief  Initalises all HCI drivers
* @param  None
* @retval None
*/
extern void os_loc_init( void ) {

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

	int matchCnt = 0;
    // Loop through array and check to see if address exists
	for (int i = 0; i < MAX_NODES; i++) {
		for (int j = 0; j < NODE_ADDR_SIZE; j++) {
			if (NodeArr[i].address[j] == node.address[j]) {
				matchCnt++;
			}
		}

		if(matchCnt == NODE_ADDR_SIZE) return i;
		matchCnt = 0;
	}

    return NEW_NODE; // Return position if it exists, 0xFF otherwise
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Updates node on array
* @param  None
* @retval None
*/
void update_node(int arrPos, uint8_t *address, int8_t rssi, 
					int xPos, int yPos) 
{
	for (int i = 0; i < NODE_ADDR_SIZE; i++) {
		NodeArr[arrPos].address[i] = address[i];
	}
	NodeArr[arrPos].prevRssi = rssi;
	NodeArr[arrPos].x_pos = xPos;
	NodeArr[arrPos].y_pos = yPos;

	os_log_print(LOG_DEBUG, "Updated node:\n\r\t rssi: %d, addr: %x:%x:%x:%x:%x:%x", 
					NodeArr[arrPos].prevRssi = rssi,
					NodeArr[arrPos].address[0],
					NodeArr[arrPos].address[1],
					NodeArr[arrPos].address[2],
					NodeArr[arrPos].address[3],
					NodeArr[arrPos].address[4],
					NodeArr[arrPos].address[5]);

}

/*----------------------------------------------------------------------------*/

/**
* @brief  Adds Node to the array
* @param  None
* @retval None
*/
void add_node(Node *node) {

    UNUSED(node);
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Sends node info to base
* @param  None
* @retval None
*/
void os_loc_sendNode(Node node) {

    if(xQueueSendToBack(QueueNode, (void *) &node, 
        (portTickType) 10) != pdPASS) 
    {
        // portENTER_CRITICAL();
        // // debug_printf("Failed to post the message, after 10 ticks.\n\r");
        // portEXIT_CRITICAL();
    }
    UNUSED(node);

}

/*----------------------------------------------------------------------------*/

/**
* @brief  Sends node info to base
* @param  None
* @retval None
*/
void base_sendNode( void ) {

    // just send NodeArr

}

/*----------------------------------------------------------------------------*/

/**
* @brief  Localisation Task
* @param  None
* @retval None
*/
void RSSI_Task( void ) {

	int nodeCheck;
	double dist;
	UNUSED(dist);
	UNUSED(nodeCheck);

    QueueNode = xQueueCreate(20, sizeof(RxNode));

    for ( ;; ) {
		
        if (xQueueReceive(QueueNode, &RxNode, 10) == pdTRUE) {   

            // Check if node exists, add it to array otherwise
            nodeCheck = node_check(RxNode);
			os_log_print(LOG_DEBUG, "node check: %x", nodeCheck);

			for(int i = 0; i < NODE_ADDR_SIZE; i++) {
				// os_log_print(LOG_INFO, "%x", RxNode.address[i]);
			}
			os_log_print(LOG_DEBUG, "rssi: %d", RxNode.prevRssi);
			dist = pow(10, ((double)(MEAS_POWER - RxNode.prevRssi) / ENV_FACTOR));
			os_log_print(LOG_DEBUG, "dist: %fm", dist);

			// Occupancy
			if(dist < 0.5) {
				vLedsSet(LEDS_RED);
			} else {
				vLedsSet(LEDS_NONE);
			}

			// if(nodeCheck == NEW_NODE) {
            //     add_node(&RxNode);
            // }

            // Update RSSI on array
			// update_node_rssi(nodeCheck, RxNode.prevRssi);
			// os_log_print(LOG_DEBUG, "Node:\n\r\t rssi: %d, addr: %x:%x:%x:%x:%x:%x", 
			// 		NodeArr[nodeCheck].prevRssi,
			// 		NodeArr[nodeCheck].address[0],
			// 		NodeArr[nodeCheck].address[1],
			// 		NodeArr[nodeCheck].address[2],
			// 		NodeArr[nodeCheck].address[3],
			// 		NodeArr[nodeCheck].address[4],
			// 		NodeArr[nodeCheck].address[5]);
            // Send to base host

        }

        vTaskDelay(5);
    }
}

/*----------------------------------------------------------------------------*/