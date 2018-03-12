#include "gpio-error.h"

#define XX(code__, name__, string__) gpioError GPIO_##name__ = { \
                                            .code = code__, \
                                            .string = string__, \
                                            .string_len = sizeof(string__) - 1 \
                                     };
GPIO_ERRORS_MAP(XX)
#undef XX

#define XX(code__, name__, string__) gpioError_t gpio_##name__ = &GPIO_##name__;
GPIO_ERRORS_MAP(XX)
#undef XX

