/** 
 **************************************************************
 * @file myoslib/hci/cli_hci.h
 * @author Cameron Stroud - 44344968
 * @date 21042020
 * @brief HCI Commands header file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */

#ifndef S4434496_CLI_HCI_H
#define S4434496_CLI_HCI_H

#include "hci_packet.h"

/* Function prototypes -------------------------------------------------------*/
extern void cli_hci_init( void );
extern void cli_hci_deinit( void );
extern void os_hci_queue_write( Packet TxPacket );

/* Commands ------------------------------------------------------------------*/
BaseType_t i2cCommand(char * pcWriteBuffer, size_t xWriteBufferLen,
                        const char * pcCommandString);
BaseType_t imuCommand(char * pcWriteBuffer, size_t xWriteBufferLen,
                        const char * pcCommandString);
BaseType_t hciCommand(char * pcWriteBuffer, size_t xWriteBufferLen,
                        const char * pcCommandString);


#endif // S4434496_CLI_HCI_H