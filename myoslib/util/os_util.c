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
#include "semphr.h"

#include "leds.h"

#include "os_util.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LEDCycle_PRIORITY (tskIDLE_PRIORITY + 2)
#define LEDCycle_STACK_SIZE (configMINIMAL_STACK_SIZE * 5)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TaskHandle_t LEDCycleHandler;

/* Private function prototypes -----------------------------------------------*/
void LEDCycle_Task( void );

/**
* @brief  Initalises all Log drivers
* @param  None
* @retval None
*/
extern void os_util_init( void ) {

    /* Create Semaphores */
    SemaphoreLED = xSemaphoreCreateBinary();

    xSemaphoreGive(SemaphoreLED);

    /* Create task */
    xTaskCreate((void *) &LEDCycle_Task, "LED Cycle Task", \
                    LEDCycle_STACK_SIZE, NULL, LEDCycle_PRIORITY, &LEDCycleHandler);
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
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Log printing task
* @param  None
* @retval None
*/
void LEDCycle_Task( void ) {

    eLEDs_t led_colour;

    for ( ;; ) {
        
        /* Change LED colour */
        switch(led_colour) {
            case LEDS_RED:      // if RED go to GREEN
                led_colour = LEDS_GREEN;
                break;
            case LEDS_GREEN:    // if GREEN go to BLUE
                led_colour = LEDS_BLUE;
                break;
            case LEDS_BLUE:     // if BLUE go to RED
                led_colour = LEDS_RED;
                break;
            default:
                led_colour = LEDS_RED;
                break;
        }
        vLedsSet(led_colour);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

/*----------------------------------------------------------------------------*/