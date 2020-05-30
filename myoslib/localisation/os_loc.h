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
#define NODE_SIZE           11
#define NODE_ADDR_SIZE		6

#define	NODE_RSSI			0
#define NODE_TYPE			1
#define NODE_XY				2

/* Global typedef ------------------------------------------------------------*/
typedef enum {

    MOBILE_NODE,
    STATIC_NODE,
    US_STATIC_NODE

} NodeType_t;

typedef struct {

    NodeType_t type;
    uint8_t address[NODE_ADDR_SIZE];
    int8_t x_pos;
    int8_t y_pos;
    int16_t mmDist;
    uint16_t ultrasonic;
    int8_t prevRssi;

} Node;

/* Global Macros -------------------------------------------------------------*/
#define os_loc_updateNode_rssi(address, node)		updateNode(address, node, NODE_RSSI)
#define os_loc_updateNode_type(address, node)		updateNode(address, node, NODE_TYPE)
#define	os_loc_updateNode_xy(address, node)			updateNode(address, node, NODE_XY)

/* Function prototypes -------------------------------------------------------*/
extern void os_loc_init( void );
extern void os_loc_deinit( void );
extern void os_loc_queueNode(Node node);
extern void os_loc_addNode(uint8_t *address);
extern void os_loc_printNodes( void );
extern int os_loc_getX( void );
extern int os_loc_getY( void );
void updateNode( uint8_t *arrPos, Node node, int parameter);


#endif // S4434496_OS_LOC_H