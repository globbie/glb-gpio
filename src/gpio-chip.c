#include "gpio-chip.h"

gpioError_t
gpioChip_init(struct gpioChip *self, const char *dev_path, size_t dev_path_len)
{
    return gpio_fail;
}
