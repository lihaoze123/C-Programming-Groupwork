#include "color.h"

void init_console() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
}

void set_text_color(color_t color) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
#else
    printf("%s", color);
#endif
}

void reset_text_color() {
    set_text_color(COLOR_ORIGINAL);
}

int cprintf(color_t color, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    set_text_color(color);
    int result = vprintf(format, args);
    reset_text_color();
    
    va_end(args);
    return result;
} 