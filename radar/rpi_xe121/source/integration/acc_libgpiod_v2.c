// Copyright (c) Acconeer AB, 2025-2026
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
#include "acc_libgpiod_v2.h"

#define RPI_GPIO_CHIPNAME             "/dev/gpiochip0"
#define GPIOD_EVENT_BUFFER_MAX_EVENTS 5

typedef enum
{
	SYMBOL_GPIOD_API_VERSION, // Needs to be placed first
	SYMBOL_GPIOD_CHIP_CLOSE,
	SYMBOL_GPIOD_CHIP_OPEN,
	SYMBOL_GPIOD_CHIP_REQUEST_LINES,
	SYMBOL_GPIOD_EDGE_EVENT_BUFFER_FREE,
	SYMBOL_GPIOD_EDGE_EVENT_BUFFER_GET_EVENT,
	SYMBOL_GPIOD_EDGE_EVENT_BUFFER_NEW,
	SYMBOL_GPIOD_EDGE_EVENT_GET_EVENT_TYPE,
	SYMBOL_GPIOD_LINE_CONFIG_ADD_LINE_SETTINGS,
	SYMBOL_GPIOD_LINE_CONFIG_FREE,
	SYMBOL_GPIOD_LINE_CONFIG_NEW,
	SYMBOL_GPIOD_LINE_CONFIG_SET_OUTPUT_VALUES,
	SYMBOL_GPIOD_LINE_REQUEST_GET_VALUE,
	SYMBOL_GPIOD_LINE_REQUEST_READ_EDGE_EVENTS,
	SYMBOL_GPIOD_LINE_REQUEST_RECONFIGURE_LINES,
	SYMBOL_GPIOD_LINE_REQUEST_RELEASE,
	SYMBOL_GPIOD_LINE_REQUEST_SET_VALUE,
	SYMBOL_GPIOD_LINE_REQUEST_WAIT_EDGE_EVENTS,
	SYMBOL_GPIOD_LINE_SETTINGS_FREE,
	SYMBOL_GPIOD_LINE_SETTINGS_NEW,
	SYMBOL_GPIOD_LINE_SETTINGS_SET_BIAS,
	SYMBOL_GPIOD_LINE_SETTINGS_SET_DEBOUNCE_PERIOD_US,
	SYMBOL_GPIOD_LINE_SETTINGS_SET_DIRECTION,
	SYMBOL_GPIOD_LINE_SETTINGS_SET_DRIVE,
	SYMBOL_GPIOD_LINE_SETTINGS_SET_EDGE_DETECTION,
	SYMBOL_GPIOD_LINE_SETTINGS_SET_EVENT_CLOCK,
	SYMBOL_GPIOD_REQUEST_CONFIG_FREE,
	SYMBOL_GPIOD_REQUEST_CONFIG_NEW,
	SYMBOL_GPIOD_REQUEST_CONFIG_SET_CONSUMER,
	SYMBOL_GPIOD_MAX,
} symbol_gpiod_t;

static const char *symbol_names_v2[SYMBOL_GPIOD_MAX] = {
    "gpiod_api_version",
    "gpiod_chip_close",
    "gpiod_chip_open",
    "gpiod_chip_request_lines",
    "gpiod_edge_event_buffer_free",
    "gpiod_edge_event_buffer_get_event",
    "gpiod_edge_event_buffer_new",
    "gpiod_edge_event_get_event_type",
    "gpiod_line_config_add_line_settings",
    "gpiod_line_config_free",
    "gpiod_line_config_new",
    "gpiod_line_config_set_output_values",
    "gpiod_line_request_get_value",
    "gpiod_line_request_read_edge_events",
    "gpiod_line_request_reconfigure_lines",
    "gpiod_line_request_release",
    "gpiod_line_request_set_value",
    "gpiod_line_request_wait_edge_events",
    "gpiod_line_settings_free",
    "gpiod_line_settings_new",
    "gpiod_line_settings_set_bias",
    "gpiod_line_settings_set_debounce_period_us",
    "gpiod_line_settings_set_direction",
    "gpiod_line_settings_set_drive",
    "gpiod_line_settings_set_edge_detection",
    "gpiod_line_settings_set_event_clock",
    "gpiod_request_config_free",
    "gpiod_request_config_new",
    "gpiod_request_config_set_consumer",
};

