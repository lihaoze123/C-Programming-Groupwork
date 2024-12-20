#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
typedef enum {
    LOG_DEBUG,  
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} LogLevel;

void set_log_level(LogLevel level);

FILE* init_logger(const char *filename);

void log_message(LogLevel level, const char* format, ...);

void close_logger();

#endif