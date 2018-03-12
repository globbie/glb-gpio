#include "gpio-controller.h"

#include <stdbool.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

static gpioError_t
new_device(struct gpioController *self, const char *dev_path, size_t dev_path_len, struct gpioChip **chip)
{
    return gpio_fail;
}

static gpioError_t
find_device(struct gpioController *self, const char *dev_path, size_t dev_path_len, struct gpioChip **chip)
{
    return gpio_notfound;
}

static bool
prefix_matches(const char *str, size_t str_len, const char *prefix, size_t prefix_len)
{
    if (str_len <= prefix_len) return false;
    if (strncmp(str, prefix, prefix_len) == 0) return true;
    return false;
}

static gpioError_t
discover(struct gpioController *self)
{
    const struct dirent *entry;
    DIR *dir;
    gpioError_t err = gpio_fail;

    dir = opendir("/dev");
    if (!dir) {
       snprintf(self->error_message, GPIO_ERROR_MESSAGE_SIZE,
               "opendir() failed, error: '%s'", strerror(errno));
       return gpio_fail;
    }

    while ((entry = readdir(dir))) {
        char device_path[PATH_MAX] = "/dev/";
        size_t device_path_len = sizeof("/dev/") - 1;

        if (prefix_matches(entry->d_name, strlen(entry->d_name), "gpiochip", sizeof("gpiochip") - 1)) {
            size_t device_name_len = strlen(entry->d_name);
            struct gpioChip *chip;

            printf("found gpio chip: '%s'\n", entry->d_name);

            if (device_path_len > sizeof(device_path) - device_path_len) {
                err = gpio_trunc;
                goto exit;
            }
            strncat(device_path, entry->d_name, device_name_len);

            err = find_device(self, device_path, device_path_len, &chip);
            if (err == gpio_ok) {
                continue;
            } else if (err == gpio_notfound) {

                err = new_device(self, device_path, device_path_len, &chip);
                if (err != gpio_ok) goto exit;

            } else {
                goto exit;
            }
        }
    }

    err = gpio_ok;
exit:
    closedir(dir);
    return err;
}

static gpioError_t
delete__(struct gpioController *self)
{
    free(self);
    return gpio_ok;
}

gpioError_t
gpioController_new(struct gpioController **controller)
{
    struct gpioController *self;
    gpioError_t err = gpio_fail;

    self = calloc(1, sizeof(*self));
    if (!self) return gpio_nomem;

    self->del = delete__;
    self->discover = discover;

    err = self->discover(self);
    if (err != gpio_ok) goto error;

    *controller = self;
    return gpio_ok;
error:
    delete__(self);
    return err;
}
