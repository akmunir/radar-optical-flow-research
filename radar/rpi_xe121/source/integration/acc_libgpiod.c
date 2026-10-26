// Copyright (c) Acconeer AB, 2019-2025
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.
#include <assert.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "acc_libgpiod.h"
#include "acc_libgpiod_v1.h"
#include "acc_libgpiod_v2.h"

typedef char *(*gpiod_api_version_t)(void);

static void                  *lib_handle;
static int                    libgpiod_version;
static struct gpiod_chip     *chip;
static acc_libgpiod_symbol_t *symbols;

static char *resolve_libgpiod_symbols(void *handle, const char **symbols_to_resolve, int symbols_len, acc_libgpiod_symbol_t *resolved_symbols)
{
	char *error = NULL;

	for (int i = 0; i < symbols_len; i++)
	{
		resolved_symbols[i].symbol = dlsym(handle, symbols_to_resolve[i]);
		error                      = dlerror();
		if (error != NULL)
		{
			return error;
		}
	}

	return NULL;
}

static bool resolve_libgpiod_version(void *handle)
{
	gpiod_api_version_t   api_version;
	acc_libgpiod_symbol_t api_version_symbol;

	const char **symbols_to_resolve = acc_libgpiod_v2_get_symbols(NULL);

	const char *error = resolve_libgpiod_symbols(handle, symbols_to_resolve, 1, &api_version_symbol);
	if (error != NULL)
	{
		symbols_to_resolve = acc_libgpiod_v1_get_symbols(NULL);
		error              = resolve_libgpiod_symbols(handle, symbols_to_resolve, 1, &api_version_symbol);
		if (error != NULL)
		{
			fprintf(stderr, "%s\n", error);
			fflush(stderr);
			return false;
		}
	}

	api_version = (gpiod_api_version_t)api_version_symbol.symbol;

	const char *version = api_version();
	printf("libgpiod version: %s\n", version);

	char *tok_version = malloc(strlen(version) + 1);
	strcpy(tok_version, version);
	char       *saveptr;
	const char *major_version = strtok_r(tok_version, ".", &saveptr);
	libgpiod_version          = strcmp("1", major_version) == 0 ? 1 : 2;
	free(tok_version);

	return true;
}

bool acc_libgpiod_init(const gpio_config_t *pin_config)
{
	lib_handle = dlopen("libgpiod.so", RTLD_LAZY);
	if (!lib_handle)
	{
		lib_handle = dlopen("libgpiod.so.2", RTLD_LAZY);
	}
	if (!lib_handle)
	{
		fprintf(stderr, "%s\n", dlerror());
		fflush(stderr);
		return false;
	}

	// Clear any existing errors
	dlerror();

	if (!resolve_libgpiod_version(lib_handle))
	{
		goto error;
	}

	uint32_t     nr_of_symbols_to_resolve;
	const char **symbols_to_resolve;
	bool         init_success;

	if (libgpiod_version == 1)
	{
		symbols_to_resolve = acc_libgpiod_v1_get_symbols(&nr_of_symbols_to_resolve);
	}
	else
	{
		symbols_to_resolve = acc_libgpiod_v2_get_symbols(&nr_of_symbols_to_resolve);
	}

	symbols = malloc(sizeof(acc_libgpiod_symbol_t) * nr_of_symbols_to_resolve);
	if (symbols == NULL)
	{
		goto error;
	}

	const char *error_str = resolve_libgpiod_symbols(lib_handle, symbols_to_resolve, nr_of_symbols_to_resolve, symbols);
	if (error_str != NULL)
	{
		fprintf(stderr, "%s\n", error_str);
		fflush(stderr);
		goto error;
	}

	if (libgpiod_version == 1)
	{
		init_success = acc_libgpiod_v1_init(symbols, &chip, pin_config);
	}
	else
	{
		init_success = acc_libgpiod_v2_init(symbols, &chip, pin_config);
	}

	if (init_success)
	{
		goto out;
	}

error:
	dlclose(lib_handle);
	lib_handle = NULL;
	free(symbols);
	symbols = NULL;
	return false;

out:
	return true;
}

void acc_libgpiod_deinit(void)
{
	if (libgpiod_version == 1)
	{
		acc_libgpiod_v1_deinit(symbols, chip);
	}
	else
	{
		acc_libgpiod_v2_deinit(symbols, chip);
	}
	chip = NULL;

	if (lib_handle)
	{
		dlclose(lib_handle);
		lib_handle = NULL;
	}

	if (symbols)
	{
		free(symbols);
		symbols = NULL;
	}
}

bool acc_libgpiod_set(int pin, gpio_pin_value_t value)
{
	assert(pin < ACC_LIBGPIOD_PIN_COUNT);

	if (libgpiod_version == 1)
	{
		return acc_libgpiod_v1_set(symbols, pin, value);
	}

	return acc_libgpiod_v2_set(symbols, pin, value);
}

bool acc_libgpiod_wait_for_interrupt(int pin, uint32_t timeout_ms)
{
	assert(pin < ACC_LIBGPIOD_PIN_COUNT);

	if (libgpiod_version == 1)
	{
		return acc_libgpiod_v1_wait_for_interrupt(symbols, pin, timeout_ms);
	}

	return acc_libgpiod_v2_wait_for_interrupt(symbols, pin, timeout_ms);
}

