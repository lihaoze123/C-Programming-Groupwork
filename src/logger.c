#include "logger.h"

static FILE* stream = NULL;

static const char* level_str[] = {
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR"
};

FILE* init_logger(const char *filename) {
    if (filename == NULL || strcmp(filename, "") == 0) {
        stream = stderr;
    } else {
        stream = fopen(filename, "a");
    }
    return stream;
}

void log_message(LogLevel level, const char* format, ...) {
    if (!stream) {
        return;
    }

    time_t now = time(NULL);
    char time_str[26];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

    fprintf(stream, "%s [%s] ", time_str, level_str[level]);

    va_list args;
    va_start(args, format);
    vfprintf(stream, format, args);
    va_end(args);

    fprintf(stream, "\n");
    fflush(stream);
}

void close_logger() {
    fclose(stream);
}