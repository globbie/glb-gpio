#pragma once

#include <linux/gpio.h>

#include "gpio-line.h"
#include "gpio-error.h"
#include "defines.h"

#define GPIO_CHIP_LINES_COUNT_MAX 128

struct gpioChip
{
    struct gpiochip_info info;

    struct gpioLine lines[GPIO_CHIP_LINES_COUNT_MAX];
    size_t lines_count;

    char device_path[PATH_MAX];

    // public methods
    gpioError_t (*destroy)(struct gpioChip *self);
    gpioError_t (*get_line)(struct gpioChip *self, size_t line_number, struct gpioLine **line);
};
gpioError_t gpioChip_init(struct gpioChip *self, const char *dev_path, size_t dev_path_len);

