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
#include "stdlib.h"
#include "math.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "leds.h"
#include "log.h"
#include "memory_operations.h"
#include "bluetooth.h"

#include "os_log.h"

#include "os_loc.h"
#include "cli_loc.h"

#include "pb_common.h"
#include "pb_encode.h"
// #include "os_loc.pb.h"

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
Node NodeArr[MAX_NODES];
const Node initNode = {0, {0}, 0, 0, 0, 0, 0};

uint8_t nodeArrPos;

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

	/* Initialise Drivers */
	cli_loc_init();

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
const char *nodeType(NodeType_t type) {

	switch (type)
	{
	case MOBILE_NODE:
		return "MOBILE";
	
	case STATIC_NODE:
		return "STATIC";

	case US_STATIC_NODE:
		return "US_STATIC";

	default:
		return "NONE";
	}
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Checks if Node exists in array
* @param  None
* @retval None
*/
uint8_t node_check(uint8_t *address) {

	uint32_t mismatch;
	int i;
    // Loop through array and check to see if address exists
	for (i = 0; i < nodeArrPos; i++) {
		mismatch = lMemcmp(NodeArr[i].address, address, NODE_ADDR_SIZE);
		if (!mismatch) return i;
		// os_log_print(LOG_ERROR, "mismatch @ [%d] = %d", i, mismatch);
	}
    return NEW_NODE; // Return position if it exists, 0xFF otherwise
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Updates node on array
* @param  None
* @retval None
*/
void updateNode(uint8_t *address, Node node, int parameter) 
{
	int arrPos = node_check(address);

	if (arrPos != NEW_NODE) {
		switch (parameter) {
			case NODE_RSSI:
				NodeArr[arrPos].prevRssi = node.prevRssi;
				NodeArr[arrPos].mmDist = node.mmDist;
				break;

			case NODE_TYPE:
				NodeArr[arrPos].type = node.type;
				os_loc_printNodes();
				break;

			case NODE_XY:
				NodeArr[arrPos].x_pos = node.x_pos;
				NodeArr[arrPos].y_pos = node.y_pos;
				os_loc_printNodes();
				break;
			
			default:
				NodeArr[arrPos] = node;
				break;
		}
	}
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Adds Node to the array
* @param  None
* @retval None
*/
extern void os_loc_addNode(uint8_t *address) {

	if(node_check(address) == NEW_NODE) {
		NodeArr[nodeArrPos] = initNode;
		pvMemcpy(NodeArr[nodeArrPos].address, address, NODE_ADDR_SIZE);
		NodeArr[nodeArrPos].mmDist = 707;
		nodeArrPos++;
		os_loc_printNodes();
	} else {
		os_log_print(LOG_ERROR, "Node exists");
	}
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Adds received node info to the queue
* @param  None
* @retval None
*/
void os_loc_queueNode(Node node) {

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
* @brief  Displays Node Array
* @param  None
* @retval None
*/
extern void os_loc_printNodes( void ) {

	os_log_print(LOG_INFO, "---------------------------------------------------------------------------------");
	os_log_print(LOG_INFO, "| NODE NUMBER\t| TYPE\t\t| ADDRESS\t\t| X_POS\t| Y_POS\t| RSSI\t|");
	os_log_print(LOG_INFO, "---------------------------------------------------------------------------------");

	for (int i = 0; i < nodeArrPos; i++) {
		/* Display Nodes */
		os_log_print(LOG_INFO, "| NODE %d\t| %s\t| %02x:%02x:%02x:%02x:%02x:%02x\t| %d\t| %d\t| %d\t|", 
						i, nodeType(NodeArr[i].type),
						NodeArr[i].address[0], NodeArr[i].address[1], NodeArr[i].address[2], 
						NodeArr[i].address[3], NodeArr[i].address[4], NodeArr[i].address[5],
						NodeArr[i].x_pos, NodeArr[i].y_pos, NodeArr[i].prevRssi);
	}

	os_log_print(LOG_INFO, "---------------------------------------------------------------------------------");

}

/*----------------------------------------------------------------------------*/

/**
* @brief  Sends node info to base
* @param  None
* @retval None
*/
void base_sendNodes( void ) {

	uint8_t buffer[NODE_ADDR_SIZE];
	UNUSED(buffer);
	xBluetoothAddress_t pxLocalAddress;
	vBluetoothGetLocalAddress(&pxLocalAddress);

	/* Send Packet */
	os_log_enterCRITICAL();
	os_log_print(LOG_VERBOSE, "%02x", 0xAA); // Packet start
	/* Mobile Address */
	for (int i = 0; i < NODE_ADDR_SIZE; i++) {
		buffer[i] = pxLocalAddress.pucAddress[i];
		os_log_print(LOG_VERBOSE, "%02x", buffer[i]);
		
	}
	/* Node Array Size */
	os_log_print(LOG_VERBOSE, "%02x", (int)nodeArrPos);
	/* Node Array Info */
	for (int i = 0; i < nodeArrPos; i++) {
		/* Node Type */
		os_log_print(LOG_VERBOSE, "%02x", (int)NodeArr[i].type);

		/* Node Address */
		for (int j = 0; j < NODE_ADDR_SIZE; j++) {
			os_log_print(LOG_VERBOSE, "%02x", NodeArr[i].address[j]);
		}

		/* Node Recorded Distance */
		os_log_print(LOG_VERBOSE, "%04x", NodeArr[i].mmDist);

		/* Static X/Y */
		if (NodeArr[i].type != MOBILE_NODE) {
			os_log_print(LOG_VERBOSE, "%02x", NodeArr[i].x_pos);
			os_log_print(LOG_VERBOSE, "%02x", NodeArr[i].y_pos);
		}

		/* Ultrasonic vals */
		if (NodeArr[i].type == US_STATIC_NODE) {
			os_log_print(LOG_VERBOSE, "%02x", NodeArr[i].ultrasonic);
		}
	}
	os_log_print(LOG_VERBOSE, "\n"); // Packet end
	os_log_exitCRITICAL();
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
	uint16_t counter;
	UNUSED(dist);
	UNUSED(nodeCheck);

    QueueNode = xQueueCreate(20, sizeof(RxNode));

	/* Initialise */
	nodeArrPos = 0;
	for(int i = 0; i < MAX_NODES; i++) {
		NodeArr[i] = initNode;
	}

    for ( ;; ) {
		
        if (xQueueReceive(QueueNode, &RxNode, 10) == pdTRUE) {   

            /* Check if node exists, add it to array otherwise */
            nodeCheck = node_check(RxNode.address);

			/* Calculate distance based on RSSI */
			dist = pow(10, ((double)(MEAS_POWER - RxNode.prevRssi) / ENV_FACTOR));
			RxNode.mmDist = (uint16_t)(dist*1000);
			RxNode.mmDist = 707;

			if(nodeCheck != NEW_NODE) {
                /* Update RSSI on array */
				os_loc_updateNode_rssi(RxNode.address, RxNode);
            }
        }

		if (counter % 200 == 0) {
			base_sendNodes();
		} if (counter >= 10000){
			counter = 0;
		}
		counter++;

        vTaskDelay(5);
    }
}

/*----------------------------------------------------------------------------*/