/** 
 **************************************************************
 * @file myoslib/hci/hal_hci.h
 * @author Cameron Stroud - 44344968
 * @date 20042020
 * @brief HCI Driver Header
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */

#ifndef S4434496_HCI_PACKET_H
#define S4434496_HCI_PACKET_H

/* Public typedef ------------------------------------------------------------*/
typedef struct Datafield {

    uint8_t sid;
    uint8_t i2caddr;
    uint8_t regaddr;
    uint8_t regval;

} Datafield;

typedef struct Packet {

    uint8_t preamble;
    uint8_t type;
    uint8_t dataCnt;
    Datafield data[2];

} Packet;

#endif // S4434496_HCI_PACKET_H