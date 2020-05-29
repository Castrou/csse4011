/** 
 **************************************************************
 * @file myoslib/ultrasonic/os_ultrasonic.c
 * @author Cameron Stroud - 44344968
 * @date 20052020
 * @brief Ultrasonic OS file
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

#include "hal_ultrasonic.h"
#include "os_ultrasonic.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define Ultrasonic_PRIORITY (tskIDLE_PRIORITY + 2)
#define Ultrasonic_STACK_SIZE (configMINIMAL_STACK_SIZE * 5)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TaskHandle_t UltrasonicHandler;
double dist;

/* Private function prototypes -----------------------------------------------*/
void Ultrasonic_Task( void );

/*----------------------------------------------------------------------------*/

/**
* @brief  Initalises all ultrasonic drivers
* @param  None
* @retval None
*/
extern void os_ultrasonic_init( void ) {

    /* Driver initialisation functions */
	hal_ultrasonic_init();

    /* Create task */
    xTaskCreate((void *) &Ultrasonic_Task, "Ultrasonic Task", \
                    Ultrasonic_STACK_SIZE, NULL, Ultrasonic_PRIORITY, &UltrasonicHandler);
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Deinitialises ultrasonic
* @param  None
* @retval None
*/
extern void os_ultrasonic_deinit( void ) {

    /* Driver deinitialisation functions */
    /* Remove Semaphores */
    /* Remove task */
    vTaskDelete(UltrasonicHandler);
    
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Fetch Ultrasonic reading
* @param  None
* @retval None
*/
extern double os_ultrasonic_read( void ) {

    return dist;
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Ultrasonic Echo Task
* @param  None
* @retval None
*/
void Ultrasonic_Task( void ) {
    
    dist = 0;

    for ( ;; ) {

        dist = hal_ultrasonic_ping();

        vTaskDelay(5);
    }
}

/*----------------------------------------------------------------------------*/