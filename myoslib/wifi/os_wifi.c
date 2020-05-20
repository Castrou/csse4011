/** 
 **************************************************************
 * @file myoslib/wifi/os_wifi.c
 * @author Cameron Stroud - 44344968
 * @date 19052020
 * @brief Wifi Comms OS file
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


/* Private typedef -----------------------------------------------------------*/
typedef struct WifiPkt_t {
    
    const uint8_t address;
    uint8_t range;

} WifiPkt_t;
/* Private define ------------------------------------------------------------*/
#define WIFI_PRIORITY (tskIDLE_PRIORITY + 2)
#define WIFI_STACK_SIZE (configMINIMAL_STACK_SIZE * 5)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
QueueHandle_t QueueWifi;
TaskHandle_t WifiHandler;
WifiPkt_t xWifiPacket;

/* Private function prototypes -----------------------------------------------*/
void Wifi_Task( void );

/*----------------------------------------------------------------------------*/

/**
* @brief  Queues Wifi message to be sent
* @param  input: string of Serial input
* @retval None
*/
extern void os_wifi_send( void ) {

    if(xQueueSendToBack(QueueWifi, (void *) &xWifiPacket, 
        (portTickType) 10) != pdPASS) 
    {
        // portENTER_CRITICAL();
        // // debug_printf("Failed to post the message, after 10 ticks.\n\r");
        // portEXIT_CRITICAL();
    }

}

/*----------------------------------------------------------------------------*/

/**
* @brief  Initalises all wifi drivers
* @param  None
* @retval None
*/
extern void os_wifi_init( void ) {

    /* Driver initialisation functions */
	hal_wifi_init();

    /* Create task */
    xTaskCreate((void *) &Wifi_Task, "Wifi Task", \
                    WIFI_STACK_SIZE, NULL, WIFI_PRIORITY, &WifiHandler);
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Deinitialises Wifi
* @param  None
* @retval None
*/
extern void os_wifi_deinit( void ) {

    /* Driver deinitialisation functions */
    /* Remove Semaphores */
    /* Remove task */
    vTaskDelete(WifiHandler);
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Wifi Sending task
* @param  None
* @retval None
*/
void Wifi_Task( void ) {

    WifiPkt_t IncomingWifi;

    /* Create Queue for retrieving Wifi  */
    QueueWifi = xQueueCreate(10, sizeof(IncomingWifi));

    for ( ;; ) {
        // vLedsToggle(LEDS_GREEN);
        if (xQueueReceive(QueueWifi, &IncomingWifi, 10) == pdTRUE) {   
            
        }

        vTaskDelay(5);
    }
}

/*----------------------------------------------------------------------------*/