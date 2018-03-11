#include "gpio-error.h"

#define XX(code__, name__, string__) gpioError_t gpio_##name__ = { \
                                            .code = code__, \
                                            .string = string__, \
                                            .string_len = sizeof(string__) - 1 \
                                     };
GPIO_ERRORS_MAP(XX)
#undef XX
