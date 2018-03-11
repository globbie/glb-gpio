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

    dir = opendir("/dev");
    if (!dir) {
       snprintf(self->error_message, GPIO_ERROR_MESSAGE_SIZE,
               "opendir() failed, error: '%s'", strerror(errno));
       return gpio_fail;
    }

    while ((entry = readdir(dir))) {
        if (prefix_matches(entry->d_name, strlen(entry->d_name),
                           "gpiochip", sizeof("gpiochip") - 1)) {
            printf("found gpio chip: '%s'\n", entry->d_name);
        }
    }

    closedir(dir);

    return gpio_ok;
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

    self = calloc(1, sizeof(*self));
    if (!self) return gpio_fail;

    self->del = delete__;

    return gpio_ok;
}
