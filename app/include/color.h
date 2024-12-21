#ifndef COLOR_H
#define COLOR_H

#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdarg.h>

#ifdef _WIN32
    #define COLOR_ORIGINAL (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
    #define COLOR_RED     (FOREGROUND_RED | FOREGROUND_INTENSITY)
    #define COLOR_GREEN   (FOREGROUND_GREEN | FOREGROUND_INTENSITY)
    #define COLOR_BLUE    (FOREGROUND_BLUE | FOREGROUND_INTENSITY)
    #define COLOR_YELLOW  (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
    #define COLOR_CYAN    (FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
    #define COLOR_WHITE   (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY)
    typedef WORD color_t;
#else
    #define COLOR_ORIGINAL "\033[0m"
    #define COLOR_RED     "\033[1;31m"
    #define COLOR_GREEN   "\033[1;32m"
    #define COLOR_BLUE    "\033[1;34m"
    #define COLOR_YELLOW  "\033[1;33m"
    #define COLOR_CYAN    "\033[1;36m"
    #define COLOR_WHITE   "\033[1;37m"
    typedef const char* color_t;
#endif

void init_console();
void set_text_color(color_t color);
void reset_text_color();

int cprint(color_t color, const char* format, ...);

#endif // COLOR_H 