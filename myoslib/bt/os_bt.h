/** 
 **************************************************************
 * @file myoslib/bt/os_bt.h
 * @author Cameron Stroud - 44344968
 * @date 02052020
 * @brief Bluetooth Comms OS header file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */

#ifndef S4434496_OS_BT_H
#define S4434496_OS_BT_H

#include "semphr.h"

/* Global Variables ----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
extern void os_bt_init( void );
extern void os_bt_deinit( void );


#endif // S4434496_OS_BT_H