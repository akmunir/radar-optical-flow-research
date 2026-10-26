# Define the compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99

# Define the target executable name
TARGET = distance_detection

# Define the source file and object file
SRCS = distance_detection.c
OBJS = $(SRCS:.c=.o)

# Define the header files your source file depends on
DEPS = rpi_xe121/include/acc_definitions_a121.h ./rpi_xe121/include/acc_detector_distance.h ./rpi_xe121/include/acc_hal_definitions_a121.h./rpi_xe121/include/acc_hal_integration_a121.h./rpi_xe121/include/acc_integration.h./rpi_xe121/include/acc_integration_log.h./rpi_xe121/include/acc_rss_a121.h./rpi_xe121/include/acc_sensor.h./rpi_xe121/include/acc_version.h

# Default rule to build the program
all: $(TARGET)

# Rule to link the object file into the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile the source file into an object file
# This tracks changes in both main.c and your header files (DEPS)
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to clean up build files
clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean
