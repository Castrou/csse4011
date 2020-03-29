#include "FreeRTOS.h"
#include "task.h"

#include "board.h"

#include "gpio.h"
#include "leds.h"



/* Private Defines ------------------------------------------*/
/* Type Definitions -----------------------------------------*/

/* Function Declarations ------------------------------------*/
eLEDs_t get_led_state();
void set_led_state(eLEDs_t colour);

/* Private Variables ----------------------------------------*/
eLEDs_t led_state;

/*-----------------------------------------------------------*/

eLEDs_t get_led_state( void ) {

    return led_state;

}

/*-----------------------------------------------------------*/

void set_led_state( eLEDs_t LED ) {

    vLedsSet(LED);
    led_state = LED;

}

/*-----------------------------------------------------------*/

void vApplicationTickCallback( uint32_t ulUptime ) {

    eLEDs_t led = get_led_state();

    UNUSED(ulUptime);

    /* Change LED colour */
    switch(led) {

        case LEDS_RED:      // if RED go to GREEN
            set_led_state(LEDS_GREEN);
            break;
        case LEDS_GREEN:    // if GREEN go to BLUE
            set_led_state(LEDS_BLUE);
            break;
        case LEDS_BLUE:     // if BLUE go to RED
            set_led_state(LEDS_RED);
            break;
        default:
            set_led_state(LEDS_RED);
            break;
    }
}