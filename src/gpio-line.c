#include "gpio-line.h"
#include "gpio-error.h"

gpioError_t gpioLine_init(struct gpioLine *self)
{
    (void) self;
    return gpio_fail;
}
