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

/* Global Defines ------------------------------------------------------------*/
#define 		SCAN_BIT		(1<<0)
/* Global Variables ----------------------------------------------------------*/
EventGroupHandle_t EventBT;

/* Function prototypes -------------------------------------------------------*/
extern void os_bt_init( void );
extern void os_bt_deinit( void );
extern void os_bt_send_unsigned( eTdfIds_t type, eTdfTimestampType_t timestamp, 
								xTdfTime_t time, uint8_t sendData[13] );
extern void os_bt_send_signed( eTdfIds_t type, eTdfTimestampType_t timestamp, 
								xTdfTime_t time, int8_t sendData[13] );


#endif // S4434496_OS_BT_H