// Copyright (c) Acconeer AB, 2025
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.
#define _POSIX_C_SOURCE 199309L

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include "acc_libgpiod.h"
#include "acc_libgpiod_v1.h"

#define RPI_GPIO_CHIPNAME "gpiochip0"

typedef enum
{
	SYMBOL_GPIOD_VERSION_STRING, // Needs to be placed first
	SYMBOL_GPIOD_CHIP_OPEN_BY_NAME,
	SYMBOL_GPIOD_CHIP_CLOSE,
	SYMBOL_GPIOD_CHIP_GET_LINE,
	SYMBOL_GPIOD_LINE_EVENT_READ,
	SYMBOL_GPIOD_LINE_EVENT_WAIT,
	SYMBOL_GPIOD_LINE_GET_VALUE,
	SYMBOL_GPIOD_LINE_RELEASE,
	SYMBOL_GPIOD_LINE_REQUEST_OUTPUT,
	SYMBOL_GPIOD_LINE_REQUEST_RISING_EDGE_EVENTS,
	SYMBOL_GPIOD_LINE_SET_VALUE,
	SYMBOL_GPIOD_MAX,
} symbol_gpiod_t;

static const char *symbol_names_v1[SYMBOL_GPIOD_MAX] = {
    "gpiod_version_string",
    "gpiod_chip_open_by_name",
    "gpiod_chip_close",
    "gpiod_chip_get_line",
    "gpiod_line_event_read",
    "gpiod_line_event_wait",
    "gpiod_line_get_value",
    "gpiod_line_release",
    "gpiod_line_request_output",
    "gpiod_line_request_rising_edge_events",
    "gpiod_line_set_value",
};

enum
{
	GPIOD_LINE_EVENT_RISING_EDGE = 1,
	/*< Rising edge event. */
	GPIOD_LINE_EVENT_FALLING_EDGE,
	/*< Falling edge event. */
};

struct gpiod_line_event
{
	struct timespec ts;
	/*< Best estimate of time of event occurrence. */
	int event_type;
	/*< Type of the event that occurred. */
};

typedef struct
{
	struct gpiod_line *line;
	gpio_direction_t   direction;
} gpio_pin_t;

typedef char *(*gpiod_version_string_t)(void);
typedef struct gpiod_chip *(*gpiod_chip_open_t)(const char *name);
typedef void (*gpiod_chip_close_t)(struct gpiod_chip *chip);
typedef struct gpiod_line *(*gpiod_chip_get_line_t)(struct gpiod_chip *chip, unsigned int offset);
typedef void (*gpiod_line_release_t)(struct gpiod_line *line);
typedef int (*gpiod_line_request_output_t)(struct gpiod_line *line, const char *consumer, int default_val);
typedef int (*gpiod_line_request_rising_edge_events_t)(struct gpiod_line *line, const char *consumer);
typedef int (*gpiod_line_get_value_t)(struct gpiod_line *line);
typedef int (*gpiod_line_set_value_t)(struct gpiod_line *line, int value);
typedef int (*gpiod_line_event_wait_t)(struct gpiod_line *line, const struct timespec *timeout);
typedef int (*gpiod_line_event_read_t)(struct gpiod_line *line, struct gpiod_line_event *event);

static gpio_pin_t gpios[ACC_LIBGPIOD_PIN_COUNT];

const char **acc_libgpiod_v1_get_symbols(uint32_t *nr_of_symbols)
{
	if (nr_of_symbols != NULL)
	{
		*nr_of_symbols = sizeof(symbol_names_v1) / sizeof(symbol_names_v1[0]);
	}
	return symbol_names_v1;
}

static bool gpio_open(acc_libgpiod_symbol_t *symbols, struct gpiod_chip *chip, int pin, gpio_direction_t direction)
{
	gpios[pin].line = ((gpiod_chip_get_line_t)symbols[SYMBOL_GPIOD_CHIP_GET_LINE].symbol)(chip, pin);

	if (gpios[pin].line == NULL)
	{
		perror("Failed opening line");
		return false;
	}

	gpios[pin].direction = direction;

	int res;

	switch (direction)
	{
		case GPIO_DIR_INPUT_INTERRUPT:
			res = ((gpiod_line_request_rising_edge_events_t)symbols[SYMBOL_GPIOD_LINE_REQUEST_RISING_EDGE_EVENTS].symbol)(gpios[pin].line,
			                                                                                                              ACC_LIBGPIOD_CONSUMER);
			break;
		case GPIO_DIR_OUTPUT_LOW:
		case GPIO_DIR_OUTPUT_HIGH:
			res = ((gpiod_line_request_output_t)symbols[SYMBOL_GPIOD_LINE_REQUEST_OUTPUT].symbol)(gpios[pin].line,
			                                                                                      ACC_LIBGPIOD_CONSUMER,
			                                                                                      direction == GPIO_DIR_OUTPUT_HIGH ? PIN_HIGH
			                                                                                                                        : PIN_LOW);
			break;
		default:
			res = -1;
			assert(false);
	}

	if (res != 0)
	{
		fprintf(stderr, "gpio_open failed");
		fflush(stderr);
		return false;
	}

	return true;
}

