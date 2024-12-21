#include "color.h"

void init_console() {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
}

void set_text_color(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void reset_text_color() {
    set_text_color(COLOR_ORIGINAL);
}

int cprint(WORD color, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    set_text_color(color);
    int result = vprintf(format, args);
    reset_text_color();
    
    va_end(args);
    return result;
} 