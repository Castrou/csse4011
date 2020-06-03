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
#define NODE_US             3

#define NODE_TIMEOUT        3
#define NODE_MM_THRESH      1330

/* Global typedef ------------------------------------------------------------*/
typedef enum {

    MOBILE_NODE,
    STATIC_NODE,
    US_STATIC_NODE

} NodeType_t;

typedef enum {

	OUTOF_CONTACT = 0,
	IN_CONTACT

} ContactType_t;

typedef struct {

	ContactType_t contactType;
	uint32_t contactTime;
	uint32_t prevTime;

} Contact;

typedef struct {

    NodeType_t type;
    uint8_t address[NODE_ADDR_SIZE];
    int8_t x_pos;
    int8_t y_pos;
    int16_t mmDist;
    uint16_t ultrasonic;
    int8_t prevRssi;
	Contact contact;

} Node;

/* Global Macros -------------------------------------------------------------*/
#define os_loc_updateNode_rssi(address, node)		    updateNode(address, node, NODE_RSSI)
#define os_loc_updateNode_type(address, node)		    updateNode(address, node, NODE_TYPE)
#define	os_loc_updateNode_xy(address, node)			    updateNode(address, node, NODE_XY)
#define	os_loc_updateNode_ultrasonic(address, node)		updateNode(address, node, NODE_US)

/* Function prototypes -------------------------------------------------------*/
extern void os_loc_init( void );
extern void os_loc_deinit( void );
extern void os_loc_enableTx( void );
extern void os_loc_disableTx( void );
extern void os_loc_queueNode(Node node);
extern void os_loc_addNode(uint8_t *address);
extern void os_loc_printNodes( void );
extern void os_loc_printLog( void );
extern int os_loc_getX( void );
extern int os_loc_getY( void );
void updateNode( uint8_t *arrPos, Node node, int parameter);


#endif // S4434496_OS_LOC_H