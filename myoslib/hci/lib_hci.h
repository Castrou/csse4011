/** 
 **************************************************************
 * @file myoslib/hci/lib_hci.h
 * @author Cameron Stroud - 44344968
 * @date 04052020
 * @brief HCI Library header file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */

#ifndef S4434496_LIB_HCI_H
#define S4434496_LIB_HCI_H

#include <stdint.h>
#include "hci_packet.h"

/* Global Defines ------------------------------------------------------------*/
/* Defines --------------------------------------------------------------------*/
#define		X_AXIS			'x'
#define		Y_AXIS			'y'
#define		Z_AXIS			'z'
#define		ALL_AXES		'a'

#define     NO_SPEC         '\0'

#define		X_HREGADDR		    0x29
#define		X_LREGADDR		    0x28
#define 	Y_HREGADDR		    0x2B
#define 	Y_LREGADDR		    0x2A
#define 	Z_HREGADDR		    0x2D
#define 	Z_LREGADDR		    0x2C

#define     PRESS_HREGADDR      0x2A
#define     PRESS_LREGADDR      0x29
#define     PRESS_XLREGADDR     0x28

/* Global Macro --------------------------------------------------------------*/
#define     lib_hci_request_accel(axis)     lib_hci_request_data(LSM6DSL, axis)
#define     lib_hci_request_baro()          lib_hci_request_data(LPS22HB, NO_SPEC)

/* Function prototypes -------------------------------------------------------*/
extern void lib_hci_request_data( uint8_t device, uint8_t specifier );


#endif // S4434496_LIB_HCI_H