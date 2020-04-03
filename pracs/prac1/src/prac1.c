#include "FreeRTOS.h"
#include "task.h"

#include "string.h"

#include "board.h"

#include "gpio.h"
#include "leds.h"
#include "log.h"
#include "rtc.h"
#include "watchdog.h"

#include "test_reporting.h"
#include "tiny_printf.h"

#include "device_nvm.h"

#include "unified_comms_serial.h"

// #include "s4434496_cli_util.h"



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
	eLogSetLogLevel(LOG_RESULT, LOG_INFO);
	eLogSetLogLevel(LOG_APPLICATION, LOG_INFO);
}

/*-----------------------------------------------------------*/

CommandLineArg process_input( char *input ) {

    CommandLineArg cli_input;
    cli_input.command = strtok(input, " ");

    /* Sort out arguments array */
    char *argBuffer = strtok(NULL, " ");
    for (int i = 0; argBuffer != NULL ; i++) {

        cli_input.arguments[i] = argBuffer[0];
        argBuffer = strtok(NULL, " ");
        
    }

    /* Status 1 for no arguments given */
    if (cli_input.arguments == NULL) {
        pvMemcpy(cli_input.arguments, "status1", MAX_ARG_CNT);
    }
    return cli_input;

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

    // Initiate startup sequence
    vLedsSet(LEDS_RED);

    // Startup routine
    // /* Get Reboot reason */
	// pxRebootData = xWatchdogRebootReason();
	// if (pxRebootData != NULL)
	// {
	// 	vWatchdogPrintRebootReason(LOG_APPLICATION, LOG_INFO, pxRebootData);
	// }

	/* Setup our serial receive handler */
	xSerialComms.fnReceiveHandler = vCustomSerialHandler;
	vUnifiedCommsListen(&xSerialComms, COMMS_LISTEN_ON_FOREVER);

    // Clear for takeoff
    set_led_state(LEDS_NONE, CYCLE_OFF);

}

/*-----------------------------------------------------------*/

void vApplicationTickCallback( uint32_t ulUptime ) {

    eLEDs_t led = get_led_state().led;
    CycleState led_cycle = get_led_state().cycle;
    const char *new_led_colour;

    /* Flush comms? */
    tdf_uptime_t xUptime = {ulUptime};
	eTdfAddMulti(BLE_LOG, TDF_UPTIME, TDF_TIMESTAMP_NONE, NULL, &xUptime);
	eTdfFlushMulti(BLE_LOG);

    /* Change LED colour */
    if (led_cycle == CYCLE_ON) {
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
        eLog(LOG_APPLICATION, LOG_INFO, "Switching LED colour to: %s\r\n", new_led_colour);
    }
    
}

/*-----------------------------------------------------------*/

void led_command(char *arguments) {

    /* Get current status */
    eLEDs_t currLED = get_led_state().led;
    int currCycle = get_led_state().cycle;

    /* Check arguments */
    switch(arguments[0]) {
        case 'o':
            switch(arguments[1]) {
                case 'r':
                    vLedsOn(LEDS_RED);
                    break;
                case 'g':
                    vLedsOn(LEDS_GREEN);
                    break;
                case 'b':
                    vLedsOn(LEDS_BLUE);
                    break;
                case 'c':
                    set_led_state(currLED, CYCLE_ON);
                    break;
                default:
                    // Error print
                    break;
            }
        case 'f':
            switch(arguments[1]){
                case 'r':
                    vLedsOff(LEDS_RED);
                    break;
                case 'g':
                    vLedsOff(LEDS_GREEN);
                    break;
                case 'b':
                    vLedsOff(LEDS_BLUE);
                    break;
                case 'c':
                    set_led_state(LEDS_NONE, CYCLE_OFF);
                    break;
                default:
                    // Error print
                    break;
            }
        case 't':
            switch(arguments[1]){
                case 'r':
                    vLedsToggle(LEDS_RED);
                    break;
                case 'g':
                    vLedsToggle(LEDS_GREEN);
                    break;
                case 'b':
                    vLedsToggle(LEDS_BLUE);
                    break;
                case 'c':
                    set_led_state(currLED, !(currCycle));
                default:
                    // Error print
                    break;
            }
    }
}

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

void vCustomSerialHandler(xCommsInterface_t *pxComms,
						  xUnifiedCommsIncomingRoute_t *pxCurrentRoute,
						  xUnifiedCommsMessage_t *pxMessage)
{
	char pcLocalString[60] = {0};
    int status = 0;
    CommandLineArg cli_input;
    pvMemcpy(cli_input.arguments, NULL, MAX_ARG_CNT);
	UNUSED(pxCurrentRoute);
	UNUSED(pxComms);
	/* 
	 * Copy the string to a local buffer so it can be NULL terminated properly
	 * The %s format specifier does not respect provided lengths
	 */
	pvMemcpy(pcLocalString, pxMessage->pucPayload, pxMessage->usPayloadLen);

    // Find Command
    cli_input = process_input(pcLocalString);

    // Check what command (if any)
	if (!(strcmp(cli_input.command, "led"))) {
        // LED command
        if (!status) {  // Only run if arguments are given
            led_command(cli_input.arguments);
        }
    } else if (!(strcmp(cli_input.command, "time"))) {
        // Time command
    } else {
        // Not a command
    }
}