struct gpiod_line_config;
struct gpiod_line_request;
struct gpiod_line_settings;
struct gpiod_request_config;
struct gpiod_edge_event_buffer;

typedef struct
{
	struct gpiod_line_request      *request;
	struct gpiod_edge_event_buffer *events;
	size_t                          max_events;
	gpio_direction_t                direction;
} gpio_pin_t;

static gpio_pin_t gpios[ACC_LIBGPIOD_PIN_COUNT];

enum gpiod_line_value
{
	GPIOD_LINE_VALUE_ERROR    = -1,
	GPIOD_LINE_VALUE_INACTIVE = 0,
	GPIOD_LINE_VALUE_ACTIVE   = 1,
};

enum gpiod_line_direction
{
	GPIOD_LINE_DIRECTION_AS_IS = 1,
	GPIOD_LINE_DIRECTION_INPUT,
	GPIOD_LINE_DIRECTION_OUTPUT,
};

enum gpiod_line_drive
{
	GPIOD_LINE_DRIVE_PUSH_PULL = 1,
	GPIOD_LINE_DRIVE_OPEN_DRAIN,
	GPIOD_LINE_DRIVE_OPEN_SOURCE,
};

enum gpiod_line_edge
{
	GPIOD_LINE_EDGE_NONE = 1,
	GPIOD_LINE_EDGE_RISING,
	GPIOD_LINE_EDGE_FALLING,
	GPIOD_LINE_EDGE_BOTH,
};

enum gpiod_edge_event_type
{
	GPIOD_EDGE_EVENT_RISING_EDGE = 1,
	GPIOD_EDGE_EVENT_FALLING_EDGE,
};

typedef char *(*gpiod_api_version_t)(void);
typedef struct gpiod_chip *(*gpiod_chip_open_t)(const char *path);
typedef void (*gpiod_chip_close_t)(struct gpiod_chip *chip);
typedef struct gpiod_line_request *(*gpiod_chip_request_lines_t)(struct gpiod_chip           *chip,
                                                                 struct gpiod_request_config *req_cfg,
                                                                 struct gpiod_line_config    *line_cfg);

typedef struct gpiod_line_settings *(*gpiod_line_settings_new_t)(void);
typedef void (*gpiod_line_settings_free_t)(struct gpiod_line_settings *settings);
typedef int (*gpiod_line_settings_set_direction_t)(struct gpiod_line_settings *settings, enum gpiod_line_direction direction);
typedef int (*gpiod_line_settings_set_drive_t)(struct gpiod_line_settings *settings, enum gpiod_line_drive drive);
typedef int (*gpiod_line_settings_set_edge_detection_t)(struct gpiod_line_settings *settings, enum gpiod_line_edge edge);
typedef int (*gpiod_line_config_add_line_settings_t)(struct gpiod_line_config   *config,
                                                     const unsigned int         *offsets,
                                                     size_t                      num_offsets,
                                                     struct gpiod_line_settings *settings);

typedef struct gpiod_line_config *(*gpiod_line_config_new_t)(void);
typedef void (*gpiod_line_config_free_t)(struct gpiod_line_config *config);
typedef int (*gpiod_line_config_set_output_values_t)(struct gpiod_line_config *config, const enum gpiod_line_value *values, size_t num_values);

typedef void (*gpiod_line_request_release_t)(struct gpiod_line_request *request);
typedef int (*gpiod_line_request_wait_edge_events_t)(struct gpiod_line_request *request, int64_t timeout_ns);
typedef int (*gpiod_line_request_read_edge_events_t)(struct gpiod_line_request *request, struct gpiod_edge_event_buffer *buffer, size_t max_events);

