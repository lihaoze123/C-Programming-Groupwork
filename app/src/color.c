#include "color.h"
#include <logger.h>

void init_console() {
    log_message(LOG_INFO, "开始初始化控制台");
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    log_message(LOG_DEBUG, "设置Windows控制台代码页为UTF-8");
#endif
    log_message(LOG_INFO, "控制台初始化完成");
}

void set_text_color(color_t color) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) {
        log_message(LOG_ERROR, "设置文本颜色失败：无法获取控制台句柄");
        return;
    }
    if (!SetConsoleTextAttribute(hConsole, color)) {
        log_message(LOG_ERROR, "设置文本颜色失败：SetConsoleTextAttribute返回错误");
        return;
    }
    log_message(LOG_DEBUG, "设置Windows控制台文本颜色 [颜色:%d]", color);
#else
    printf("%s", color);
    log_message(LOG_DEBUG, "设置Unix控制台文本颜色");
#endif
}

void reset_text_color() {
    log_message(LOG_DEBUG, "重置文本颜色");
    set_text_color(COLOR_ORIGINAL);
}

int cprintf(color_t color, const char* format, ...) {
    if (!format) {
        log_message(LOG_ERROR, "彩色打印失败：格式字符串为NULL");
        return -1;
    }

    va_list args;
    va_start(args, format);
    
    set_text_color(color);
    int result = vprintf(format, args);
    if (result < 0) {
        log_message(LOG_ERROR, "彩色打印失败：vprintf返回错误");
    }
    reset_text_color();
    
    va_end(args);
    return result;
} 