
CC = arm-linux-gnueabihf-gcc

SDK_DIR = rpi_xe121
INCLUDE_DIR = $(SDK_DIR)/include
LIB_DIR = $(SDK_DIR)/lib

CFLAGS = -Wall -Wextra -std=c99
CPPFLAGS = -D_POSIX_C_SOURCE=200809L -I$(INCLUDE_DIR)

TARGET = distance_detection

SRCS = distance_detection.c \
       $(SDK_DIR)/source/integration/acc_board_xe121.c \
       $(SDK_DIR)/source/integration/acc_integration_linux.c \
       $(SDK_DIR)/source/integration/acc_integration_log.c \
       $(SDK_DIR)/source/integration/acc_libgpiod.c \
       $(SDK_DIR)/source/integration/acc_libgpiod_v1.c \
       $(SDK_DIR)/source/integration/acc_libgpiod_v2.c \
       $(SDK_DIR)/source/integration/acc_libspi.c \
       $(SDK_DIR)/source/integration/acc_time_integration.c


TARGET = distance_detection_iq_data_print

SRCS = distance_detection_iq_data_print.c \
       $(SDK_DIR)/source/integration/acc_board_xe121.c \
       $(SDK_DIR)/source/integration/acc_integration_linux.c \
       $(SDK_DIR)/source/integration/acc_integration_log.c \
       $(SDK_DIR)/source/integration/acc_libgpiod.c \
       $(SDK_DIR)/source/integration/acc_libgpiod_v1.c \
       $(SDK_DIR)/source/integration/acc_libgpiod_v2.c \
       $(SDK_DIR)/source/integration/acc_libspi.c \
       $(SDK_DIR)/source/integration/acc_time_integration.c

OBJS = $(SRCS:.c=.o)

LIBS = -L$(LIB_DIR) \
       -lacc_detector_distance_a121 \
       -lacconeer_a121 \
       -lgpiod \
       -lm

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LIBS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
