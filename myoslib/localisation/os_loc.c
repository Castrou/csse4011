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

#include "board.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "leds.h"
#include "log.h"
#include "tdf.h"
#include "tdf_struct.h"
#include "memory_operations.h"
#include "bluetooth.h"

#include "os_log.h"

#include "os_loc.h"
#include "cli_loc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define     NEW_NODE		0xFF
#define		MAX_NODES		10

#define		ENV_FACTOR		30.0
#define		MEAS_POWER		-55

#define		BYTE_SIZE		8

#define RSSI_PRIORITY (tskIDLE_PRIORITY + 3)
#define RSSI_STACK_SIZE (configMINIMAL_STACK_SIZE * 1)

#define BaseSend_PRIORITY (tskIDLE_PRIORITY + 3)
#define BaseSend_STACK_SIZE (configMINIMAL_STACK_SIZE * 1)

#define Occupancy_PRIORITY (tskIDLE_PRIORITY + 3)
#define Occupancy_STACK_SIZE (configMINIMAL_STACK_SIZE * 1)

/* Private variables ---------------------------------------------------------*/
TaskHandle_t RSSIHandler;
TaskHandle_t BaseSendHandler;
TaskHandle_t OccupancyHandler;

QueueHandle_t QueueNode;
SemaphoreHandle_t SemaphoreSerialNode;

Node RxNode;
Node NodeArr[MAX_NODES];
const Node initNode = {0, {0}, 0, 0, 0, 0, 0, {0}};

uint8_t nodeArrPos;

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void RSSI_Task( void );
void BaseSend_Task( void );
void Occupancy_Task( void );

/*----------------------------------------------------------------------------*/

/**
* @brief  Initalises all HCI drivers
* @param  None
* @retval None
*/
extern void os_loc_init( void ) {

	/* Initialise Drivers */
	cli_loc_init();

	SemaphoreSerialNode = xSemaphoreCreateBinary();
	xSemaphoreGive(SemaphoreSerialNode);
	xSemaphoreTake(SemaphoreSerialNode, (TickType_t) 10);

    /* Create task */
    xTaskCreate((void *) &RSSI_Task, "RSSI Ranging Task",
                    RSSI_STACK_SIZE, NULL, RSSI_PRIORITY, &RSSIHandler);
	xTaskCreate((void *) &BaseSend_Task, "Send Node Task",
                    BaseSend_STACK_SIZE, NULL, BaseSend_PRIORITY, &BaseSendHandler);
	xTaskCreate((void *) &Occupancy_Task, "Occupancy Task",
                    Occupancy_STACK_SIZE, NULL, Occupancy_PRIORITY, &OccupancyHandler);

}

/*----------------------------------------------------------------------------*/