bool acc_libgpiod_v1_init(acc_libgpiod_symbol_t *symbols, struct gpiod_chip **chip, const gpio_config_t *pin_config)
{
	int pin;

	for (pin = 0; pin < ACC_LIBGPIOD_PIN_COUNT; pin++)
	{
		gpios[pin].direction = GPIO_DIR_UNKNOWN;
	}

	*chip = ((gpiod_chip_open_t)symbols[SYMBOL_GPIOD_CHIP_OPEN_BY_NAME].symbol)(RPI_GPIO_CHIPNAME);
	if (*chip == NULL)
	{
		perror("gpiod_chip_open_by_name failed");
		goto error;
	}

	pin = 0;

	while (pin_config[pin].direction != GPIO_DIR_UNKNOWN)
	{
		if (!gpio_open(symbols, *chip, pin_config[pin].pin, pin_config[pin].direction))
		{
			goto error;
		}

		pin++;
	}

	goto out;

error:
	return false;

out:
	return true;
}

void acc_libgpiod_v1_deinit(acc_libgpiod_symbol_t *symbols, struct gpiod_chip *chip)
{
	if (chip != NULL)
	{
		for (int pin = 0; pin < ACC_LIBGPIOD_PIN_COUNT; pin++)
		{
			if (gpios[pin].direction != GPIO_DIR_UNKNOWN)
			{
				((gpiod_line_release_t)symbols[SYMBOL_GPIOD_LINE_RELEASE].symbol)(gpios[pin].line);
				gpios[pin].direction = GPIO_DIR_UNKNOWN;
			}
		}

		((gpiod_chip_close_t)symbols[SYMBOL_GPIOD_CHIP_CLOSE].symbol)(chip);
	}
}

bool acc_libgpiod_v1_set(acc_libgpiod_symbol_t *symbols, int pin, gpio_pin_value_t value)
{
	assert((gpios[pin].direction == GPIO_DIR_OUTPUT_HIGH) || (gpios[pin].direction == GPIO_DIR_OUTPUT_LOW));

	int res = ((gpiod_line_set_value_t)symbols[SYMBOL_GPIOD_LINE_SET_VALUE].symbol)(gpios[pin].line, value);
	if (res != 0)
	{
		perror("gpiod_line_set_value failed");
		return false;
	}
	return true;
}

static unsigned int get_elapsed_ms(struct timespec *start)
{
	struct timespec now;

	clock_gettime(CLOCK_MONOTONIC, &now);
	int elapsed = (((now.tv_sec - start->tv_sec) * 1000) + (now.tv_nsec - start->tv_nsec) / 1000000);
	assert(elapsed >= 0);
	return elapsed;
}

bool acc_libgpiod_v1_wait_for_interrupt(acc_libgpiod_symbol_t *symbols, int pin, uint32_t timeout_ms)
{
	assert(gpios[pin].direction == GPIO_DIR_INPUT_INTERRUPT);

	struct timespec start;
	clock_gettime(CLOCK_MONOTONIC, &start);

	int pin_value = ((gpiod_line_get_value_t)symbols[SYMBOL_GPIOD_LINE_GET_VALUE].symbol)(gpios[pin].line);
	assert(pin_value >= 0);

	unsigned int elapsed_ms = get_elapsed_ms(&start);

	while ((pin_value != PIN_HIGH) && (elapsed_ms < timeout_ms))
	{
		timeout_ms           -= elapsed_ms;
		time_t          secs  = timeout_ms / 1000;
		long            ns    = (timeout_ms % 1000) * 1000000;
		struct timespec ts    = {secs, ns};
		int             res   = ((gpiod_line_event_wait_t)symbols[SYMBOL_GPIOD_LINE_EVENT_WAIT].symbol)(gpios[pin].line, &ts);
		if (res < 0)
		{
			perror("gpiod_line_event_wait failed");
			assert(true);
		}
		else if (res == 0)
		{
			// Timeout
			break;
		}
		else
		{
			// Wait complete, check which event we got.
			struct gpiod_line_event event;
			res = ((gpiod_line_event_read_t)symbols[SYMBOL_GPIOD_LINE_EVENT_READ].symbol)(gpios[pin].line, &event);
			if (res != 0)
			{
				perror("gpiod_line_event_read failed");
				assert(true);
			}

			if (event.event_type != GPIOD_LINE_EVENT_RISING_EDGE)
			{
				// The enums were renumbered between v1.0 and v1.1, make sure gpiod.h matches
				// the shared library version if this happens.
				fprintf(stderr,
				        "Unexpected event_type: %d (expected %d), library version mismatch?\n",
				        event.event_type,
				        GPIOD_LINE_EVENT_RISING_EDGE);
				assert(true);
			}

			pin_value = ((gpiod_line_get_value_t)symbols[SYMBOL_GPIOD_LINE_GET_VALUE].symbol)(gpios[pin].line);
			if (pin_value < 0)
			{
				perror("gpiod_line_get_value failed");
				assert(true);
			}
		}

		elapsed_ms = get_elapsed_ms(&start);
	}
	return pin_value == PIN_HIGH;
}

