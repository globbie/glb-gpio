#include "gpio-chip.h"

#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

gpioError_t
gpioChip_init(struct gpioChip *self, const char *dev_path, size_t dev_path_len)
{
    char c_dev_path[PATH_MAX];
    int fd;
    int ret;
    gpioError_t err = gpio_fail;

    memset(self, 0, sizeof(*self));

    if (dev_path_len >= PATH_MAX) return gpio_trunc;
    memcpy(c_dev_path, dev_path, dev_path_len);
    c_dev_path[dev_path_len] = '\0';

    fd = open(c_dev_path, 0);
    if (fd == -1) return gpio_system;

    ret = ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &self->info);
    if (ret == -1) {
        err = gpio_system;
        goto exit;
    }

    printf("gpio chip: '%s', label: '%s', lines: '%u'\n",
            self->info.name, self->info.label, self->info.lines);

    err = gpio_ok;
exit:
    return err;
}
