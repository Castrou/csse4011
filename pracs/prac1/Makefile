##############################################################################
# Application Settings
##############################################################################

PROJ_NAME     		:= prac2
SUPPORTED_TARGETS 	:= nrf52840dk bleatag argon xenon

EIFREERTOS_PATH = $(realpath ./../../../ei-freertos)
FREERTOSPLUS_PATH = $(realpath ./../../ei-changes/FreeRTOS-Plus)
MYOSLIB_PATH = $(realpath ./../../myoslib/)


##############################################################################
# Application Specific Flags
##############################################################################

APP_CFLAGS			:= 

##############################################################################
# Application Specific Library Source Files
##############################################################################

CORE_CSIRO          := $(EIFREERTOS_PATH)/core_csiro
APPLICATION_SRCS 	:= 
override REPO_ROOT	:= $(EIFREERTOS_PATH)

##############################################################################
# Main Build Rules
##############################################################################

# include filelist.mk
include $(FREERTOSPLUS_PATH)/filelist.mk
include filelist.mk
include $(CORE_CSIRO)/m_common.mk