/**
* @brief  DeInitalises all Log drivers
* @param  None
* @retval None
*/
extern void os_loc_deinit( void ) {

    /* Delete Task */
    vTaskDelete(RSSIHandler);
	// vTaskDelete(BaseSendHandler);
	vTaskDelete(OccupancyHandler);
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Enable sending to base
* @param  None
* @retval None
*/
extern void os_loc_enableTx( void ) {

    xSemaphoreGive(SemaphoreSerialNode);
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Disable sending to base
* @param  None
* @retval None
*/
extern void os_loc_disableTx( void ) {

    xSemaphoreTake(SemaphoreSerialNode, (TickType_t) 1);
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
			
			case NODE_US:
				NodeArr[arrPos].ultrasonic = node.ultrasonic;
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
extern void os_loc_printLog( void ) {

	os_log_print(LOG_INFO, "-----------------------------------------");
	os_log_print(LOG_INFO, "| NODE ADDRESS\t\t| CONTACT TIME\t|");
	os_log_print(LOG_INFO, "-----------------------------------------");

	for (int i = 0; i < nodeArrPos; i++) {
		/* Display Nodes */
		if (NodeArr[i].type == MOBILE_NODE) {
			os_log_print(LOG_INFO, "| %02x:%02x:%02x:%02x:%02x:%02x\t| %d\t\t|",
							NodeArr[i].address[0], NodeArr[i].address[1], NodeArr[i].address[2], 
							NodeArr[i].address[3], NodeArr[i].address[4], NodeArr[i].address[5],
							NodeArr[i].contact.contactTime);
		}
	}

	os_log_print(LOG_INFO, "-----------------------------------------");

}

/*----------------------------------------------------------------------------*/

/**
* @brief  Displays Node Array
* @param  None
* @retval None
*/
extern void os_loc_printNodes( void ) {

	os_log_print(LOG_INFO, "-------------------------------------------------------------------------------------------");
	os_log_print(LOG_INFO, "| NODE NUMBER\t| TYPE\t\t| ADDRESS\t\t| X_POS\t| Y_POS\t| RSSI\t| MM DIST |");
	os_log_print(LOG_INFO, "-------------------------------------------------------------------------------------------");

	for (int i = 0; i < nodeArrPos; i++) {
		/* Display Nodes */
		os_log_print(LOG_INFO, "| NODE %d\t| %s\t| %02x:%02x:%02x:%02x:%02x:%02x\t| %d\t| %d\t| %d\t| %d\t  |", 
						i, nodeType(NodeArr[i].type),
						NodeArr[i].address[0], NodeArr[i].address[1], NodeArr[i].address[2], 
						NodeArr[i].address[3], NodeArr[i].address[4], NodeArr[i].address[5],
						NodeArr[i].x_pos, NodeArr[i].y_pos, NodeArr[i].prevRssi, NodeArr[i].mmDist);
	}

	os_log_print(LOG_INFO, "-------------------------------------------------------------------------------------------");

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

	uint64_t TxAddr = 0;
	uint64_t nodeId = 0;
	uint64_t nodeInfo[MAX_NODES] = {0};

	/* Send Packet */
	os_log_enterCRITICAL();
	// os_log_print(LOG_VERBOSE, "%02x", 0xAA); // Packet start

	/* Mobile Address */
	for (int i = 0; i < NODE_ADDR_SIZE; i++) {
		// buffer[i] = pxLocalAddress.pucAddress[i];
		// os_log_print(LOG_VERBOSE, "%02x", buffer[i]);
		TxAddr |= (pxLocalAddress.pucAddress[i] << 8*i);
		
	}

	/* Node Array Size */
	// os_log_print(LOG_VERBOSE, "%02x", (int)nodeArrPos);
	nodeId |= ((uint64_t)(nodeArrPos & 0xF) << 56);

	/* Node Array Info */
	for (int i = 0; i < nodeArrPos; i++) {
		int nodeInfoIndex = 0;
		/* Node Type */
		// os_log_print(LOG_VERBOSE, "%02x", (int)NodeArr[i].type);
		nodeInfo[i] |= ((int)NodeArr[i].type << BYTE_SIZE*nodeInfoIndex);
		nodeInfoIndex++;


		/* Node Address */
		// for (int j = 0; j < NODE_ADDR_SIZE; j++) {
		// 	os_log_print(LOG_VERBOSE, "%02x", NodeArr[i].address[j]);
		// }
		nodeId |= (i << 4*i);
		/* Node Recorded Distance */
		// os_log_print(LOG_VERBOSE, "%04x", (uint16_t)(NodeArr[i].mmDist));
		nodeInfo[i] |= (NodeArr[i].mmDist << BYTE_SIZE*nodeInfoIndex);
		nodeInfoIndex += 2;

		/* Static X/Y */
		if (NodeArr[i].type != MOBILE_NODE) {
			// os_log_print(LOG_VERBOSE, "%02x", NodeArr[i].x_pos);
			// os_log_print(LOG_VERBOSE, "%02x", NodeArr[i].y_pos);
			nodeInfo[i] |= (NodeArr[i].x_pos << BYTE_SIZE*nodeInfoIndex);
			nodeInfo[i] |= (NodeArr[i].y_pos << BYTE_SIZE*(nodeInfoIndex+1));
			nodeInfoIndex += 2;
		}

		/* Ultrasonic vals */
		if (NodeArr[i].type == US_STATIC_NODE) {
			// os_log_print(LOG_VERBOSE, "%04x", NodeArr[i].ultrasonic);
			nodeInfo[i] |= (NodeArr[i].ultrasonic << BYTE_SIZE*nodeInfoIndex);

		}
	}
	// os_log_print(LOG_VERBOSE, "\n"); // Packet end
	os_log_exitCRITICAL();
}

/*----------------------------------------------------------------------------*/

/**
* @brief  send update to node
* @param  None
* @retval None
*/
void base_sendUpdate( int nodePos ) {
	/* Variables */
	// tdf_fence_line_t mobAddr = {0};
	// tdf_cvm_info_t nodeInfo = {0};
	tdf_lsm6dsl_t nodeInfo = {0};
	uint16_t xy_pos = 0;
	uint16_t idxtype = 0;

	xBluetoothAddress_t pxLocalAddress;
	vBluetoothGetLocalAddress(&pxLocalAddress);

	/* Mobile Address */
	// for (int i = 0; i < NODE_ADDR_SIZE; i++) {
	// 	mobAddr.fence_line |= (pxLocalAddress.pucAddress[i] << 8*i);
	// }

	/* ID and Type */
	idxtype |= (nodePos << 4);
	idxtype |= (NodeArr[nodePos].type << 8);
	nodeInfo.acc_x = idxtype;

	/* X/Y Position */
	xy_pos |= NodeArr[nodePos].x_pos;
	xy_pos |= (NodeArr[nodePos].y_pos << 8);
	nodeInfo.acc_y = xy_pos;

	/* mm value */
	nodeInfo.gyro_x = NodeArr[nodePos].mmDist;
	
	/* ultrasonic value */
	nodeInfo.gyro_y = NodeArr[nodePos].ultrasonic;

	/* Send */
	// os_log_enterCRITICAL();
	eTdfAddMulti(SERIAL_LOG, TDF_LSM6DSL, TDF_TIMESTAMP_NONE, NULL, &nodeInfo);
	eTdfFlushMulti(SERIAL_LOG);
	// os_log_exitCRITICAL();
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Track Occupancy
* @param  None
* @retval None
*/
void trackContact( int arrPos ) {
	uint32_t time = ulApplicationUptime();
	uint32_t prevTime = NodeArr[arrPos].contact.prevTime;

	switch (NodeArr[arrPos].contact.contactType) {
		case OUTOF_CONTACT:
			if (NodeArr[arrPos].mmDist < NODE_MM_THRESH) {
				NodeArr[arrPos].contact.prevTime = time;
			}
			break;

		case IN_CONTACT:
			NodeArr[arrPos].contact.contactTime += time - prevTime;
			if (NodeArr[arrPos].mmDist < NODE_MM_THRESH) {
				NodeArr[arrPos].contact.prevTime = time;
			}
			break;
	}
}

/*----------------------------------------------------------------------------*/

/**
* @brief  RSSI Ranging Task
* @param  None
* @retval None
*/
void RSSI_Task( void ) {

	int nodeCheck;
	double dist;

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

			/* Is Registered Node? */
			if(nodeCheck != NEW_NODE) {
                /* Update RSSI on array */
				os_loc_updateNode_rssi(RxNode.address, RxNode);
				/* Occupancy */
				trackContact(nodeCheck);
				if (xSemaphoreTake(SemaphoreSerialNode, (TickType_t) 10) == pdTRUE) {
					base_sendUpdate(nodeCheck);
					xSemaphoreGive(SemaphoreSerialNode);
				}
            }
        }
    }
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Send Node Task
* @param  None
* @retval None
*/
void BaseSend_Task( void ) {

    for ( ;; ) {

		if (xSemaphoreTake(SemaphoreSerialNode, (TickType_t) 10) == pdTRUE) {
			// base_sendNodes();
			xSemaphoreGive(SemaphoreSerialNode);
		}

        vTaskDelay(1000);
    }
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Occupancy Task
* @param  None
* @retval None
*/
void Occupancy_Task( void ) {

	int contactCount = 0;
	uint32_t time, prevTime;

	vTaskDelay(1000);
	
    for ( ;; ) {

		contactCount = 0;
		time = ulApplicationUptime();
		for (int i = 0; i < nodeArrPos; i++) {
			prevTime = NodeArr[i].contact.prevTime;
			if (NodeArr[i].type == MOBILE_NODE) {
				if (time - prevTime > NODE_TIMEOUT) {
					/* Connection Lost or >thresh for long enough */
					NodeArr[i].contact.contactType = OUTOF_CONTACT;
				} else {
					/* Within Range */
					NodeArr[i].contact.contactType = IN_CONTACT;
					contactCount++;
				}				
			}
		}

		/* Update LED */
		if (contactCount) {
			vLedsOn(LEDS_RED);
		} else {
			vLedsOff(LEDS_RED);
		}

        vTaskDelay(50);
    }
}

/*----------------------------------------------------------------------------*/