#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include <linux/gpio.h>

// see original 'https://github.com/torvalds/linux/blob/master/tools/gpio/lsgpio.c' by Linus Walleij

struct gpio_flag {
    char *name;
    unsigned long mask;
};

struct gpio_flag flagnames[] = {
    {
        .name = "kernel",
        .mask = GPIOLINE_FLAG_KERNEL,
    },
    {
        .name = "output",
        .mask = GPIOLINE_FLAG_IS_OUT,
    },
    {
        .name = "active-low",
        .mask = GPIOLINE_FLAG_ACTIVE_LOW,
    },
    {
        .name = "open-drain",
        .mask = GPIOLINE_FLAG_OPEN_DRAIN,
    },
    {
        .name = "open-source",
        .mask = GPIOLINE_FLAG_OPEN_SOURCE,
    },
};

void print_flags(unsigned long flags)
{
    int i;
    int printed = 0;

    for (i = 0; i < sizeof(flagnames) / sizeof((flagnames)[0]); i++) {
        if (flags & flagnames[i].mask) {
            if (printed)
                fprintf(stdout, " ");
            fprintf(stdout, "%s", flagnames[i].name);
            printed++;
        }
    }
}

static bool
is_prefix(const char *str, size_t str_len, const char *prefix, size_t prefix_len)
{
    if (str_len <= prefix_len) return false;
    if (strncmp(str, prefix, prefix_len) == 0) return true;
    return false;
}

static int
show_device(const char *device_name, size_t device_name_len)
{
    struct gpiochip_info chip_info;

    char chrdev_name[PATH_MAX] = "/dev/";
    char chrdev_name_len = sizeof("/dev/");

    int fd;
    int err;
    int ret = -1;

    if (device_name_len > sizeof(chrdev_name) - chrdev_name_len) {
        fprintf(stderr, "chrdev_name truncated\n");
        return -1;
    }

    strncat(chrdev_name, device_name, device_name_len);
    printf("debug: chrdev_name: '%s'\n", chrdev_name);

    fd = open(chrdev_name, 0);
    if (fd == -1) {
        fprintf(stderr, "open('%s') failed, error: '%s'\n", chrdev_name, strerror(errno));
        return -1;
    }

    err = ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &chip_info);
    if (err == -1) {
        perror("CHIPINFO IOCTL call failed");
        goto exit;
    }

    printf("gpio chip: %s, '%s', %u gpio lines\n", chip_info.name, chip_info.label, chip_info.lines);

    for (size_t i = 0; i < chip_info.lines; ++i) {
        struct gpioline_info line_info;
        memset(&line_info, 0, sizeof(line_info));

        line_info.line_offset = i;

        err = ioctl(fd, GPIO_GET_LINEINFO_IOCTL, &line_info);
        if (err == -1) {
            perror("LINEINGO IOCTL call failed");
            goto exit;
        }

        printf("\tline %2d:", line_info.line_offset);

        if (line_info.name[0])
            printf(" '%s'", line_info.name);
        else
            printf(" unnamed");

        if (line_info.consumer[0])
            printf(" '%s'", line_info.consumer);
        else
            printf(" unused");

        if (line_info.flags) {
            printf(" [");
            print_flags(line_info.flags);
            printf("]");
        }

        printf("\n");
    }

    ret = 0;
exit:
    close(fd);
    return ret;
}

int main(int argc, char **argv)
{
    const struct dirent *entry;
    DIR *dir;

    dir = opendir("/dev");
    if (!dir) {
        perror("opendir failed");
        goto error;
    }

    while ((entry = readdir(dir))) {
        if (is_prefix(entry->d_name, strlen(entry->d_name), "gpiochip", sizeof("gpiochip") - 1)) {
            printf("found gpio chip: '%s'\n", entry->d_name);
            show_device(entry->d_name, strlen(entry->d_name));
        }
    }

    closedir(dir);
    return EXIT_SUCCESS;
error:
    return EXIT_FAILURE;
}
