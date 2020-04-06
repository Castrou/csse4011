###################################################
# APPLICATION SOURCES
###################################################

CLITASK_PATH = $(MYOSLIB_PATH)/cli
UTIL_PATH = $(MYOSLIB_PATH)/util
LOG_PATH = $(MYOSLIB_PATH)/log

# Set folder path with header files to include.
APP_CFLAGS += -I$(CLITASK_PATH)
APP_CFLAGS += -I$(UTIL_PATH)
APP_CFLAGS += -I$(LOG_PATH)

# List all c files that must be included (use space as seperate e.g. LIBSRCS += file1.c file2.c)
APPLICATION_SRCS += $(CLITASK_PATH)/cli_task.c
APPLICATION_SRCS += $(UTIL_PATH)/lib_util.c
APPLICATION_SRCS += $(UTIL_PATH)/cli_util.c
APPLICATION_SRCS += $(UTIL_PATH)/os_util.c
APPLICATION_SRCS += $(LOG_PATH)/lib_log.c
APPLICATION_SRCS += $(LOG_PATH)/cli_log.c
APPLICATION_SRCS += $(LOG_PATH)/os_log.c