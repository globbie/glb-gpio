#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include <linux/gpio.h>

#include "gpio.h"


static gpioError_t
set(struct gpioController *ctl, size_t argc, const char **argv)
{
    if (argc != 4) {
        fprintf(stderr, "'set' command required exactly three arguments:\n"
                "\t set <chip name> <line number> <value>\n");
        return gpio_invalid;
    }

    const char *chip_name = argv[1];
    const char *line_number_ = argv[2];
    const char *value_ = argv[3];

    size_t line_number = 0;
    unsigned int value = 0;

    struct gpioChip *chip = NULL;
    struct gpioLine *line = NULL;

    gpioError_t err;

    // todo: use proper functions, check limits
    line_number = strtoul(line_number_, NULL, 10);
    value = strtoul(value_, NULL, 10);

    err = ctl->get_chip(ctl, chip_name, &chip);
    if (err != gpio_ok) {
        fprintf(stderr, "chip '%s', error: '%s'\n", chip_name, err->string);
        return err;
    }

    err = chip->get_line(chip, line_number, &line);
    if (err != gpio_ok) {
        fprintf(stderr, "chip '%s' line '%zu', error '%s'\n", chip_name, line_number, err->string);
        return err;
    }

    return gpio_ok;
}

int main(int argc __attribute__((unused)), char **argv __attribute__((unused)))
{
    struct gpioController *ctl;
    gpioError_t err;

    err = gpioController_new(&ctl);
    if (err != gpio_ok) {
        fprintf(stderr, "gpioController_new() failed, error: %s\n", err->string);
        return EXIT_FAILURE;
    }

    char *line = NULL;
    size_t line_cap = 0;
    ssize_t line_len;

    for (;;) {
        printf(">>> ");

        line_len = getline(&line, &line_cap, stdin);
        if (line_len < 0) {
            printf("\n");
            break;
        }

        const char *cmd_args[10];
        size_t cmd_args_count = 0;
        const char *token;

        while ((token = strsep(&line, " \t\n")) != NULL) {
            if (*token == '\0') continue;
            //printf("- '%s'\n", token);
            cmd_args[cmd_args_count] = token;
            ++cmd_args_count;
            if (cmd_args_count == 10) break;
        }

        if (cmd_args_count == 0) continue;

        if (strcmp(cmd_args[0], "set") == 0) {
            err = set(ctl, cmd_args_count, cmd_args);
            if (err == gpio_ok) goto exit;
        }
    }

exit:

    ctl->del(ctl);
    return EXIT_SUCCESS;
}

