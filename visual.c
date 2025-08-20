#include <stdio.h>
#include "visual.h"

void print_usage_bar_colored(int percent, const char *color) {
    int blocks = 30;
    int filled = (percent * blocks) / 100;

    printf("%s[", color);

    for (int i = 0; i < blocks; i++) {
        if (i < filled)
            printf("█");
        else
            printf(" ");
    }
    printf("] %d%%%s\n", percent, COLOR_RESET);
}
