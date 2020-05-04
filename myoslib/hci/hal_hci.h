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

/* Defines --------------------------------------------------------------------*/
#define		X_AXIS			'x'
#define		Y_AXIS			'y'
#define		Z_AXIS			'z'
#define		ALL_AXES		'a'

/* Function prototypes -------------------------------------------------------*/
extern void hal_hci_init( void );
extern void hal_hci_send_packet( Packet packet );
extern void hal_hci_addDatafield( Packet *destPacket, Datafield newField );
extern Datafield hal_hci_build_datafield( char cmd, uint8_t sid,
                                            uint8_t regaddr, uint8_t regval);

#endif // S4434496_HAL_HCI_H