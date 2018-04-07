#include "gpio-chip.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>


static gpioError_t
get_line(struct gpioChip *self, size_t line_number, struct gpioLine **line)
{
    return gpio_fail;
}

static gpioError_t
destroy__(struct gpioChip *self __attribute__((unused)))
{
    // todo
    return gpio_ok;
}

gpioError_t
gpioChip_init(struct gpioChip *self, const char *dev_path, size_t dev_path_len)
{
    int fd;
    int ret;
    gpioError_t err = gpio_fail;

    memset(self, 0, sizeof(*self));

    if (dev_path_len >= PATH_MAX) return gpio_trunc;
    memcpy(self->device_path, dev_path, dev_path_len);
    self->device_path[dev_path_len] = '\0';

    fd = open(self->device_path, 0);
    if (fd == -1) return gpio_system;

    ret = ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &self->info);
    if (ret == -1) {
        err = gpio_system;
        goto exit;
    }

    printf("gpio chip: '%s', label: '%s', lines: '%u'\n",
            self->info.name, self->info.label, self->info.lines);

    self->lines_count = self->info.lines;
    self->destroy = destroy__;

    for (size_t i = 0; i < self->lines_count; ++i) {
        err = gpioLine_init(&self->lines[i], i, fd);
        if (err != gpio_ok) goto exit;
    }

    err = gpio_ok;
exit:
    close(fd);
    return err;
}
