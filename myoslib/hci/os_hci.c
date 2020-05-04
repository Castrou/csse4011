/** 
 **************************************************************
 * @file myoslib/hci/os_hci.c
 * @author Cameron Stroud - 44344968
 * @date 20042020
 * @brief HCI OS source file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */


/* Includes ***************************************************/
#include "string.h"
#include "stdio.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"

#include "leds.h"
#include "log.h"

#include "unified_comms_serial.h"

#include "os_log.h"

#include "hal_hci.h"
#include "cli_hci.h"
#include "os_hci.h"
#include "hci_packet.h"

/* Private typedef -----------------------------------------------------------*/
#define     X_BIT       (1<<0)
#define     Y_BIT       (1<<1)
#define     Z_BIT       (1<<2)

#define		G_CONVERT	16384.0

/* Private define ------------------------------------------------------------*/
#define HCI_PRIORITY (tskIDLE_PRIORITY + 2)
#define HCI_STACK_SIZE (configMINIMAL_STACK_SIZE * 1)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TaskHandle_t HCIHandler;
QueueHandle_t QueueHCIWrite;
QueueHandle_t QueueHCIRead;
EventGroupHandle_t EventAccel;

/* Private function prototypes -----------------------------------------------*/
void HCI_Task( void );

/*----------------------------------------------------------------------------*/

/**
* @brief  Initalises all HCI drivers
* @param  None
* @retval None
*/
extern void os_hci_init( void ) {

    /* Initialise HCI */
    hal_hci_init();
    cli_hci_init();

	/* Creat Semaphore */
	SemaphoreUart = xSemaphoreCreateBinary();

	xSemaphoreGive(SemaphoreUart);
	
    /* Create task */
    xTaskCreate((void *) &HCI_Task, "HCI Task",
                    HCI_STACK_SIZE, NULL, HCI_PRIORITY, &HCIHandler);

}

/*----------------------------------------------------------------------------*/

/**
* @brief  Initalises all Log drivers
* @param  None
* @retval None
*/
extern void os_hci_deinit( void ) {

	/* Delete Semaphore */
	vSemaphoreDelete(SemaphoreUart);

    /* Delete Task */
    vTaskDelete(HCIHandler);
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Add to packet to write queue
* @param  TxPacket: Packet struct to transmit
* @retval None
*/
extern void os_hci_write( Packet TxPacket ) {

    if(xQueueSendToBack(QueueHCIWrite, (void *) &TxPacket, (portTickType) 10) != pdPASS) {
        // portENTER_CRITICAL();
        // // debug_printf("Failed to post the message, after 10 ticks.\n\r");
        // portEXIT_CRITICAL();
    }
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Add to packet to read queue
* @param  RxPacket: Packet struct to read
* @retval None
*/
extern void os_hci_read( Packet RxPacket ) {

    if(xQueueSendToBack(QueueHCIRead, (void *) &RxPacket, (portTickType) 10) != pdPASS) {
        // portENTER_CRITICAL();
        // // debug_printf("Failed to post the message, after 10 ticks.\n\r");
        // portEXIT_CRITICAL();
    }
}

/*----------------------------------------------------------------------------*/

/**
* @brief  Set the event bits for what command is expected in HCI read
* @param  cmd: Command entered
* @retval None
*/
extern void os_hci_setEvent( char cmd ) {
    
    EventBits_t bits = 0;

    switch(cmd) {
        case X_AXIS:
            bits |= X_BIT;
            break;

        case Y_AXIS:
            bits |= Y_BIT;
            break;

        case Z_AXIS:
            bits |= Z_BIT;
            break;

        case ALL_AXES:
            bits |= (X_BIT | Y_BIT | Z_BIT);
            break;

        default:
            break;
    }
    
    xEventGroupClearBits(EventAccel, 0xFF);
    xEventGroupSetBits(EventAccel, bits);
}

/*----------------------------------------------------------------------------*/

/**
* @brief  HCI task - writing and reading from UART
* @param  None
* @retval None
*/
void HCI_Task( void ) {
    
    EventBits_t axisBits;
    Packet TxPacket;
    Packet RxPacket;
    int dataPos;
	double incomingData;
	int16_t incomingRaw;

    QueueHCIWrite = xQueueCreate(5, 2*sizeof(TxPacket));
    QueueHCIRead = xQueueCreate(5, 2*sizeof(RxPacket));
    EventAccel = xEventGroupCreate();

    for ( ;; ) {
        if (xQueueReceive(QueueHCIWrite, &TxPacket, 10) == pdTRUE) {   
            /* Write Packet */
            hal_hci_send_packet(TxPacket);
        }

        if (xQueueReceive(QueueHCIRead, &RxPacket, 10) == pdTRUE) {   
            dataPos = 0;

            /* Read packet */
            if (RxPacket.type == 2) { // Response

                switch((RxPacket.data[0].i2caddr>>1)) {
                    case LSM6DSL:
                        axisBits = xEventGroupGetBits(EventAccel);

                        /* Handle X Info */
                        if ((axisBits & X_BIT) == X_BIT) {
                            incomingRaw = (RxPacket.data[dataPos].regval<<8) | 
                                                RxPacket.data[dataPos+1].regval;
                            incomingData = (float) (incomingRaw/G_CONVERT);
                            xEventGroupClearBits(EventAccel, X_BIT);
                            os_log_print(LOG_INFO, "X Acceleration: %.2fg(s)", incomingData);
                            dataPos += 2;
                        }
                        
                        /* Handle Y Info */
                        if ((axisBits & Y_BIT) == Y_BIT) {
                            incomingRaw = (RxPacket.data[dataPos].regval<<8) | 
                                                RxPacket.data[dataPos+1].regval;
                            incomingData = ((float)incomingRaw/(float)G_CONVERT);
                            xEventGroupClearBits(EventAccel, Y_BIT);
                            os_log_print(LOG_INFO, "Y Acceleration: %.2fg(s)", incomingData);
                            dataPos += 2;
                        }

                        /* Handle Z Info */
                        if ((axisBits & Z_BIT) == Z_BIT) {
                            incomingRaw = (RxPacket.data[dataPos].regval<<8) | 
                                                RxPacket.data[dataPos+1].regval;
                            incomingData = (float) (incomingRaw/G_CONVERT);
                            xEventGroupClearBits(EventAccel, Z_BIT);
                            os_log_print(LOG_INFO, "Z Acceleration: %.2fg(s)", incomingData);

                        }
                        break;

                    default:
                        for(int i = 0; i < RxPacket.dataCnt; i++) {
                            os_log_print(LOG_INFO, "RECV(SID%d): REGADDR=0x%02x REGVAL=0x%02x",
                                            RxPacket.data[i].sid, RxPacket.data[i].regaddr,
                                            RxPacket.data[i].regval);
                        }
                        break;
                }
            }
			xSemaphoreGive(SemaphoreUart);
        }

        vTaskDelay(5);
    }
}

/*----------------------------------------------------------------------------*/