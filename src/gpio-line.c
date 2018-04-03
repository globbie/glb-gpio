#include "gpio-line.h"
#include "gpio-error.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>


static void print_flags(unsigned long flags)
{
    printf("\t\tkern: %u, out: %u, a-l: %u, o-d: %u, o-s: %u\n",
            flags & GPIOLINE_FLAG_KERNEL ? 1 : 0,
            flags & GPIOLINE_FLAG_IS_OUT ? 1 : 0,
            flags & GPIOLINE_FLAG_ACTIVE_LOW ? 1 : 0,
            flags & GPIOLINE_FLAG_OPEN_DRAIN ? 1 : 0,
            flags & GPIOLINE_FLAG_OPEN_SOURCE ? 1 : 0);
}

gpioError_t gpioLine_init(struct gpioLine *self, size_t line_offset, int fd)
{
    int ret;

    memset(self, 0, sizeof(*self));
    self->info.line_offset = line_offset;

    ret = ioctl(fd, GPIO_GET_LINEINFO_IOCTL, &self->info);
    if (ret == -1) return gpio_system;

    printf("\tline: '%u', name: '%s', consumer: '%s'\n",
            self->info.line_offset, self->info.name, self->info.consumer);

    print_flags(self->info.flags);

    return gpio_ok;
}
