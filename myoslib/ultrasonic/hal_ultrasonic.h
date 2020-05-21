/** 
 **************************************************************
 * @file myoslib/hci/hal_ultrasonic.h
 * @author Cameron Stroud - 44344968
 * @date 18052020
 * @brief Ultrasonic Driver Header
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */

#ifndef S4434496_HAL_US_H
#define S4434496_HAL_US_H

/* Global Defines ------------------------------------------------------------*/
#define     TRIG_PIN       (xGpio_t){.ucPin = NRF_GPIO_PIN_MAP( 1, 1 )}
#define     ECHO_PIN       (xGpio_t){.ucPin = NRF_GPIO_PIN_MAP( 1, 2 )}

/* Function prototypes -------------------------------------------------------*/
extern void hal_ultrasonic_init( void );
extern double hal_ultrasonic_ping( void );

#endif // S4434496_HAL_US_H