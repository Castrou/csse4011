/** 
 **************************************************************
 * @file myoslib/flash/hal_flash.c
 * @author Cameron Stroud - 44344968
 * @date 18052020
 * @brief Flash Driver file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */

/* Includes ***************************************************/
#include "board.h"
#include "tiny_printf.h"

#include "spi.h"
#include "flash_interface.h"
#include "mx25r.h"
#include "onboard_logger.h"

#include "gpio.h"
#include "leds.h"
#include "log.h"

#include "os_log.h"

#include "hal_flash.h"

/* Private typedef -----------------------------------------------------------*/
SPI_MODULE_CREATE( FLASH_SPI, FLASH_SPI_INSTANCE, SPIM1_TWIM1_IRQHandler );

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
xSpiModule_t *	 pxFlashSpi   = &SPI_MODULE_GET( FLASH_SPI );

/* Flash Memory */
xMX25rHardware_t xMX25rHardware = {
	.pxInterface = &SPI_MODULE_GET( FLASH_SPI ),
	.xSpiConfig  = { .xCsGpio = FLASH_CS_GPIO, 0, 0, 0, 0 }
};
xFlashDevice_t xMX25rDevice = {
	.xSettings		  = { 0 },
	.pxImplementation = &xMX25rDriver,
	.xCommandQueue	  = NULL,
	.pcName			  = "MX25R",
	.pxHardware		  = (xFlashDefaultHardware_t *) &xMX25rHardware
};

xFlashDevice_t *const  pxOnboardFlash = &xMX25rDevice;

// TDF_LOGGER_STRUCTURES( ONBOARD_STORAGE_LOG, xFlashLog, "FlashLog", (xLoggerDevice_t *) &xOnboardLoggerDevice, 256, 0, LOGGER_LENGTH_REMAINING_BLOCKS );
// eTdfLoggerConfigure( &xFlashLog, LOGGER_CONFIG_INIT_DEVICE, NULL );
// eTdfLoggerConfigure( &xFlashLog, LOGGER_CONFIG_COMMIT_ONLY_USED_BYTES, 0 );
// eTdfLoggerConfigure( &xFlashLog, LOGGER_CONFIG_APPEND_MODE, 0 );

/* Private function prototypes -----------------------------------------------*/

/*-----------------------------------------------------------*/

/**
* @brief  brief
* @param  None
* @retval None
*/
extern void hal_flash_init( void ) {

	eModuleError_t eResult; 

    /* Initialise Flash SPI Pins */
	vGpioSetup( FLASH_CS_GPIO, GPIO_PUSHPULL, GPIO_PUSHPULL_HIGH );
	vGpioSetup( FLASH_MISO_GPIO, GPIO_PUSHPULL, GPIO_PUSHPULL_HIGH );
	vGpioSetup( FLASH_MOSI_GPIO, GPIO_PUSHPULL, GPIO_PUSHPULL_HIGH );
	vGpioSetup( FLASH_SCK_GPIO, GPIO_PUSHPULL, GPIO_PUSHPULL_HIGH );
	vGpioSetup( FLASH_WP_GPIO, GPIO_PUSHPULL, GPIO_PUSHPULL_HIGH);

	/* Setup the MX25 external flash interface channel */
	pxFlashSpi->xPlatform.xMosi = FLASH_MOSI_GPIO;
	pxFlashSpi->xPlatform.xMiso = FLASH_MISO_GPIO;
	pxFlashSpi->xPlatform.xSclk = FLASH_SCK_GPIO;
	
	/* Initialise SPI */
	eResult = eSpiInit(pxFlashSpi);
	if ( eResult != ERROR_NONE ) {
		eLog( LOG_APPLICATION, LOG_APOCALYPSE, "Failed to initialise SPI\r\n" );
	}

	/* Initialise flash chip */
	// eBoardEnablePeripheral( PERIPHERAL_ONBOARD_FLASH, NULL, portMAX_DELAY );
	vGpioClear( FLASH_WP_GPIO );
	eResult = eFlashInit( &xMX25rDevice );
	if ( eResult != ERROR_NONE ) {
		eLog( LOG_APPLICATION, LOG_APOCALYPSE, "Failed to initialise Flash with error code %d\r\n", eResult );
	}

}

/*-----------------------------------------------------------*/

/**
* @brief  flash write
* @param  None
* @retval None
*/
extern void hal_flash_write(uint32_t ulBlockNum, void *pvBlockData, uint32_t ulBlockSize) {

	// uint64_t ullAddress = ( (uint64_t) ulBlockNum << pxOnboardFlash->xSettings.ucPageSizePower );
	// uint32_t ulEraseSize = pxOnboardFlash->xSettings.usErasePages << pxOnboardFlash->xSettings.ucPageSizePower;

	// /* If the block is the start of an erase boundary, erase if */
	// if ( ulBlockNum % pxOnboardFlash->xSettings.usErasePages == 0 ) {
	// 	eFlashErase( pxOnboardFlash, ullAddress, ulEraseSize, pdMS_TO_TICKS( 1000 ) );
	// }

	// return eFlashWrite( pxOnboardFlash, ullAddress, pvBlockData, ulBlockSize, pdMS_TO_TICKS( 1000 ) );
	xOnboardLoggerDevice.fnWriteBlock(ulBlockNum, pvBlockData, ulBlockSize);

}

/*-----------------------------------------------------------*/

extern void hal_flash_read( uint32_t ulBlockNum, uint16_t usOffset, void *pvBlockData, uint32_t ulBlockSize )
{
	// uint64_t ullAddress = ( (uint64_t) ulBlockNum << pxOnboardFlash->xSettings.ucPageSizePower ) + usOffset;
	// return eFlashRead( pxOnboardFlash, ullAddress, pvBlockData, ulBlockSize, pdMS_TO_TICKS( 1000 ) );
	xOnboardLoggerDevice.fnReadBlock(ulBlockNum, usOffset, &pvBlockData, ulBlockSize);
}

/*-----------------------------------------------------------*/