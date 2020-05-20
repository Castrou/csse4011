/** 
 **************************************************************
 * @file myoslib/wifi/hal_wifi.c
 * @author Cameron Stroud - 44344968
 * @date 02052020
 * @brief Bluetooth Comms OS file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */


/* Includes ***************************************************/
#include <string.h>

#include "board.h"
#include "argon.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "leds.h"
#include "log.h"
#include "os_log.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/*----------------------------------------------------------------------------*/

/**
* @brief  Initalises all wifi drivers
* @param  None
* @retval None
*/
extern void hal_wifi_init( void ) {



}

/*----------------------------------------------------------------------------*/