#include "board.h"
#include "log.h"
#include "tiny_printf.h"

uint32_t bufflen;

void os_serial_write(uint8_t *payload) {
    /* Write packet to UART */
	pcBuffer = (char *) xSerialBackend.fnClaimBuffer(pxSerialOutput, &bufflen);
    pvMemcpy(pcBuffer, payload, sizeof(payload));
    xUartBackend.fnSendBuffer(pxSerialOutput, pcBuffer, bufflen);
}