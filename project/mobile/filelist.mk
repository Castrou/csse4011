###################################################
# APPLICATION SOURCES
###################################################

CLITASK_PATH = $(MYOSLIB_PATH)/cli
UTIL_PATH = $(MYOSLIB_PATH)/util
LOG_PATH = $(MYOSLIB_PATH)/log
HCI_PATH = $(MYOSLIB_PATH)/hci
BT_PATH = $(MYOSLIB_PATH)/bt
FLASH_PATH = $(MYOSLIB_PATH)/flash
LOC_PATH = $(MYOSLIB_PATH)/localisation

# Set folder path with header files to include.
APP_CFLAGS += -I$(CLITASK_PATH)
APP_CFLAGS += -I$(UTIL_PATH)
APP_CFLAGS += -I$(LOG_PATH)
APP_CFLAGS += -I$(HCI_PATH)
APP_CFLAGS += -I$(BT_PATH)
APP_CFLAGS += -I$(FLASH_PATH)
APP_CFLAGS += -I$(LOC_PATH)

# List all c files that must be included (use space as seperate e.g. LIBSRCS += file1.c file2.c)
APPLICATION_SRCS += $(CLITASK_PATH)/cli_task.c
# Utility
APPLICATION_SRCS += $(UTIL_PATH)/lib_util.c
APPLICATION_SRCS += $(UTIL_PATH)/cli_util.c
APPLICATION_SRCS += $(UTIL_PATH)/os_util.c
# CLI
APPLICATION_SRCS += $(LOG_PATH)/cli_log.c
APPLICATION_SRCS += $(LOG_PATH)/os_log.c
# HCI
APPLICATION_SRCS += $(HCI_PATH)/hal_hci.c
APPLICATION_SRCS += $(HCI_PATH)/cli_hci.c
APPLICATION_SRCS += $(HCI_PATH)/lib_hci.c
APPLICATION_SRCS += $(HCI_PATH)/os_hci.c
# Bluetooth
APPLICATION_SRCS += $(BT_PATH)/os_bt.c
APPLICATION_SRCS += $(BT_PATH)/lib_bt.c
APPLICATION_SRCS += $(BT_PATH)/cli_bt.c
# Flash
APPLICATION_SRCS += $(FLASH_PATH)/cli_flash.c
APPLICATION_SRCS += $(FLASH_PATH)/hal_flash.c
APPLICATION_SRCS += $(FLASH_PATH)/os_flash.c
# Localisation
APPLICATION_SRCS += $(LOC_PATH)/os_loc.c