###################################################
# APPLICATION SOURCES
###################################################

LOG_PATH = $(MYOSLIB_PATH)/log
US_PATH = $(MYOSLIB_PATH)/ultrasonic


# Set folder path with header files to include.
APP_CFLAGS += -I$(LOG_PATH)
APP_CFLAGS += -I$(US_PATH)

# List all c files that must be included (use space as seperate e.g. LIBSRCS += file1.c file2.c)
APPLICATION_SRCS += $(LOG_PATH)/cli_log.c
APPLICATION_SRCS += $(LOG_PATH)/os_log.c
APPLICATION_SRCS += $(US_PATH)/hal_ultrasonic.c
APPLICATION_SRCS += $(US_PATH)/os_ultrasonic.c