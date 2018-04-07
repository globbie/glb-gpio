#pragma once

#include <stdlib.h>

typedef struct {
    int code;
    const char *string;
    size_t string_len;
} gpioError, *gpioError_t;


#define GPIO_ERRORS_MAP(XX) \
    XX(0, ok,       "ok") \
    XX(1, fail,     "unspecified failure") \
    XX(2, nomem,    "could not allocate memory") \
    XX(3, trunc,    "string was truncated") \
    XX(4, notfound, "item was not found") \
    XX(5, limit,    "limit was reached") \
    XX(6, system,   "system call failure") \
    XX(7, invalid,  "invalid argument")

#define XX(code__, name__, string__) extern gpioError GPIO_##name__;
    GPIO_ERRORS_MAP(XX)
#undef XX

#define XX(code__, name__, string__) extern gpioError_t gpio_##name__;
    GPIO_ERRORS_MAP(XX)
#undef XX

