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

#include "os_log.h"
#include "cli_log.h"


/* Private typedef -----------------------------------------------------------*/
typedef struct LogMessage {
    const char *message;
    int logType;
} LogMessage;

/* Private define ------------------------------------------------------------*/
#define Log_PRIORITY (tskIDLE_PRIORITY + 2)
#define Log_STACK_SIZE (configMINIMAL_STACK_SIZE * 5)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// LogMessage Log;
QueueHandle_t QueueLog;
TaskHandle_t LogHandler;
LogMessage Log;

/* Private function prototypes -----------------------------------------------*/
void Log_Task( void );


/**
* @brief  Queues Log to be printed
* @param  input: string of Serial input
* @retval None
*/
extern void os_log_queue_print( LogLevel_t type, const char *payload ) {

    Log.message = payload;
    Log.logType = type;

    if(xQueueSendToBack(QueueLog, (void *) &Log, 
        (portTickType) 10) != pdPASS) 
    {
        // portENTER_CRITICAL();
        // // debug_printf("Failed to post the message, after 10 ticks.\n\r");
        // portEXIT_CRITICAL();
    }
}


/**
* @brief  Initalises all Log drivers
* @param  None
* @retval None
*/
extern void os_log_init( void ) {

    /* Driver initialisation functions */
    cli_log_init();

    /* Create task */
    xTaskCreate((void *) &Log_Task, "Log Task", \
                    Log_STACK_SIZE, NULL, Log_PRIORITY, &LogHandler);
}

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
            portENTER_CRITICAL();
            eLog(LOG_APPLICATION, IncomingLog.logType, "%s", IncomingLog.message);
            portEXIT_CRITICAL();
            // vTaskDelay(5);
        }
    }
}