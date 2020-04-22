/** 
 **************************************************************
 * @file myoslib/hci/hal_hci.h
 * @author Cameron Stroud - 44344968
 * @date 20042020
 * @brief HCI Driver Header
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */

#ifndef S4434496_HAL_HCI_H
#define S4434496_HAL_HCI_H

#include "argon.h"
#include "hci_packet.h"

/* Function prototypes -------------------------------------------------------*/
extern void hal_hci_init( void );
extern void hal_hci_send_packet( Packet packet );
extern void hal_hci_addDatafield( Packet *destPacket, Datafield newField );
extern Datafield hal_hci_build_datafield( char *command, char *sid, char *regaddr, char *regval);

extern void uart_write( const char *payload, ... );

#endif // S4434496_HAL_HCI_H