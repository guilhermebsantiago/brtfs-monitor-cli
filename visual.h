#ifndef VISUAL_H
#define VISUAL_H

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_BLUE    "\033[1;34m"

void print_usage_bar_colored(int percent, const char *color);

#endif
