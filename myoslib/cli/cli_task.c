/**
************************************************************** 
* @file    myoslib/cli/cli_task.c
* @author  Cameron Stroud - 44344968
* @date    30032020
* @brief   Command Line Interface Driver
*
**************************************************************
*      EXTERNAL FUNCTIONS
************************************************************** 
*  
************************************************************** 
*/

/* Includes -------------------------------------------------*/

#include "FreeRTOS.h"
#include "task.h"

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

/* Private Defines ------------------------------------------*/

/* Type Definitions -----------------------------------------*/

/* Function Declarations ------------------------------------*/