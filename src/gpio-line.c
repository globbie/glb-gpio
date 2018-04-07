#include "gpio-line.h"
#include "gpio-error.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>


static void
print_flags(unsigned long flags)
{
    printf("\t\tkern: %u, out: %u, active-low: %u, open-drain: %u, open-source: %u\n",
            flags & GPIOLINE_FLAG_KERNEL ? 1 : 0,
            flags & GPIOLINE_FLAG_IS_OUT ? 1 : 0,
            flags & GPIOLINE_FLAG_ACTIVE_LOW ? 1 : 0,
            flags & GPIOLINE_FLAG_OPEN_DRAIN ? 1 : 0,
            flags & GPIOLINE_FLAG_OPEN_SOURCE ? 1 : 0);
}

static gpioError_t
set__(struct gpioLine *self, unsigned int value)
{
    struct gpiohandle_request req;
    int ret;

    req.flags = GPIOHANDLE_REQUEST_OUTPUT;
    req.lines = 1;
    req.lineoffsets[0] = self->info.line_offset;
    req.default_values[0] = value;
    strcpy(req.consumer_label, "globbie");

    ret = ioctl(self->fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
    if (ret == -1) return gpio_system;

    return gpio_ok;
}

static gpioError_t
get__(struct gpioLine *self, unsigned int *value)
{
    (void) self, (void) value;
    return gpio_ok;
}

static gpioError_t
destroy__(struct gpioLine *self __attribute__((unused)))
{
    return gpio_ok;
}

gpioError_t
gpioLine_init(struct gpioLine *self, size_t line_offset, int fd)
{
    int ret;

    memset(self, 0, sizeof(*self));
    self->info.line_offset = line_offset;

    ret = ioctl(fd, GPIO_GET_LINEINFO_IOCTL, &self->info);
    if (ret == -1) return gpio_system;

    self->fd = fd;

    self->set = set__;
    self->get = get__;
    self->destroy = destroy__;

    printf("\tline: '%u', name: '%s', consumer: '%s'\n",
            self->info.line_offset, self->info.name, self->info.consumer);
    print_flags(self->info.flags);

    return gpio_ok;
}

