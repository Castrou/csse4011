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

#include "cli_log.h"
#include "os_log.h"


/* Private typedef -----------------------------------------------------------*/
typedef struct LogMessage {
    const char *message;
    LogLevel_t logType;
} LogMessage;

/* Private define ------------------------------------------------------------*/
#define Log_PRIORITY (tskIDLE_PRIORITY + 2)
#define Log_STACK_SIZE (configMINIMAL_STACK_SIZE * 5)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
QueueHandle_t QueueLog;
TaskHandle_t LogHandler;
LogMessage Log;

/* Private function prototypes -----------------------------------------------*/
void Log_Task( void );

/*----------------------------------------------------------------------------*/

/**
* @brief  Queues Log to be printed
* @param  input: string of Serial input
* @retval None
*/
extern void os_log_print( LogLevel_t type, const char *payload, ... ) {

    char buffer[100];
    va_list argList;

    va_start(argList, payload);

    tiny_vsnprintf(buffer, 100, payload, argList);

	va_end(argList);

    Log.message = buffer;
    Log.logType = type;

    if(xQueueSendToBack(QueueLog, (void *) &Log, 
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
extern void os_log_init( void ) {

    /* Driver initialisation functions */
    cli_log_init();

    /* Create Semaphores */
    SemaphoreErrorLog = xSemaphoreCreateBinary();
    SemaphoreInfoLog = xSemaphoreCreateBinary();
    SemaphoreDebugLog = xSemaphoreCreateBinary();

    xSemaphoreGive(SemaphoreErrorLog);
    xSemaphoreGive(SemaphoreInfoLog);
    xSemaphoreGive(SemaphoreDebugLog);

    /* Create task */
    xTaskCreate((void *) &Log_Task, "Log Task", \
                    Log_STACK_SIZE, NULL, Log_PRIORITY, &LogHandler);
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Initalises all Log drivers
* @param  None
* @retval None
*/
extern void os_log_deinit( void ) {

    /* Driver deinitialisation functions */
    cli_log_deinit();

    /* Remove Semaphores */
    vSemaphoreDelete(SemaphoreErrorLog);
    vSemaphoreDelete(SemaphoreInfoLog);
    vSemaphoreDelete(SemaphoreDebugLog);

    /* Remove task */
    vTaskDelete(LogHandler);
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Log printing task
* @param  None
* @retval None
*/
void Log_Task( void ) {

    LogMessage IncomingLog;

    /* Create Queue for retrieving from Serial ISR */
    QueueLog = xQueueCreate(10, sizeof(IncomingLog));

    for ( ;; ) {
        if (xQueueReceive(QueueLog, &IncomingLog, 10) == pdTRUE) {   
            switch(IncomingLog.logType) {
                case LOG_ERROR:
                    if(xSemaphoreTake(SemaphoreErrorLog, (TickType_t) 10) == pdTRUE) {
                        eLog(LOG_APPLICATION, IncomingLog.logType, \
                                "\e[1;31m%s\e[0m\r\n", IncomingLog.message);
                        xSemaphoreGive(SemaphoreErrorLog);
                    }
                    break;
                case LOG_INFO:
                    if(xSemaphoreTake(SemaphoreInfoLog, (TickType_t) 10) == pdTRUE) {
                        eLog(LOG_APPLICATION, IncomingLog.logType, \
                                "\e[1;32m%s\e[0m\r\n", IncomingLog.message);
                        xSemaphoreGive(SemaphoreInfoLog);
                    }
                    break;
                case LOG_DEBUG:
                    if(xSemaphoreTake(SemaphoreDebugLog, (TickType_t) 10) == pdTRUE) {
                        eLog(LOG_APPLICATION, IncomingLog.logType, \
                                "\e[1;34m%s\e[0m\r\n", IncomingLog.message);
                        xSemaphoreGive(SemaphoreDebugLog);
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

/*----------------------------------------------------------------------------*/