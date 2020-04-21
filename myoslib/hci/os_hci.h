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

/* Global Variables ----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
extern void os_hci_queue_write( const char *payload );
extern void os_hci_init( void );
extern void os_hci_deinit( void );


#endif // S4434496_OS_LOG_H