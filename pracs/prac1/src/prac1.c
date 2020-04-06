/* Includes ***************************************************/
#include "string.h"

#include "board.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "FreeRTOS_CLI.h"

#include "gpio.h"
#include "leds.h"
#include "log.h"
#include "rtc.h"
#include "watchdog.h"

#include "test_reporting.h"
#include "tiny_printf.h"

#include "device_nvm.h"

#include "unified_comms_serial.h"

#include "cli_task.h"
#include "cli_util.h"
#include "os_log.h"
#include "lib_log.h"

/* Private Defines ------------------------------------------*/
#define MAX_ARG_CNT     8
/* Type Definitions -----------------------------------------*/
typedef enum CycleState {
    CYCLE_ON,
    CYCLE_OFF
} CycleState;

typedef struct CommandLineArg {
    char *command;  // Self explanatory
    char arguments[MAX_ARG_CNT]; // Array of single character arguments
} CommandLineArg;

struct LedState {
    CycleState cycle;
    eLEDs_t led;
};
/* Function Declarations ------------------------------------*/
struct LedState get_led_state();
void set_led_state(eLEDs_t colour, CycleState cycleFlag);
void vCustomSerialHandler(xCommsInterface_t *pxComms,
						  xUnifiedCommsIncomingRoute_t *pxCurrentRoute,
						  xUnifiedCommsMessage_t *pxMessage);

/* Private Variables ----------------------------------------*/
struct LedState led_state;

/*-----------------------------------------------------------*/

void vApplicationSetLogLevels(void)
{
	eLogSetLogLevel(LOG_RESULT, LOG_DEBUG);
	eLogSetLogLevel(LOG_APPLICATION, LOG_DEBUG);
}

/*-----------------------------------------------------------*/

struct LedState get_led_state( void ) {

    return led_state;

}

/*-----------------------------------------------------------*/

void set_led_state( eLEDs_t LED, CycleState cycleFlag ) {

    vLedsSet(LED);
    led_state.led = LED;
    led_state.cycle = cycleFlag;

}

/*-----------------------------------------------------------*/

void vApplicationStartupCallback( void ) {

    WatchdogReboot_t *  pxRebootData;
    
    /* Initiate startup sequence */
    vLedsSet(LEDS_ALL);

    /* Get Reboot reason */
	pxRebootData = xWatchdogRebootReason();
	if (pxRebootData != NULL)
	{
		vWatchdogPrintRebootReason(LOG_APPLICATION, LOG_INFO, pxRebootData);
	}

	/* Setup our serial receive handler */
	xSerialComms.fnReceiveHandler = vCustomSerialHandler;
	vUnifiedCommsListen(&xSerialComms, COMMS_LISTEN_ON_FOREVER);

    /* Init functions */
    os_log_init();
    cli_util_init();
    cli_task_init();

    /* Clear for takeoff */
    set_led_state(LEDS_NONE, CYCLE_ON);

}

/*-----------------------------------------------------------*/

void vApplicationTickCallback( uint32_t ulUptime ) {

    eLEDs_t led = get_led_state().led;
    CycleState led_cycle = get_led_state().cycle;
    const char *new_led_colour;

    UNUSED(ulUptime);
    /* Flush comms? */
    tdf_uptime_t xUptime = {ulUptime};
	eTdfAddMulti(BLE_LOG, TDF_UPTIME, TDF_TIMESTAMP_NONE, NULL, &xUptime);
	eTdfFlushMulti(BLE_LOG);

    /* Change LED colour */
    switch(led) {
        case LEDS_RED:      // if RED go to GREEN
            new_led_colour = "GREEN";
            set_led_state(LEDS_GREEN, CYCLE_ON);
            break;
        case LEDS_GREEN:    // if GREEN go to BLUE
            new_led_colour = "BLUE";
            set_led_state(LEDS_BLUE, CYCLE_ON);
            break;
        case LEDS_BLUE:     // if BLUE go to RED
            new_led_colour = "RED";
            set_led_state(LEDS_RED, CYCLE_ON);
            break;
        default:
            new_led_colour = "RED";
            set_led_state(LEDS_RED, CYCLE_ON);
            break;
    }

    UNUSED(new_led_colour);
    UNUSED(led_cycle);
}

/*-----------------------------------------------------------*/

void vCustomSerialHandler(xCommsInterface_t *pxComms,
						  xUnifiedCommsIncomingRoute_t *pxCurrentRoute,
						  xUnifiedCommsMessage_t *pxMessage)
{
	char pcLocalString[100] = {0};
    
	UNUSED(pxCurrentRoute);
	UNUSED(pxComms);
	/* 
	 * Copy the string to a local buffer so it can be NULL terminated properly
	 * The %s format specifier does not respect provided lengths
	 */
	pvMemcpy(pcLocalString, pxMessage->pucPayload, pxMessage->usPayloadLen);
    
    cli_task_queue_serial(pcLocalString);
}