###################################################
# APPLICATION SOURCES
###################################################

FREERTOSCLI_PATH = $(FREERTOSPLUS_PATH)/FreeRTOS-Plus-CLI

# Set folder path with header files to include.
APP_CFLAGS += -I$(FREERTOSCLI_PATH)

# List all c files that must be included (use space as seperate e.g. LIBSRCS += file1.c file2.c)
APPLICATION_SRCS += $(FREERTOSCLI_PATH)/FreeRTOS_CLI.c