// Copyright (c) Acconeer AB, 2025
// All rights reserved

#ifndef ACC_LIBGPIOD_V1_H_
#define ACC_LIBGPIOD_V1_H_

#include <stdbool.h>
#include <stdint.h>

#include "acc_libgpiod.h"

const char **acc_libgpiod_v1_get_symbols(uint32_t *nr_of_symbols);
bool         acc_libgpiod_v1_init(acc_libgpiod_symbol_t *symbols, struct gpiod_chip **chip, const gpio_config_t *pin_config);
void         acc_libgpiod_v1_deinit(acc_libgpiod_symbol_t *symbols, struct gpiod_chip *chip);
bool         acc_libgpiod_v1_set(acc_libgpiod_symbol_t *symbols, int pin, gpio_pin_value_t value);
bool         acc_libgpiod_v1_wait_for_interrupt(acc_libgpiod_symbol_t *symbols, int pin, uint32_t timeout_ms);

#endif
