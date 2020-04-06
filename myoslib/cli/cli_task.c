/** 
 **************************************************************
 * @file myoslib/cli/cli_task.c
 * @author Cameron Stroud - 44344968
 * @date 04042020
 * @brief CLI Task source file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */


/* Includes ***************************************************/
#include <string.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "queue.h"
#include "FreeRTOS_CLI.h"

#include "log.h"

#include "os_log.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct CLIInput {
	char Payload[100];
} CLIInput;

/* Private define ------------------------------------------------------------*/
#define CLI_PRIORITY (tskIDLE_PRIORITY + 2)
#define CLI_STACK_SIZE (configMINIMAL_STACK_SIZE * 5)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
CLIInput CLI;
QueueHandle_t QueueCLI;
TaskHandle_t CLIHandler = NULL;

/* Private function prototypes -----------------------------------------------*/
void CLI_Task(void);


/**
* @brief  Queues Serial input
* @param  input: string of Serial input
* @retval None
*/
extern void cli_task_queue_serial( const char *input ) {

    strcpy(CLI.Payload, input);
    if(xQueueSendToBack(QueueCLI, (void *) &CLI, 
        (portTickType) 10) != pdPASS) 
    {
        // portENTER_CRITICAL();
        // // debug_printf("Failed to post the message, after 10 ticks.\n\r");
        // portEXIT_CRITICAL();
    }
}


/**
* @brief  Initalises CLI task
* @param  None
* @retval None
*/
extern void cli_task_init( void ) {

    xTaskCreate((void *) &CLI_Task, "CLI Task", \
                    CLI_STACK_SIZE, NULL, CLI_PRIORITY, &CLIHandler);

}


/**
* @brief  Main CLI Task
* @param  None
* @retval None
*/
void CLI_Task( void ) {

    CLIInput Input;
	char *pcOutputString;
	BaseType_t xReturned;

    /* Initialise pointer to CLI output buffer. */
	pcOutputString = FreeRTOS_CLIGetOutputBuffer();

    /* Create Queue for retrieving from Serial ISR */
    QueueCLI = xQueueCreate(10, sizeof(Input));

    for( ;; ) {

        if (xQueueReceive(QueueCLI, &Input, 10) == pdTRUE) {   

            xReturned = pdTRUE;
            /* Process command input string. */
            while (xReturned != pdFALSE) {

                /* Returns pdFALSE, when all strings have been returned */
                xReturned = FreeRTOS_CLIProcessCommand( Input.Payload, \
                pcOutputString, configCOMMAND_INT_MAX_OUTPUT_SIZE );

                vTaskDelay(5);	//Must delay between debug_printfs.
            }
        }
    }
}