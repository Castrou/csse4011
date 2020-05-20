/** 
 **************************************************************
 * @file myoslib/localisation/cli_loc.h
 * @author Cameron Stroud - 44344968
 * @date 19052020
 * @brief Localisation Commands header file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */

#ifndef S4434496_CLI_LOC_H
#define S4434496_CLI_LOC_H

/* Function prototypes -------------------------------------------------------*/
extern void cli_hci_init( void );
extern void cli_hci_deinit( void );
extern void os_hci_queue_write( Packet TxPacket );

/* Commands ------------------------------------------------------------------*/
BaseType_t nodePosCommand(char * pcWriteBuffer, size_t xWriteBufferLen,
                        const char * pcCommandString);
BaseType_t nodeTypeCommand(char * pcWriteBuffer, size_t xWriteBufferLen,
                        const char * pcCommandString);


#endif // S4434496_CLI_HCI_H