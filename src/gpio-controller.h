#pragma once

#include "gpio-chip.h"
#include "gpio-error.h"

#include <stdlib.h>

#define GPIO_CHIPS_COUNT_MAX 8
#define GPIO_ERROR_MESSAGE_SIZE 1024

struct gpioController
{
    struct gpioChip chips[GPIO_CHIPS_COUNT_MAX];
    size_t chips_count;

    char error_message[GPIO_ERROR_MESSAGE_SIZE];

    // public methods
    gpioError_t (*del)(struct gpioController *self);
    gpioError_t (*discover)(struct gpioController *self);
};

gpioError_t gpioController_new(struct gpioController **controller);

