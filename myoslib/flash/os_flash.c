/** 
 **************************************************************
 * @file myoslib/log/os_log.c
 * @author Cameron Stroud - 44344968
 * @date 04042020
 * @brief Log OS file
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

#include "gpio.h"
#include "leds.h"
#include "log.h"
#include "rtc.h"
#include "watchdog.h"

#include "test_reporting.h"
#include "tiny_printf.h"

#include "device_nvm.h"

#include "unified_comms_serial.h"
#include "onboard_logger.h"

#include "os_log.h"
#include "hal_flash.h"
#include "os_flash.h"


/* Private typedef -----------------------------------------------------------*/
typedef struct FlashMessage {
    char payload[100];
    int payloadLen;
} FlashMessage;

/* Private define ------------------------------------------------------------*/
#define Flash_PRIORITY (tskIDLE_PRIORITY + 2)
#define Flash_STACK_SIZE (configMINIMAL_STACK_SIZE * 5)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
QueueHandle_t QueueFlashWrite;
QueueHandle_t QueueFlashRead;
TaskHandle_t FlashHandler;
FlashMessage Flash;

/* Private function prototypes -----------------------------------------------*/
void Flash_Task( void );

/*----------------------------------------------------------------------------*/

/**
* @brief  Reads flash
* @param  input: string of Serial input
* @retval None
*/
extern void os_flash_read( void *dataBuffer, uint32_t blockSize ) {

    hal_flash_read( 0, 0, &dataBuffer, blockSize );
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Queues flash write
* @param  input: string of Serial input
* @retval None
*/
extern void os_flash_write(const char *payload, ... ) {

    char buffer[100];
    va_list argList;

    va_start(argList, payload);

    tiny_vsnprintf(buffer, 100, payload, argList);

	va_end(argList);

    pvMemcpy(Flash.payload, buffer, 100);
    Flash.payloadLen = strlen(buffer);

    if(xQueueSendToBack(QueueFlashWrite, (void *) &Flash, 
        (portTickType) 10) != pdPASS) 
    {
        // portENTER_CRITICAL();
        // // debug_printf("Failed to post the message, after 10 ticks.\n\r");
        // portEXIT_CRITICAL();
    }

    vTaskDelay(5);
}

/*----------------------------------------------------------------------------*/


/**
* @brief  Initalises all Log drivers
* @param  None
* @retval None
*/
extern void os_flash_init( void ) {

    /* Initialise Drivers */
    hal_flash_init();

    xTaskCreate((void *) &Flash_Task, "Flash Task", 
                    Flash_STACK_SIZE, NULL, Flash_PRIORITY, &FlashHandler);
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Initalises all Log drivers
* @param  None
* @retval None
*/
extern void os_flash_deinit( void ) {

    vTaskDelete(FlashHandler);
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Log flashing task
* @param  None
* @retval None
*/
void Flash_Task( void ) {

    FlashMessage IncomingFlash;
    eModuleError_t eResult;

    UNUSED(eResult);

    /* Create Queue for retrieving from Serial ISR */
    QueueFlashWrite = xQueueCreate(5, sizeof(IncomingFlash));

    for ( ;; ) {
        if (xQueueReceive(QueueFlashWrite, &IncomingFlash, 10) == pdTRUE) {   
             
            hal_flash_write(0, IncomingFlash.payload, Flash.payloadLen);
              
        }
    }
}

/*----------------------------------------------------------------------------*/