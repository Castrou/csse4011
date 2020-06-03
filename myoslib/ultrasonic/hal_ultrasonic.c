/** 
 **************************************************************
 * @file myoslib/ultrasonic/hal_ultrasonic.c
 * @author Cameron Stroud - 44344968
 * @date 18052020
 * @brief Ultrasonic Driver file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */

/* Includes ***************************************************/
#include "board.h"
#include "tiny_printf.h"

#include "gpio.h"
#include "leds.h"
#include "log.h"

#include "nrf_delay.h"

#include "os_log.h"

#include "hal_ultrasonic.h"
#include "os_ultrasonic.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define     US_CONVERT  0.34 / 2 // Speed of sound / 2 (because it travels there and back)
#define     TIMEOUT     1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/*-----------------------------------------------------------*/

/**
* @brief  Intialise ultrasonic
* @param  None
* @retval None
*/
extern void hal_ultrasonic_init( void ) {

    vGpioSetup( TRIG_PIN, GPIO_PUSHPULL, GPIO_PUSHPULL_LOW); 
    vGpioSetup( ECHO_PIN, GPIO_INPUTPULL, GPIO_INPUTPULL_PULLDOWN); 

}

/*-----------------------------------------------------------*/

/**
* @brief  Ping ultrasonic and get distance
* @param  None
* @retval None
*/
extern double hal_ultrasonic_ping( void ) {

    int16_t pulseTime = 0;

    vGpioClear(TRIG_PIN);
    nrf_delay_us(2);
    
    vGpioSet(TRIG_PIN); // start pulse
    nrf_delay_us(10);            // us delay
    vGpioClear(TRIG_PIN); // end pulse

    while (!bGpioRead(ECHO_PIN) && pulseTime++ < TIMEOUT); // pulse hasnt come back yet
    pulseTime = 0;
    while (bGpioRead(ECHO_PIN)) {
        pulseTime++;
        nrf_delay_us(1);
        if(pulseTime > TIMEOUT) { 
            return DISCONNECTED;
        }
    }

    /* convert time to dist */
    return ((double)pulseTime * US_CONVERT); 

}