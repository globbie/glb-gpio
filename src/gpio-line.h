#pragma once

#include <linux/gpio.h>

#include "gpio-error.h"

struct gpioLine
{
    struct gpioline_info info;
    int fd;

    // public methods
    gpioError_t (*set)(struct gpioLine *self, unsigned int value);
    gpioError_t (*get)(struct gpioLine *self, unsigned int *value);
    gpioError_t (*destroy)(struct gpioLine *self);
};

gpioError_t gpioLine_init(struct gpioLine *self, size_t line_offset, int fd);

