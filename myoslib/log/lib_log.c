/** 
 **************************************************************
 * @file myoslib/log/lib_log.c
 * @author Cameron Stroud - 44344968
 * @date 05042020
 * @brief Log Library
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */


/* Includes ***************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#include "FreeRTOS.h"
#include "task.h"

#include "tiny_printf.h"
#include "log.h"

#include "os_log.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**
* @brief  Passes string and variable to print task
* @param  type: Type of log message to be printed (ERR, LOG, DBG)
* @param  string: String to be printed.
* @retval None
*/
extern void log_print(LogLevel_t type, const char *string, ... ) {

    char buffer[100];
    va_list argList;

    va_start(argList, string);

    tiny_vsnprintf(buffer, 100, string, argList);

	va_end(argList);

    os_log_queue_print(type, buffer);
    vTaskDelay(5);

}

