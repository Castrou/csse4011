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

/* Global Variables ----------------------------------------------------------*/
SemaphoreHandle_t SemaphoreUart;

/* Function prototypes -------------------------------------------------------*/
extern void os_hci_write( Packet TxPacket );
extern void os_hci_read( Packet RxPacket );
extern void os_hci_setEvent( char cmd );
extern void os_hci_init( void );
extern void os_hci_deinit( void );


#endif // S4434496_OS_LOG_H