/** 
 **************************************************************
 * @file myoslib/hci/lib_hci.c
 * @author Cameron Stroud - 44344968
 * @date 04052020
 * @brief HCI Library source file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */


/* Includes ***************************************************/
#include "string.h"
#include "stdlib.h"

#include "board.h"

#include "leds.h"
#include "log.h"

#include "hal_hci.h"
#include "os_hci.h"
#include "hci_packet.h"
#include "os_log.h"

#include "lib_hci.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define     COMMAND			1
#define     SID				2
#define     REGADDR			3
#define     REGVAL			4

#define		READ			'r'

#define     PEDO            'p'
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/


/*----------------------------------------------------------------------------*/

/**
* @brief  Function for requesting SCU data
* @param  device: I2C Address for desired device reading
* @param  specifier: Accelerometer: 'x' 'y' 'z' or 'a'
*                    No other implemented options - default to register address
* @retval None
*/
extern void lib_hci_request_data( uint8_t device, uint8_t specifier ) {
    Packet TxPacket;
    Datafield buffField;

    TxPacket.dataCnt = 0;
    TxPacket.type = 1;
        
    if (device == LSM6DSL) {
        switch(specifier) {
            case X_AXIS:
                /* Setup X Packet */
                buffField = hal_hci_build_datafield(READ, IMU, X_HREGADDR, 0);
                hal_hci_addDatafield(&TxPacket, buffField);
                buffField = hal_hci_build_datafield(READ, IMU, X_LREGADDR, 0);
                hal_hci_addDatafield(&TxPacket, buffField);
                /* Send X Packet */
                if (xSemaphoreTake(SemaphoreUart, (TickType_t) 10) == pdTRUE) {
                    os_hci_setAccel(X_AXIS);
                    os_hci_write(TxPacket);
                }
                break;

            case Y_AXIS:
                /* Setup Y Packet */
                buffField = hal_hci_build_datafield(READ, IMU, Y_HREGADDR, 0);
                hal_hci_addDatafield(&TxPacket, buffField);
                buffField = hal_hci_build_datafield(READ, IMU, Y_LREGADDR, 0);
                hal_hci_addDatafield(&TxPacket, buffField);
                /* Send Y Packet */
                if (xSemaphoreTake(SemaphoreUart, (TickType_t) 10) == pdTRUE) {
                    os_hci_setAccel(Y_AXIS);
                    os_hci_write(TxPacket);
                }
                break;

            case Z_AXIS:
                /* Setup Z Packet */
                buffField = hal_hci_build_datafield(READ, IMU, Z_HREGADDR, 0);
                hal_hci_addDatafield(&TxPacket, buffField);
                buffField = hal_hci_build_datafield(READ, IMU, Z_LREGADDR, 0);
                hal_hci_addDatafield(&TxPacket, buffField);
                /* Send Z Packet */
                if (xSemaphoreTake(SemaphoreUart, (TickType_t) 10) == pdTRUE) {
                    os_hci_setAccel(Z_AXIS);
                    os_hci_write(TxPacket);
                }
                break;

            case ALL_AXES:
                /* Setup X Packet */
                buffField = hal_hci_build_datafield(READ, IMU, X_HREGADDR, 0);
                hal_hci_addDatafield(&TxPacket, buffField);
                buffField = hal_hci_build_datafield(READ, IMU, X_LREGADDR, 0);
                hal_hci_addDatafield(&TxPacket, buffField);

                /* Setup Y Packet */
                buffField = hal_hci_build_datafield(READ, IMU, Y_HREGADDR, 0);
                hal_hci_addDatafield(&TxPacket, buffField);
                buffField = hal_hci_build_datafield(READ, IMU, Y_LREGADDR, 0);
                hal_hci_addDatafield(&TxPacket, buffField);

                /* Setup Z Packet */
                buffField = hal_hci_build_datafield(READ, IMU, Z_HREGADDR, 0);
                hal_hci_addDatafield(&TxPacket, buffField);
                buffField = hal_hci_build_datafield(READ, IMU, Z_LREGADDR, 0);
                hal_hci_addDatafield(&TxPacket, buffField);

                /* Send Packets */
                if (xSemaphoreTake(SemaphoreUart, (TickType_t) 10) == pdTRUE) {
                    os_hci_setAccel(ALL_AXES);
                    os_hci_write(TxPacket);
                }
                break;
            
            case PEDO:
                /* Setup Pedo Packet */
                buffField = hal_hci_build_datafield(READ, IMU, 0x4B, 0);
                hal_hci_addDatafield(&TxPacket, buffField);
                buffField = hal_hci_build_datafield(READ, IMU, 0x4C, 0);
                hal_hci_addDatafield(&TxPacket, buffField);
                /* Send X Packet */
                if (xSemaphoreTake(SemaphoreUart, (TickType_t) 10) == pdTRUE) {
                    os_hci_write(TxPacket);
                }
                break;
        }
    } else if(device == LPS22HB) {

        buffField = hal_hci_build_datafield(READ, TEMP, PRESS_HREGADDR, 0);
        hal_hci_addDatafield(&TxPacket, buffField);
        buffField = hal_hci_build_datafield(READ, TEMP, PRESS_LREGADDR, 0);
        hal_hci_addDatafield(&TxPacket, buffField);
        buffField = hal_hci_build_datafield(READ, TEMP, PRESS_XLREGADDR, 0);
        hal_hci_addDatafield(&TxPacket, buffField);

        if (xSemaphoreTake(SemaphoreUart, (TickType_t) 10) == pdTRUE) {
            os_hci_write(TxPacket);
        }

    } else if(device == VL53L0X) {

        buffField = hal_hci_build_datafield(READ, TOF, 0x53, 0);
        hal_hci_addDatafield(&TxPacket, buffField);

        if (xSemaphoreTake(SemaphoreUart, (TickType_t) 10) == pdTRUE) {
            os_hci_write(TxPacket);
        }
    } else if(device == LIS3MDL) {

        buffField = hal_hci_build_datafield(READ, MAGNO, X_HREGADDR, 0);
        hal_hci_addDatafield(&TxPacket, buffField);
        buffField = hal_hci_build_datafield(READ, MAGNO, X_LREGADDR, 0);
        hal_hci_addDatafield(&TxPacket, buffField);

        /* Setup Y Packet */
        buffField = hal_hci_build_datafield(READ, MAGNO, Y_HREGADDR, 0);
        hal_hci_addDatafield(&TxPacket, buffField);
        buffField = hal_hci_build_datafield(READ, MAGNO, Y_LREGADDR, 0);
        hal_hci_addDatafield(&TxPacket, buffField);

        /* Setup Z Packet */
        buffField = hal_hci_build_datafield(READ, MAGNO, Z_HREGADDR, 0);
        hal_hci_addDatafield(&TxPacket, buffField);
        buffField = hal_hci_build_datafield(READ, MAGNO, Z_LREGADDR, 0);
        hal_hci_addDatafield(&TxPacket, buffField);

        if (xSemaphoreTake(SemaphoreUart, (TickType_t) 10) == pdTRUE) {
            os_hci_write(TxPacket);
        }
    }
}
/*----------------------------------------------------------------------------*/