/** 
 **************************************************************
 * @file myoslib/localisation/os_loc.h
 * @author Cameron Stroud - 44344968
 * @date 18052020
 * @brief Localisation OS header file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */

#ifndef S4434496_OS_LOC_H
#define S4434496_OS_LOC_H

#include "FreeRTOS.h"
#include "semphr.h"

/* Global Defines ------------------------------------------------------------*/
/* Global typedef ------------------------------------------------------------*/
typedef struct Node {

    int x_pos;
    int y_pos;
    const uint8_t address;
    int8_t prevRssi;

} Node;

/* Global Macros -------------------------------------------------------------*/
/* Global Variables ----------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
extern void os_loc_init( void );
extern void os_loc_deinit( void );

#endif // S4434496_OS_LOC_H