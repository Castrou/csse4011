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
#define     NODE_ADDR_SIZE   6
/* Global typedef ------------------------------------------------------------*/
typedef struct Node {

    int x_pos;
    int y_pos;
    uint8_t address[NODE_ADDR_SIZE];
    int8_t prevRssi;
    int32_t dist100;

} Node;

/* Global Macros -------------------------------------------------------------*/
/* Global Variables ----------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
extern void os_loc_init( void );
extern void os_loc_deinit( void );
extern void os_loc_sendNode(Node node);
void update_node(int arrPos, uint8_t *address, int8_t rssi, 
					int xPos, int yPos);

#endif // S4434496_OS_LOC_H