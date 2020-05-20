/** 
 **************************************************************
 * @file myoslib/wifi/os_wifi.h
 * @author Cameron Stroud - 44344968
 * @date 19052020
 * @brief Wifi Comms OS header file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */

#ifndef S4434496_OS_WIFI_H
#define S4434496_OS_WIFI_H

/* Global Defines ------------------------------------------------------------*/
/* Global Variables ----------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
extern void os_wifi_init( void );
extern void os_wifi_deinit( void );
extern void os_wifi_send( void );

#endif // S4434496_OS_WIFI_H