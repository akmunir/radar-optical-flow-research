// Copyright (c) Acconeer AB, 2025
// All rights reserved

#ifndef ACC_LIBGPIOD_V2_H_
#define ACC_LIBGPIOD_V2_H_

#include <stdbool.h>
#include <stdint.h>

#include "acc_libgpiod.h"

const char **acc_libgpiod_v2_get_symbols(uint32_t *nr_of_symbols);
bool         acc_libgpiod_v2_init(acc_libgpiod_symbol_t *symbols, struct gpiod_chip **chip, const gpio_config_t *pin_config);
void         acc_libgpiod_v2_deinit(acc_libgpiod_symbol_t *symbols, struct gpiod_chip *chip);
bool         acc_libgpiod_v2_set(acc_libgpiod_symbol_t *symbols, int pin, gpio_pin_value_t value);
bool         acc_libgpiod_v2_wait_for_interrupt(acc_libgpiod_symbol_t *symbols, int pin, uint32_t timeout_ms);

#endif
