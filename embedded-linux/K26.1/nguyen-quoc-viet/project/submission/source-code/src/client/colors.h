#ifndef COLORS_H
#define COLORS_H

/* ANSI Color Codes */
#define COLOR_RESET     "\x1b[0m"
#define COLOR_BOLD      "\x1b[1m"
#define COLOR_DIM       "\x1b[2m"

/* Foreground Colors */
#define COLOR_BLACK     "\x1b[30m"
#define COLOR_RED       "\x1b[31m"
#define COLOR_GREEN     "\x1b[32m"
#define COLOR_YELLOW    "\x1b[33m"
#define COLOR_BLUE      "\x1b[34m"
#define COLOR_MAGENTA   "\x1b[35m"
#define COLOR_CYAN      "\x1b[36m"
#define COLOR_WHITE     "\x1b[37m"

/* Bright Colors */
#define COLOR_BRIGHT_BLACK   "\x1b[90m"
#define COLOR_BRIGHT_RED     "\x1b[91m"
#define COLOR_BRIGHT_GREEN   "\x1b[92m"
#define COLOR_BRIGHT_YELLOW  "\x1b[93m"
#define COLOR_BRIGHT_BLUE    "\x1b[94m"
#define COLOR_BRIGHT_MAGENTA "\x1b[95m"
#define COLOR_BRIGHT_CYAN    "\x1b[96m"
#define COLOR_BRIGHT_WHITE   "\x1b[97m"

/* Combined Styles */
#define SUCCESS  COLOR_GREEN COLOR_BOLD
#define ERROR    COLOR_RED COLOR_BOLD
#define INFO     COLOR_CYAN
#define WARNING  COLOR_YELLOW COLOR_BOLD
#define MUTED    COLOR_BRIGHT_BLACK

/* Utility Macros */
#define PRINT_SUCCESS(msg)  printf(SUCCESS "✓ " COLOR_RESET "%s\n", msg)
#define PRINT_ERROR(msg)    printf(ERROR "✗ " COLOR_RESET "%s\n", msg)
#define PRINT_INFO(msg)     printf(INFO "ℹ " COLOR_RESET "%s\n", msg)
#define PRINT_WARNING(msg)  printf(WARNING "⚠ " COLOR_RESET "%s\n", msg)

#endif