typedef struct gpiod_request_config *(*gpiod_request_config_new_t)(void);
typedef void (*gpiod_request_config_free_t)(struct gpiod_request_config *config);
typedef void (*gpiod_request_config_set_consumer_t)(struct gpiod_request_config *config, const char *consumer);

typedef int (*gpiod_line_request_set_value_t)(struct gpiod_line_request *request, unsigned int offset, enum gpiod_line_value value);
typedef enum gpiod_line_value (*gpiod_line_request_get_value_t)(struct gpiod_line_request *request, unsigned int offset);
typedef int (*gpiod_line_request_reconfigure_lines_t)(struct gpiod_line_request *request, struct gpiod_line_config *config);

typedef struct gpiod_edge_event_buffer *(*gpiod_edge_event_buffer_new_t)(size_t capacity);
typedef void (*gpiod_edge_event_buffer_free_t)(struct gpiod_edge_event_buffer *buffer);
typedef struct gpiod_edge_event *(*gpiod_edge_event_buffer_get_event_t)(struct gpiod_edge_event_buffer *buffer, unsigned long index);
typedef enum gpiod_edge_event_type (*gpiod_edge_event_get_event_type_t)(struct gpiod_edge_event *event);

const char **acc_libgpiod_v2_get_symbols(uint32_t *nr_of_symbols)
{
	if (nr_of_symbols != NULL)
	{
		*nr_of_symbols = sizeof(symbol_names_v2) / sizeof(symbol_names_v2[0]);
	}
	return symbol_names_v2;
}

static int apply_line_setting(acc_libgpiod_symbol_t *symbols, unsigned int pin, gpio_direction_t direction, struct gpiod_line_config *line_config)
{
	struct gpiod_line_settings *settings = ((gpiod_line_settings_new_t)symbols[SYMBOL_GPIOD_LINE_SETTINGS_NEW].symbol)();

	int res = -1;

	switch (direction)
	{
		case GPIO_DIR_INPUT_INTERRUPT:
		{
			res =
			    ((gpiod_line_settings_set_direction_t)symbols[SYMBOL_GPIOD_LINE_SETTINGS_SET_DIRECTION].symbol)(settings, GPIOD_LINE_DIRECTION_INPUT);

			if (res == 0)
			{
				res =
				    ((gpiod_line_settings_set_edge_detection_t)symbols[SYMBOL_GPIOD_LINE_SETTINGS_SET_EDGE_DETECTION].symbol)(settings,
				                                                                                                              GPIOD_LINE_EDGE_RISING);
			}
			break;
		}
		case GPIO_DIR_OUTPUT_LOW:
		case GPIO_DIR_OUTPUT_HIGH:
		{
			res = ((gpiod_line_settings_set_direction_t)symbols[SYMBOL_GPIOD_LINE_SETTINGS_SET_DIRECTION].symbol)(settings,
			                                                                                                      GPIOD_LINE_DIRECTION_OUTPUT);
			if (res == 0)
			{
				res = ((gpiod_line_settings_set_drive_t)symbols[SYMBOL_GPIOD_LINE_SETTINGS_SET_DRIVE].symbol)(settings, GPIOD_LINE_DRIVE_PUSH_PULL);
			}
			if (res == 0)
			{
				const enum gpiod_line_value value = (direction == GPIO_DIR_OUTPUT_HIGH) ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE;
				res = ((gpiod_line_config_set_output_values_t)symbols[SYMBOL_GPIOD_LINE_CONFIG_SET_OUTPUT_VALUES].symbol)(line_config, &value, 1);
			}
			break;
		}
		default:
			res = -1;
			break;
	}
	if (res == 0)
	{
		res = ((gpiod_line_config_add_line_settings_t)symbols[SYMBOL_GPIOD_LINE_CONFIG_ADD_LINE_SETTINGS].symbol)(line_config, &pin, 1, settings);
	}
	((gpiod_line_settings_free_t)symbols[SYMBOL_GPIOD_LINE_SETTINGS_FREE].symbol)(settings);

	return res;
}

