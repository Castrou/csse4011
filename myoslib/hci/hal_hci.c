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
#include "tiny_printf.h"

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
#define		MAX_PKT_SIZE	70
#define		PREAMBLE		0xAA
#define		REQUEST			0x01
#define		RESPONSE		0x02

#define		DF_SIZE				4
#define     DF_PKT_START    	2
#define		SID_DF_POS			0
#define		I2C_DF_POS			1
#define		REGADDR_DF_POS		2
#define		REGVAL_DF_POS		3

#define		READ			'r'
#define 	WRITE			'w'

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
    }

}

/*-----------------------------------------------------------*/

void uart_serial_handler( char RxChar ) {

    uart_print(RxChar);
}

/*-----------------------------------------------------------*/

extern void uart_write( const char *payload, ... ) {

    char buffer[100];
    va_list argList;

    va_start(argList, payload);

    tiny_vsnprintf(buffer, 100, payload, argList);

	va_end(argList);

    pcBuffer = (char *) xUartBackend.fnClaimBuffer(uartOutput, &bufflen);
    pvMemcpy(pcBuffer, buffer, 100);
    xUartBackend.fnSendBuffer(uartOutput, pcBuffer, bufflen);

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
            vLedsToggle(LEDS_RED);
            break;
        case LIS3MDL:	// 3-axis magno
			i2caddr = 0x3C;
            vLedsToggle(LEDS_GREEN);
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
            vLedsToggle(LEDS_ALL);
            break;
    }

	/* Get Register Address */
	regaddr = strtol(regaddrString, NULL, 16);

	/* Adjust according to read or write command */
    switch(cmd) {
        case READ:
			i2caddr++;	// Read address is 1 more than write address
			retDatafield.regval = 0;	// Not writing anything to it
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

extern void hal_hci_addDatafield( Packet *destPacket, Datafield newField ) {

    destPacket->data[destPacket->dataCnt] = newField;
	destPacket->dataCnt += 1;
}

/*-----------------------------------------------------------*/

extern void hal_hci_send_packet( Packet packet ) {
	
	int dfLength = packet.dataCnt * DF_SIZE;
	int pktPos;
	char TxPacket[MAX_PKT_SIZE];
	char typeXlength = (packet.type << 4) | dfLength;

	/* Assemble the Packet */
	TxPacket[0] = PREAMBLE;
	TxPacket[1] = typeXlength;

	/* Assemble datafields */
	for (pktPos = 0; pktPos < packet.dataCnt; pktPos++) {
		TxPacket[DF_PKT_START+SID_DF_POS+DF_SIZE*pktPos] = packet.data[pktPos].sid;
		TxPacket[DF_PKT_START+I2C_DF_POS+DF_SIZE*pktPos] = packet.data[pktPos].i2caddr;
		TxPacket[DF_PKT_START+REGADDR_DF_POS+DF_SIZE*pktPos] = packet.data[pktPos].regaddr;
		TxPacket[DF_PKT_START+REGVAL_DF_POS+DF_SIZE*pktPos] = packet.data[pktPos].regval;
	}
	
	/* End Packet Assembly */
	// TxPacket[DF_PKT_START+DF_SIZE*pktPos] = '\0';

    /* Write packet to UART */
	pcBuffer = (char *) xUartBackend.fnClaimBuffer(uartOutput, &bufflen);
    pvMemcpy(pcBuffer, TxPacket, dfLength + DF_PKT_START);
    xUartBackend.fnSendBuffer(uartOutput, pcBuffer, bufflen);

}

/*-----------------------------------------------------------*/