/** 
 **************************************************************
 * @file myoslib/hci/os_hci.h
 * @author Cameron Stroud - 44344968
 * @date 20042020
 * @brief HCI OS header file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */

#ifndef S4434496_OS_HCI_H
#define S4434496_OS_HCI_H

#include "FreeRTOS.h"
#include "semphr.h"


#include "hci_packet.h"

/* Global Defines ------------------------------------------------------------*/
#define     I2C_CMD        0x00

/* Global Macros -------------------------------------------------------------*/
#define     os_hci_clearuHCI()      os_hci_clearHCIarray('u')
#define     os_hci_clearHCI()       os_hci_clearHCIarray('i')

/* Global Variables ----------------------------------------------------------*/
SemaphoreHandle_t SemaphoreUart;
uint8_t uHCIdata[13];
int8_t HCIdata[13];

/* Function prototypes -------------------------------------------------------*/
extern void os_hci_write( Packet TxPacket );
extern void os_hci_read( Packet RxPacket );
extern void os_hci_setAccel( char axis );
extern void os_hci_clearHCIarray( char sign );
extern void os_hci_clearPacket( Packet *packet );
extern void os_hci_init( void );
extern void os_hci_deinit( void );


#endif // S4434496_OS_LOG_H