static bool gpio_open(acc_libgpiod_symbol_t *symbols, struct gpiod_chip *chip, unsigned int pin, gpio_direction_t direction)
{
	gpios[pin].direction = direction;

	int res;

	struct gpiod_line_config *line_config = ((gpiod_line_config_new_t)symbols[SYMBOL_GPIOD_LINE_CONFIG_NEW].symbol)();

	switch (direction)
	{
		case GPIO_DIR_INPUT_INTERRUPT:
			res = apply_line_setting(symbols, pin, direction, line_config);
			if (res == 0)
			{
				gpios[pin].max_events = GPIOD_EVENT_BUFFER_MAX_EVENTS;
				gpios[pin].events     = ((gpiod_edge_event_buffer_new_t)symbols[SYMBOL_GPIOD_EDGE_EVENT_BUFFER_NEW].symbol)(gpios[pin].max_events);
			}
			break;
		case GPIO_DIR_OUTPUT_LOW:
		case GPIO_DIR_OUTPUT_HIGH:
		{
			res = apply_line_setting(symbols, pin, direction, line_config);
			if (res == 0)
			{
				enum gpiod_line_value value = (direction == GPIO_DIR_OUTPUT_HIGH) ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE;
				res = ((gpiod_line_config_set_output_values_t)symbols[SYMBOL_GPIOD_LINE_CONFIG_SET_OUTPUT_VALUES].symbol)(line_config, &value, 1);
			}
			break;
		}
		default:
			res = -1;
			assert(false);
	}

	if (res == 0)
	{
		struct gpiod_request_config *request_config = ((gpiod_request_config_new_t)symbols[SYMBOL_GPIOD_REQUEST_CONFIG_NEW].symbol)();

		((gpiod_request_config_set_consumer_t)symbols[SYMBOL_GPIOD_REQUEST_CONFIG_SET_CONSUMER].symbol)(request_config, ACC_LIBGPIOD_CONSUMER);
		gpios[pin].request = ((gpiod_chip_request_lines_t)symbols[SYMBOL_GPIOD_CHIP_REQUEST_LINES].symbol)(chip, request_config, line_config);
		((gpiod_request_config_free_t)symbols[SYMBOL_GPIOD_REQUEST_CONFIG_FREE].symbol)(request_config);
	}

	((gpiod_line_config_free_t)symbols[SYMBOL_GPIOD_LINE_CONFIG_FREE].symbol)(line_config);

	if (res != 0)
	{
		perror("gpiod_chip_open failed");
		return false;
	}

	return true;
}

