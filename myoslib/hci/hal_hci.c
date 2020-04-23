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
#include "os_hci.h"
#include "hci_packet.h"
#include "os_log.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum {
	LSM6DSL = 1,    // IMU
	LIS3MDL = 2,    // 3-axis magnetometer
	LPS22HB = 3,    // MEMS nano pressure sensor
	VL53L0X = 4,    // ToF and Gesture Detection sensor
	HTS221 = 5      // Temp & Humidity Sensor
} SID_t;		// Sensor IDs

typedef enum {
    UART_IDLE,		// Idle state
    UART_TL_CHECK,	// Type/Length checking state
    UART_RESPONSE,	// Packet-is-a-response state
    UART_REQUEST	// Packet-is-a-request state
} uartState_t;	// UART Reading states

typedef enum {
    PKT_SID,		// Sensor ID state
    PKT_I2CADDR,	// I2C Address state
    PKT_REGADDR,	// Register Address state
    PKT_REGVAL		// Register Value state
} packetState_t; // Packet reading states

/* Private define ------------------------------------------------------------*/
#define		MAX_PKT_SIZE	70			// Maximum packet size
#define		PREAMBLE		0xAA		// Preamble
#define		REQUEST			0x01		// Packet type: Request
#define		RESPONSE		0x02		// Packet type: Response

#define		DF_SIZE				4		// Number of bytes in a datafield
#define     DF_PKT_START    	2		// Packet position of first datafield byte
#define		SID_DF_POS			0		// Datafield position of SID
#define		I2C_DF_POS			1		// Datafield position of I2C Address
#define		REGADDR_DF_POS		2		// Datafield position of Register Address
#define		REGVAL_DF_POS		3		// Datafield position of Register Value

#define		READ			'r'			// Read command
#define 	WRITE			'w'			// Write command

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
UART_MODULE_CREATE( SERIAL_OUTPUT1, NRF_UARTE1, UARTE1_IRQHandler, UNUSED_IRQ, 4, 128, 64 );
xUartModule_t * uartOutput = &UART_MODULE_GET( SERIAL_OUTPUT1 );

uartState_t uartState;			// UART state for state machine
packetState_t packetState;		// Packet state for state machine

Packet recvPacket;				// Received packet
uint8_t recvLength;				// Length of a received packet
uint8_t recvDFPos;				// Number of datafields of a received packet

uint32_t bufflen;
uint32_t buffPos;
char *pcBuffer;
char printBuff[80];

/* Private function prototypes -----------------------------------------------*/

/*-----------------------------------------------------------*/

/**
* @brief  Initialise print buffer
* @param  None
* @retval None
*/
void init_printBuff ( void ) {

    buffPos = 0;
    for (int i = 0; printBuff[i] != 0; i++) {
        printBuff[i] = 0;
    }
}

/*-----------------------------------------------------------*/

/**
* @brief  Add characters to a buffer to print horiz. instead of vert.
* @param  c: Character to be added to print buffer
* @retval None
*/
void uart_print( char c ) {

    if (c == '\r' || c == '\n' || c == '\0') { 
        /* Print completed string */
        printBuff[buffPos] = '\0';
        os_log_print(LOG_INFO, printBuff);
        init_printBuff();
    } else {    
        /* Build string */
        printBuff[buffPos] = c;
        buffPos++;
    }

}

/*-----------------------------------------------------------*/

/**
* @brief  FSM Processing for reading a packet
* @param  c: Incoming UART byte
* @retval None
*/
void packet_process( uint8_t c ) {

    switch(packetState) {
        case PKT_SID:
            recvPacket.data[recvPacket.dataCnt-recvDFPos].sid = c;
            packetState = PKT_I2CADDR;
            break;
        case PKT_I2CADDR:
            recvPacket.data[recvPacket.dataCnt-recvDFPos].i2caddr = c;
            packetState = PKT_REGADDR;
            break;
        case PKT_REGADDR:
            recvPacket.data[recvPacket.dataCnt-recvDFPos].regaddr = c;
            packetState = PKT_REGVAL;
            break;
        case PKT_REGVAL:
            recvPacket.data[recvPacket.dataCnt-recvDFPos].regval = c;
            recvDFPos--;
            packetState = PKT_SID;
            break;
    }

}

/*-----------------------------------------------------------*/

