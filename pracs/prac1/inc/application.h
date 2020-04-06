/*
 * Copyright (c) 2020, Commonwealth Scientific and Industrial Research 
 * Organisation (CSIRO) ABN 41 687 119 230.
 *
 * Filename: application.h
 * Creation_Date: 04/08/2019
 * Author: Jordan Yates <jordan.yates@data61.csiro.au>
 *
 * Application specific configuration
 * 
 */
#ifndef __CSIRO_CORE_APPLICATION
#define __CSIRO_CORE_APPLICATION
/* Includes -------------------------------------------------*/

/* Module Defines -------------------------------------------*/
// clang-format off

#define APP_MAJOR                               100
#define APP_MINOR                               1

#define NVM_VALID_KEY                           0x12349876

/* The size of the global output buffer that is available for use when there
are multiple command interpreters running at once (for example, one on a UART
and one on TCP/IP).  This is done to prevent an output buffer being defined by
each implementation - which would waste RAM.  In this case, there is only one
command interpreter running, and it has its own local output buffer, so the
global buffer is just set to be one byte long as it is not used and should not
take up unnecessary RAM. */
#define configCOMMAND_INT_MAX_OUTPUT_SIZE 1

// clang-format on
/* Type Definitions -----------------------------------------*/

#endif /* __CSIRO_CORE_APPLICATION */
