#pragma once

#include <stdlib.h>

typedef struct {
    int code;
    const char *string;
    size_t string_len;
} gpioError_t ;

#define GPIO_ERRORS_MAP(XX) \
    XX(0, ok, "ok") \
    XX(1, fail, "fail")

#define XX(code__, name__, string__) extern gpioError_t gpio_##name__;
    GPIO_ERRORS_MAP(XX)
#undef XX

