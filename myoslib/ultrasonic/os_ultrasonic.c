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
#include "tdf.h"
#include "tdf_struct.h"
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
int connectedSonic = 1;

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
    tdf_range_mm_t xRange;
    xTdfTime_t xTime;

    for ( ;; ) {

        if (connectedSonic) {
            vLedsToggle(LEDS_RED);
            dist = hal_ultrasonic_ping();
            if (!dist) {
                connectedSonic = DISCONNECTED;
            }
        }

        /* Send ultrasonic value over bluetooth */
        xRange.range = (uint16_t)(dist);
        eTdfAddMulti(BLE_LOG, TDF_RANGE_MM, TDF_TIMESTAMP_RELATIVE_OFFSET_MS, &xTime, &xRange);
        /** 
         * Force the BLE_LOG to send all data.
         * If this is not called, data will be buffered until the TDF Logger fills up
         * 
         * Note that the payload sizes received at baselisten will be larger than those specified
         * above due to the packet headers and data padding on bluetooth.
         */
        eTdfFlushMulti(BLE_LOG);

        vTaskDelay(300/portTICK_PERIOD_MS);
    }
}

/*----------------------------------------------------------------------------*/