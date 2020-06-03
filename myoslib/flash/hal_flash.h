/** 
 **************************************************************
 * @file myoslib/flash/hal_flash.h
 * @author Cameron Stroud - 44344968
 * @date 18052020
 * @brief Flash Driver Header
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */

#ifndef S4434496_HAL_FLASH_H
#define S4434496_HAL_FLASH_H

/* Global Defines ------------------------------------------------------------*/
#define FLASH_SPI_INSTANCE  1							// SPIM0 Instance
#define FLASH_MISO_GPIO		(xGpio_t){.ucPin = NRF_GPIO_PIN_MAP( 0, 21 )}	// SPI0 MISO Pin
#define FLASH_MOSI_GPIO		(xGpio_t){.ucPin = NRF_GPIO_PIN_MAP( 0, 20 )}	// SPI0 MOSI Pin
#define FLASH_SCK_GPIO		(xGpio_t){.ucPin = NRF_GPIO_PIN_MAP( 0, 19 )}	// SPI0 SCK Pin
#define FLASH_CS_GPIO		(xGpio_t){.ucPin = NRF_GPIO_PIN_MAP( 0, 17 )}	// SPI0 SS Pin (Placeholder: Each device should have its own SS pin).
#define FLASH_WP_GPIO		(xGpio_t){.ucPin = NRF_GPIO_PIN_MAP( 0, 22 )}	// SPI0 SS Pin (Placeholder: Each device should have its own SS pin).

/* Function prototypes -------------------------------------------------------*/
extern void hal_flash_init( void );
extern void hal_flash_write(uint32_t ulBlockNum, void *pvBlockData, uint32_t ulBlockSize);
extern void hal_flash_read( uint32_t ulBlockNum, uint16_t usOffset, void *pvBlockData, uint32_t ulBlockSize );
#endif // S4434496_HAL_FLASH_H