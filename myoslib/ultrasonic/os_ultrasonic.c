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
#define     HIGH    1
#define     LOW     0

#define Ultrasonic_PRIORITY (tskIDLE_PRIORITY + 2)
#define Ultrasonic_STACK_SIZE (configMINIMAL_STACK_SIZE * 5)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// QueueHandle_t QueueUs;
TaskHandle_t UltrasonicHandler;

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
* @brief  Ultrasonic Echo Task
* @param  None
* @retval None
*/
void Ultrasonic_Task( void ) {
    float dist;
    int echo = 0;
    uint32_t pulseStart;
    uint32_t pulseEnd;
    uint8_t pulseTime;
    /* Create Queue for US  */
    // QueueUs = xQueueCreate(10, sizeof(echo));

    for ( ;; ) {

        vGpioWrite(TRIG_PIN, HIGH); // start pulse
        while (!echo) {
            echo = bGpioRead(ECHO_PIN);
            pulseStart = ulApplicationUptime(); // time of beginning to recieve pulse
        }
        
        vGpioWrite(TRIG_PIN, LOW); // end pulse
        while (echo) {
            echo = bGpioRead(ECHO_PIN);
            pulseEnd = ulApplicationUptime(); // time of end of pulse
        }

        pulseTime = pulseEnd - pulseStart; // Get pulse width

        /* convert time to dist */
        // dist = (pulseTime / 2) * ?speed?
        UNUSED(dist);
        UNUSED(pulseTime);
        vTaskDelay(5);
    }
}

/*----------------------------------------------------------------------------*/