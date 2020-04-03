##############################################################################
# Application Settings
##############################################################################

PROJ_NAME     		:= prac1
SUPPORTED_TARGETS 	:= thunderboard2 nrf52840dk nrf52832dk bleatag argon xenon

MYOSLIB_PATH = 	./../../myoslib
EIFREERTOS_PATH = ./../../ei-freertos

##############################################################################
# Application Specific Flags
##############################################################################

# CLI_PATH = $(MYOSLIB_PATH)/cli
# UTIL_PATH = $(MYOSLIB_PATH)/util

APP_CFLAGS			:=

##############################################################################
# Application Specific Library Source Files
##############################################################################

CORE_CSIRO          := $(realpath $(EIFREERTOS_PATH)/core_csiro)
APPLICATION_SRCS 	:=
override REPO_ROOT 	:= $(EIFREERTOS_PATH)

##############################################################################
# Main Build Rules
##############################################################################

include $(CORE_CSIRO)/m_common.mk