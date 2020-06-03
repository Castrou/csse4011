/** 
 **************************************************************
 * @file myoslib/flash/os_flash.h
 * @author Cameron Stroud - 44344968
 * @date 20052020
 * @brief Flash OS header file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */

#ifndef S4434496_OS_FLASH_H
#define S4434496_OS_FLASH_H

/* Global Variables ----------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
extern void os_flash_init( void );
extern void os_flash_deinit( void );
extern void os_flash_write(const char *payload, ... );
extern void os_flash_read( void *dataBuffer, uint32_t blockSize );


#endif // S4434496_OS_FLASH_H