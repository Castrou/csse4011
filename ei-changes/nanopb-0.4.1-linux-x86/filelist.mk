###################################################
# APPLICATION SOURCES
###################################################

# Set folder path with header files to include.
APP_CFLAGS += -I$(NANOPB_PATH)

# List all c files that must be included (use space as seperate e.g. LIBSRCS += file1.c file2.c)
APPLICATION_SRCS += $(NANOPB_PATH)/pb_encode.c
APPLICATION_SRCS += $(NANOPB_PATH)/pb_decode.c
APPLICATION_SRCS += $(NANOPB_PATH)/pb_common.c