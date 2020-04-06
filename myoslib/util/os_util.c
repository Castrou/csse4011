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

#include "os_util.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**
* @brief  Initalises all Log drivers
* @param  None
* @retval None
*/
extern void os_util_init( void ) {

    /* Create Semaphores */
    SemaphoreLED = xSemaphoreCreateBinary();

    xSemaphoreGive(SemaphoreLED);
}


/**
* @brief  Initalises all Log drivers
* @param  None
* @retval None
*/
extern void os_util_deinit( void ) {

    /* Remove Semaphores */
    vSemaphoreDelete(SemaphoreLED);
}
