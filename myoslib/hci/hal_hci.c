/** 
 **************************************************************
 * @file myoslib/hci/hal_hci.c
 * @author Cameron Stroud - 44344968
 * @date 20042020
 * @brief HCI Driver file
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

#include "uart.h"
#include "unified_comms_serial.h"
#include "argon.h"

#include "gpio.h"
#include "leds.h"
#include "log.h"

#include "hal_hci.h"
#include "hci_packet.h"
#include "os_log.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum {
    LSM6DSL = 1,    // IMU
    LIS3MDL = 2,    // 3-axis magnetometer
    LPS22HB = 3,    // MEMS nano pressure sensor
    VL53L0X = 4,    // ToF and Gesture Detection sensor
    HTS221 = 5      // Temp & Humidity Sensor
} SID_t;

/* Private define ------------------------------------------------------------*/
#define		READ		'r'
#define 	WRITE		'w'
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
UART_MODULE_CREATE( SERIAL_OUTPUT1, NRF_UARTE1, UARTE1_IRQHandler, UNUSED_IRQ, 4, 128, 64 );
xUartModule_t * uartOutput = &UART_MODULE_GET( SERIAL_OUTPUT1 );

Packet HCIPacket;
uint8_t numDatafield;

uint32_t bufflen;
uint32_t buffPos;
char *pcBuffer;
char printBuff[80];

/* Private function prototypes -----------------------------------------------*/

/*-----------------------------------------------------------*/

void init_printBuff ( void ) {

    buffPos = 0;
    for (int i = 0; printBuff[i] != 0; i++) {
        printBuff[i] = 0;
    }
}

/*-----------------------------------------------------------*/

void uart_print( char c ) {
    if (c == '\r' || c == '\n' || c == '\0') {
        printBuff[buffPos] = '\0';
        os_log_print(LOG_INFO, printBuff);
        init_printBuff();
    } else {
        printBuff[buffPos] = c;
        buffPos++;
        // os_log_print(LOG_DEBUG, "%c", c);
    }

}

/*-----------------------------------------------------------*/

void uart_serial_handler( char RxChar ) {
    uart_print(RxChar);
}

/*-----------------------------------------------------------*/

extern void hal_hci_init() {

    /* Init variables */
    numDatafield = 0;
    init_printBuff();

    /* Setup UART */
    uartOutput->xPlatform.pxTimer = NRF_TIMER1; 
    uartOutput->ulBaud = 115200; 
    uartOutput->xPlatform.xRx = UART1_RX_PIN; 
    uartOutput->xPlatform.xTx = UART1_TX_PIN; 
    uartOutput->xPlatform.xRts = UART1_RTS_PIN;
    uartOutput->xPlatform.xCts = UART1_CTS_PIN; 
    eUartInit( uartOutput, true );

    static xSerialReceiveArgs_t xArgs;
    xArgs.pxUart    = uartOutput;    
    xArgs.fnHandler = (fnSerialByteHandler_t) uart_serial_handler;    
    configASSERT( xTaskCreate( vSerialReceiveTask, "Ser Recv", configMINIMAL_STACK_SIZE, &xArgs, tskIDLE_PRIORITY + 1, NULL ) == pdPASS );

    vUartOn(uartOutput);
}

/*-----------------------------------------------------------*/

// extern void hal_hci_writetest() {

//     pcBuffer = (char *) xUartBackend.fnClaimBuffer(uartOutput, &bufflen);
//     pvMemcpy(pcBuffer, "This is a string", 17);
//     xUartBackend.fnSendBuffer(uartOutput, pcBuffer, bufflen);

// }

/*-----------------------------------------------------------*/

// void build_packet(Packet packet) {


// }

/*-----------------------------------------------------------*/

extern Datafield hal_hci_build_datafield( char *command, char *sidString, char *regaddrString, char *regvalString) {

    Datafield retDatafield;
	SID_t sid;
    uint8_t i2caddr;
	uint8_t regaddr;
	uint8_t regval;
	char cmd;

	/* Get command */
	cmd = command[0];

	/* Get Sensor ID */
	sid = (SID_t)strtol(sidString, NULL, 10);

	/* Find I2C Address */
    switch(sid) {		// Default the values to write address for now
        case LSM6DSL:	// IMU
			i2caddr = 0xD4;
            break;
        case LIS3MDL:	// 3-axis magno
			i2caddr = 0x3C;
            break;
        case LPS22HB:	// Pressure sensor
			i2caddr = 0xBA;
            break;
        case VL53L0X:   // ToF & Gesture
			i2caddr = 0x52;
            break;
        case HTS221:    // Temp/Humidity sensor
			i2caddr = 0xBE;
            break;
        default:
            break;
    }

	/* Get Register Address */
	regaddr = strtol(regaddrString, NULL, 10);

	/* Adjust according to read or write command */
    switch(cmd) {
        case READ:
			i2caddr++;	// Read address is 1 more than write address
            break;
        case WRITE:
            regval = strtol(regvalString, NULL, 10);
			retDatafield.regval = regval;
            break;
    }

    retDatafield.sid = sid;
	retDatafield.i2caddr = i2caddr;
	retDatafield.regaddr = regaddr;

	return retDatafield;
}

/*-----------------------------------------------------------*/

extern void hal_hci_addDatafield( Datafield newField ) {

    HCIPacket.data[numDatafield] = newField;
}

/*-----------------------------------------------------------*/

extern void hal_hci_send_packet( Packet packet ) {

	int totalLen = 0;

    for (int i = 0; i < numDatafield; i++) {
        totalLen += packet.data[i].length;
    }

}

/*-----------------------------------------------------------*/