/**
* @brief  FSM Processing for reading from UART
* @param  c: Incoming UART byte
* @retval None
*/
void uart_fsmprocessing( char c ) {
    
    int type;

    switch(uartState) {
        case UART_IDLE:
            if (c == PREAMBLE) { // Check Preamble
                uartState = UART_TL_CHECK;
            }
            break;

        case UART_TL_CHECK:
            type = (c >> 4);	// Bit shift to get type
            recvPacket.type = type;
            recvLength = (c & 0x0F)-1;	// Mask to get length, -1 for 0 indexing
            recvDFPos = (recvLength+1)/DF_SIZE; // Number of datafields with 1 indexing
            recvPacket.dataCnt = recvDFPos;

            if (type == REQUEST) {
                packetState = PKT_SID;
                uartState = UART_REQUEST;
            } else if (type == RESPONSE) {
                uartState = UART_RESPONSE;
            } else {
                uartState = UART_IDLE;
            }
            break;

        case UART_RESPONSE:
            if(recvLength) {
				/* While receiving from UART */
                recvLength--;
                packet_process(c);
            } else {
				/* Last receive, send off for printing */
                packet_process(c);
				os_hci_read(recvPacket);
                packetState = PKT_SID;
                uartState = UART_IDLE;
            }
            break;

        case UART_REQUEST:
			/* Not expecting requests from SCU */
            uartState = UART_IDLE;
            break;
    }
}

/*-----------------------------------------------------------*/

/**
* @brief  UART Serial Handler
* @param  RxChar: Character received over UART
* @retval None
*/
void uart_serial_handler( char RxChar ) {

    uart_fsmprocessing(RxChar);
}

/*-----------------------------------------------------------*/

/**
* @brief  Function for writing to UART
* @param  None
* @retval None
*/
extern void hal_hci_uart_write( const char *payload, ... ) {

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

/**
* @brief  Initialisation function for HCI
* @param  None
* @retval None
*/
extern void hal_hci_init( void ) {

    /* Init variables */
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

/**
* @brief  Function for building datafields
* @param  cmd: Command entered ('r' for read; 'w' for write)
* @param  sid: Sensor ID
* @param  regaddr: Register Address to be read from or written to
* @param  regval: Value to be written to register
* @retval None
*/
extern Datafield hal_hci_build_datafield( char cmd, uint8_t sid, 
                                            uint8_t regaddr, uint8_t regval) {

    Datafield retDatafield;
    uint8_t i2caddr;

	/* Find I2C Address */
    switch((SID_t)sid) {    // Default the values to write address for now
        case LSM6DSL:	    // IMU
			i2caddr = 0xD4;
            break;
        case LIS3MDL:	    // 3-axis magno
			i2caddr = 0x3C;
            break;
        case LPS22HB:	    // Pressure sensor
			i2caddr = 0xBA;
            break;
        case VL53L0X:       // ToF & Gesture
			i2caddr = 0x52;
            break;
        case HTS221:        // Temp/Humidity sensor
			i2caddr = 0xBE;
            break;
        default:
            break;
    }

	/* Adjust according to read or write command */
    switch(cmd) {
        case READ:
			i2caddr++;	// Read address is 1 more than write address
			retDatafield.regval = 0;	// Not writing anything to it
            break;
        case WRITE:
			retDatafield.regval = regval;
            break;
    }

    retDatafield.sid = sid;
	retDatafield.i2caddr = i2caddr;
	retDatafield.regaddr = regaddr;

	return retDatafield;
}

/*-----------------------------------------------------------*/

/**
* @brief  Function for adding datafields to a given packet
* @param  destPacket: Packet for datafield to be added to
* @param  newField: Datafield being added
* @retval None
*/
extern void hal_hci_addDatafield( Packet *destPacket, Datafield newField ) {
    destPacket->data[destPacket->dataCnt] = newField;
	destPacket->dataCnt += 1;
}

/*-----------------------------------------------------------*/

/**
* @brief  Send packet over UART
* @param  packet: Packet to be sent over UART
* @retval None
*/
extern void hal_hci_send_packet( Packet packet ) {
	
	int dfLength = packet.dataCnt*DF_SIZE;
	int pktPos;
	char TxPacket[MAX_PKT_SIZE];
	char typeXlength;
    
    if(packet.type != 1 && packet.type != 2) { // Type not set, default to request
        packet.type = 1;
    }

    typeXlength = (packet.type << 4) | dfLength;    

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

    /* Write packet to UART */
	pcBuffer = (char *) xUartBackend.fnClaimBuffer(uartOutput, &bufflen);
    pvMemcpy(pcBuffer, TxPacket, dfLength + DF_PKT_START);
    xUartBackend.fnSendBuffer(uartOutput, pcBuffer, bufflen);

}

/*-----------------------------------------------------------*/