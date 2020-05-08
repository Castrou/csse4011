/** 
 **************************************************************
 * @file myoslib/bt/cli_bt.c
 * @author Cameron Stroud - 44344968
 * @date 03052020
 * @brief Bluetooth Command Line file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */


/* Includes ***************************************************/
#include "string.h"
#include "math.h"

#include "board.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "FreeRTOS_CLI.h"

#include "rtc.h"
#include "tdf.h"
#include "tdf_auto.h"

#include "unified_comms_bluetooth.h"
#include "bluetooth.h"

#include "leds.h"
#include "log.h"
#include "os_log.h"

#include "os_bt.h"

#include "hci_packet.h"
#include "os_hci.h"
#include "lib_hci.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define		G_CONVERT	    16384.0
#define     HPA_CONVERT     12.005
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
xTdfTime_t xTime;

/* Private function prototypes -----------------------------------------------*/


/*----------------------------------------------------------------------------*/

extern void send_bt( int tdf_id, char sign ) {

	if (xSemaphoreTake(SemaphoreUart, (TickType_t) 2000) == pdTRUE) {
		if (sign == 'u') {
			bRtcGetTdfTime(&xTime);
			os_bt_send_unsigned(tdf_id, TDF_TIMESTAMP_RELATIVE_OFFSET_MS, xTime, uHCIdata);			
			xSemaphoreGive(SemaphoreUart);
		} else {
			bRtcGetTdfTime(&xTime);
			os_bt_send_signed(tdf_id, TDF_TIMESTAMP_RELATIVE_OFFSET_MS, xTime, HCIdata);			
			xSemaphoreGive(SemaphoreUart);
		}
	}
}

/*----------------------------------------------------------------------------*/

extern void lib_bt_tdf_lsm6dsl( void ) {

    lib_hci_request_accel(ALL_AXES);
}

/*----------------------------------------------------------------------------*/

extern void lib_bt_tdf_3dpose( void ) {

    /* Get Accelerometer info */
    lib_hci_request_accel(ALL_AXES);
    if (xSemaphoreTake(SemaphoreUart, (TickType_t) 1000) == pdTRUE) {
        int16_t xRaw = (HCIdata[0]<<8) | HCIdata[1];
        int16_t yRaw = (HCIdata[2]<<8) | HCIdata[3];
        int16_t zRaw = (HCIdata[4]<<8) | HCIdata[5];
        double xMag = (double)(xRaw/G_CONVERT);
        double yMag = (double)(yRaw/G_CONVERT);
        double zMag = (double)(zRaw/G_CONVERT);
        double x = pow(xMag, 2);
        double y = pow(yMag, 2);
        double z = pow(zMag, 2);

        /* Calculate Pitch/Roll */        
        int16_t roll = (int16_t)(100 * atan2(xMag, sqrt(y + z)) * 180 / M_PI);
        int16_t pitch = (int16_t)(100 * atan2(yMag, sqrt(x + z)) * 180 / M_PI);
        // int16_t yaw = (int16_t)(100 * atan2(zMag, sqrt(x + y)) * 180 / M_PI);

        os_log_print(LOG_DEBUG, "roll: %.2f pitch: %.2f: ", (double)(roll)/100, 
                        (double)(pitch)/100);

        /*Add to data array */
        os_hci_clearHCI();
        HCIdata[0] = (roll & 0xFF);
        HCIdata[1] = (roll >> 8);
        HCIdata[2] = (pitch & 0xFF);
        HCIdata[3] = (pitch >> 8);
        // HCIdata[4] = (yaw & 0xFF);
        // HCIdata[5] = (yaw >> 8);
        xSemaphoreGive(SemaphoreUart);
    }
 }

/*----------------------------------------------------------------------------*/

extern void lib_bt_tdf_range_mm( void ) {

    lib_bt_tdf_3dpose();
    if (xSemaphoreTake(SemaphoreUart, (TickType_t) 1000) == pdTRUE) {
        xSemaphoreGive(SemaphoreUart);
        // double roll = (double)((HCIdata[0] & 0xFF) | (HCIdata[1] << 8))/100;
        // double pitch = (double)((HCIdata[2] & 0xFF) | (HCIdata[3] << 8))/100;
    }

    // lib_hci_request_tof();
    // if(xSemaphoreTake(SemaphoreUart, (TickType_t) 1000) == pdTRUE) {
    //     xSemaphoreGive(SemaphoreUart);
    // }
}

/*----------------------------------------------------------------------------*/

extern void lib_bt_tdf_height_msl( void ) {

    lib_hci_request_baro();
    if (xSemaphoreTake(SemaphoreUart, (TickType_t) 1000) == pdTRUE) {
        uint32_t baro = (uHCIdata[0] << 16) | (uHCIdata[1] << 8) | uHCIdata[2];
        double pressure = (double)(baro)/4096;
        uint16_t altitude = pressure/HPA_CONVERT;

        uHCIdata[0] = (altitude & 0xFF);
        uHCIdata[1] = (altitude >> 8);

        xSemaphoreGive(SemaphoreUart);    
        // os_log_print(LOG_DEBUG, "Baro reading: %d Pressure: %.2f alt: %d", baro, pressure, altitude);
    }
}