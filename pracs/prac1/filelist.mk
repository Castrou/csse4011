# MYOSLIB_PATH = ./../../myoslib 

CLI_PATH = $(MYOSLIB_PATH)/cli 
UTIL_PATH = $(MYOSLIB_PATH)/util

# Include Headers
CFLAGS += -I$(CLI_PATH)
CFLAGS += -I$(UTIL_PATH)

# Include source files
LIBSRCS += $(wildcard $(CLI_PATH)/*.c)
LIBSRCS += $(wildcard $(UTIL_PATH)/*.c)