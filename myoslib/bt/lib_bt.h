/** 
 **************************************************************
 * @file myoslib/bt/lib_bt.h
 * @author Cameron Stroud - 44344968
 * @date 04052020
 * @brief BT Library header file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */

#ifndef S4434496_LIB_BT_H
#define S4434496_LIB_BT_H

#include <stdint.h>
#include "hci_packet.h"

/* Global Defines ------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
extern void send_bt( int tdf_id, char sign );
extern void lib_bt_tdf_lsm6dsl( void );
extern void lib_bt_tdf_3dpose( void );
extern void lib_bt_tdf_height_msl( void );

#endif // S4434496_LIB_BT_H