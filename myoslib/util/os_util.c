/** 
 **************************************************************
 * @file myoslib/util/os_util.c
 * @author Cameron Stroud - 44344968
 * @date 06042020
 * @brief Utility OS source file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */


/* Includes ***************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "leds.h"
#include "log.h"

#include "os_log.h"
#include "os_util.h"
#include "cli_util.h"
#include "lib_util.h"


/* Private typedef -----------------------------------------------------------*/
typedef struct LEDControl {
    eLEDs_t colour;
    char *colourName;
    char function;
} LEDControl;

/* Private define ------------------------------------------------------------*/
#define LEDControl_PRIORITY (tskIDLE_PRIORITY + 2)
#define LEDControl_STACK_SIZE (configMINIMAL_STACK_SIZE * 1)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TaskHandle_t LEDControlHandler;
QueueHandle_t QueueLED;
LEDControl TxLED;
/* Private function prototypes -----------------------------------------------*/
void LEDControl_Task( void );

/**
* @brief  Initalises all Log drivers
* @param  None
* @retval None
*/
extern void os_util_init( void ) {

    /* Create Semaphores */
    SemaphoreLED = xSemaphoreCreateBinary();

    xSemaphoreGive(SemaphoreLED);

    /* Init util commands */
    cli_util_init();

    /* Create task */
    xTaskCreate((void *) &LEDControl_Task, "LED Control Task",
                    LEDControl_STACK_SIZE, NULL, LEDControl_PRIORITY, &LEDControlHandler);

}

/*----------------------------------------------------------------------------*/

/**
* @brief  Initalises all Log drivers
* @param  None
* @retval None
*/
extern void os_util_deinit( void ) {

    /* Remove Semaphores */
    vSemaphoreDelete(SemaphoreLED);

    /* Delete Task */
    vTaskDelete(LEDControlHandler);
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Add to LED Queue
* @param  ledColour: Colour of LED
* @param  function: LED Control function
* @retval None
*/
void os_util_queue_led(eLEDs_t ledColour, char function) {

    TxLED.colour = ledColour;
    TxLED.function = function;

    if(xQueueSendToBack(QueueLED, (void *) &TxLED, (portTickType) 1) != pdPASS) {
        // Failed
    }

    vTaskDelay(5);
}

/*----------------------------------------------------------------------------*/

/**
* @brief  LED Control task
* @param  None
* @retval None
*/
void LEDControl_Task( void ) {

    LEDControl RxLED;
    QueueLED = xQueueCreate(5, sizeof(RxLED));

    for ( ;; ) {
        if (xQueueReceive(QueueLED, &RxLED, 10) == pdTRUE) {
            switch(RxLED.function) {
                case LEDS_SET:
                    vLedsSet(RxLED.colour);
                    os_log_print(LOG_DEBUG, "Setting LED");
                    break;
                case LEDS_ON:
                    vLedsOn(RxLED.colour);
                    os_log_print(LOG_DEBUG, "Turning on LED");
                    break;
                case LEDS_OFF:
                    vLedsOff(RxLED.colour);
                    os_log_print(LOG_DEBUG, "Turning off LED");
                    break;
                case LEDS_TOGGLE:
                    vLedsToggle(RxLED.colour);
                    os_log_print(LOG_DEBUG, "Toggling LED");
                    break;
                default:
                    vLedsSet(LEDS_RED);
                    break;
            }
        }
    }
}

/*----------------------------------------------------------------------------*/