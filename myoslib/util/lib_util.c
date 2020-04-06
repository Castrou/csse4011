/** 
 **************************************************************
 * @file myoslib/util/lib_util.c
 * @author Cameron Stroud - 44344968
 * @date 05042020
 * @brief Utility Functions
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */


/* Includes ***************************************************/
#include "leds.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**
* @brief  Sets a variable to the colour specified in input
* @param  ledVar: LED Colour
* @param  argInput: Argument provided
* @retval eLEDs_t: 
*/
extern int set_led_colour( eLEDs_t *ledVar, const char **ledName, const char *argInput ) {

	switch(argInput[0]) {
		case 'r':
			*ledVar = LEDS_RED;
			*ledName = "RED";
			break;
		case 'g':
			*ledVar = LEDS_GREEN;
			*ledName = "GREEN";
			break;
		case 'b':
			*ledVar = LEDS_BLUE;
			*ledName = "BLUE";
			break;
		case 'c':
			break;
		default: 
			return 1; // Invalid arguments
    }

	return 0;
}