bool acc_libgpiod_v2_init(acc_libgpiod_symbol_t *symbols, struct gpiod_chip **chip, const gpio_config_t *pin_config)
{
	int pin;

	for (pin = 0; pin < ACC_LIBGPIOD_PIN_COUNT; pin++)
	{
		gpios[pin].direction = GPIO_DIR_UNKNOWN;
	}

	*chip = ((gpiod_chip_open_t)symbols[SYMBOL_GPIOD_CHIP_OPEN].symbol)(RPI_GPIO_CHIPNAME);
	if (*chip == NULL)
	{
		perror("gpiod_chip_open failed");
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

void acc_libgpiod_v2_deinit(acc_libgpiod_symbol_t *symbols, struct gpiod_chip *chip)
{
	if (chip != NULL)
	{
		for (int pin = 0; pin < ACC_LIBGPIOD_PIN_COUNT; pin++)
		{
			if (gpios[pin].direction != GPIO_DIR_UNKNOWN)
			{
				((gpiod_line_request_release_t)symbols[SYMBOL_GPIOD_LINE_REQUEST_RELEASE].symbol)(gpios[pin].request);
				gpios[pin].request = NULL;
				((gpiod_edge_event_buffer_free_t)symbols[SYMBOL_GPIOD_EDGE_EVENT_BUFFER_FREE].symbol)(gpios[pin].events);
				gpios[pin].events    = NULL;
				gpios[pin].direction = GPIO_DIR_UNKNOWN;
			}
		}

		((gpiod_chip_close_t)symbols[SYMBOL_GPIOD_CHIP_CLOSE].symbol)(chip);
	}
}

bool acc_libgpiod_v2_set(acc_libgpiod_symbol_t *symbols, int pin, gpio_pin_value_t value)
{
	int res = ((gpiod_line_request_set_value_t)symbols[SYMBOL_GPIOD_LINE_REQUEST_SET_VALUE].symbol)(gpios[pin].request,
	                                                                                                pin,
	                                                                                                (value == PIN_HIGH) ? GPIOD_LINE_VALUE_ACTIVE
	                                                                                                                    : GPIOD_LINE_VALUE_INACTIVE);
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

static int get_line_value(acc_libgpiod_symbol_t *symbols, int pin)
{
	int line_value = ((gpiod_line_request_get_value_t)symbols[SYMBOL_GPIOD_LINE_REQUEST_GET_VALUE].symbol)(gpios[pin].request, pin);
	if (line_value < 0)
	{
		perror("gpiod_line_request_get_value failed");
		assert(true);
	}

	return line_value;
}

bool acc_libgpiod_v2_wait_for_interrupt(acc_libgpiod_symbol_t *symbols, int pin, uint32_t timeout_ms)
{
	struct timespec start;
	unsigned int    elapsed_ms            = 0;
	int64_t         wait_event_timeout_ms = timeout_ms;

	clock_gettime(CLOCK_MONOTONIC, &start);

	int line_value = get_line_value(symbols, pin);

	while ((line_value != GPIOD_LINE_VALUE_ACTIVE) && (wait_event_timeout_ms > 0))
	{
		const int64_t timeout_ns = wait_event_timeout_ms * 1000000;
		int res = ((gpiod_line_request_wait_edge_events_t)symbols[SYMBOL_GPIOD_LINE_REQUEST_WAIT_EDGE_EVENTS].symbol)(gpios[pin].request, timeout_ns);
		if (res < 0)
		{
			perror("gpiod_line_request_wait_edge_events failed");
			assert(true);
		}
		if (res == 0) // No event available
		{
			// Timeout or interrupt high without any edge event
			break;
		}
		int num_events = ((gpiod_line_request_read_edge_events_t)symbols[SYMBOL_GPIOD_LINE_REQUEST_READ_EDGE_EVENTS].symbol)(gpios[pin].request,
		                                                                                                                     gpios[pin].events,
		                                                                                                                     gpios[pin].max_events);
		if (num_events == 0)
		{
			perror("gpiod_line_request_read_edge_events failed");
			assert(true);
		}

		struct gpiod_edge_event *event =
		    ((gpiod_edge_event_buffer_get_event_t)symbols[SYMBOL_GPIOD_EDGE_EVENT_BUFFER_GET_EVENT].symbol)(gpios[pin].events, 0);
		enum gpiod_edge_event_type event_type = ((gpiod_edge_event_get_event_type_t)symbols[SYMBOL_GPIOD_EDGE_EVENT_GET_EVENT_TYPE].symbol)(event);

		if (event_type != GPIOD_EDGE_EVENT_RISING_EDGE)
		{
			fprintf(stderr, "Unexpected event_type: %d (expected %d), library version mismatch?\n", event_type, GPIOD_EDGE_EVENT_RISING_EDGE);
			fflush(stderr);
			assert(true);
		}

		elapsed_ms             = get_elapsed_ms(&start);
		wait_event_timeout_ms -= elapsed_ms;

		line_value = get_line_value(symbols, pin);
	}

	return line_value == GPIOD_LINE_VALUE_ACTIVE;
}

