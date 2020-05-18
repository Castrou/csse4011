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
#define LOC_PRIORITY (tskIDLE_PRIORITY + 3)
#define LOC_STACK_SIZE (configMINIMAL_STACK_SIZE * 1)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TaskHandle_t LocHandler;
QueueHandle_t QueueNode;

Node RxNode;

/* Private function prototypes -----------------------------------------------*/
void Loc_Task( void );

/*----------------------------------------------------------------------------*/

/**
* @brief  Initalises all HCI drivers
* @param  None
* @retval None
*/
extern void os_loc_init( void ) {

	/* Create Semaphore */
	
    /* Create task */
    xTaskCreate((void *) &Loc_Task, "Localisation Task",
                    LOC_STACK_SIZE, NULL, LOC_PRIORITY, &LocHandler);

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
    vTaskDelete(LocHandler);
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Localisation Task
* @param  None
* @retval None
*/
void Loc_Task( void ) {

    QueueNode = xQueueCreate(5, sizeof(RxNode));

    for ( ;; ) {

        if (xQueueReceive(QueueNode, &RxNode, 10) == pdTRUE) {   

        }

        vTaskDelay(5);
    }
}

/*----------------------------------------------------------------------------*/