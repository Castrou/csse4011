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


/* Global Defines ------------------------------------------------------------*/
#define        MAX_DF      13

/* Global typedef ------------------------------------------------------------*/
typedef enum Sensor_I2C {

    LSM6DSL = 0x6A,
    LPS22HB = 0x5D,
    VL53L0X = 0x29

} Sensor_I2C;

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
    Datafield data[MAX_DF];

} Packet;

#endif // S4434496_HCI_PACKET_H