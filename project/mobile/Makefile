##############################################################################
# Application Settings
##############################################################################

PROJ_NAME     		:= cli_framework
SUPPORTED_TARGETS 	:= nrf52840dk bleatag argon xenon

EIFREERTOS_PATH = $(realpath ./../../../ei-freertos)
FREERTOSPLUS_PATH = $(realpath ./../../ei-changes/FreeRTOS-Plus)
MYOSLIB_PATH = $(realpath ./../../myoslib/)


##############################################################################
# Application Specific Flags
##############################################################################

# NRF_SDK_PATH = $(EIFREERTOS_PATH)/core_external/nrf52_sdk
# NRF_DRV_PATH = $(NRF_SDK_PATH)/modules/nrfx/drivers

APP_CFLAGS			:= #-I$(NRF_SDK_PATH)/components/libraries/uart

##############################################################################
# Application Specific Library Source Files
##############################################################################

CORE_CSIRO          		:= $(EIFREERTOS_PATH)/core_csiro
APPLICATION_SRCS 			:= 
override REPO_ROOT			:= $(EIFREERTOS_PATH)
override CORE_CSIRO_SRCS 	+= $(CORE_CSIRO_DIR)/peripherals/memory/src/mx25r.c

##############################################################################
# Main Build Rules
##############################################################################

include $(FREERTOSPLUS_PATH)/filelist.mk
include filelist.mk
include $(CORE_CSIRO)/m_common.mk