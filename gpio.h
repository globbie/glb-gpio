#pragma once

#include <linux/gpio.h>

typedef struct {
    int code;
    const char *string;
} gpioError_t ;


#define GPIO_ERRORS_MAP(XX) \
    XX(0, ok, "ok") \
    XX(1, fail, "fail") \
    XX(1, nomem, "no memory")

#define XX(code__, name__, string__) gpioError_t gpio_##name__ = { .code = code__, .string = string__ };
    GPIO_ERRORS_MAP(XX)
#undef XX

struct gpioLine
{
    struct gpioline_info info;
};

struct gpioChip
{
    struct gpiochip_info info;

    struct gpioLine lines[];
    size_t lines_count;
};

struct gpioController
{
    struct gpioChip chips[];
    int chips_count;
};

gpioError_t gpioController_new(void);
