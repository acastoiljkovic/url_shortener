#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <syslog.h>
#include "logger.h"

static FILE *log_file = NULL;

void logger_init(const char *logfile) {
    log_file = fopen(logfile, "a");
    openlog("url_shortener", LOG_PID | LOG_PERROR, LOG_USER);
}

void logger_close() {
    if (log_file) fclose(log_file);
    closelog();
}

void log_message(int level, const char *format, ...) {
    va_list args;
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_buffer[64];

    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", tm_info);

    va_start(args, format);
    vsyslog(level, format, args);
    va_end(args);

    if (log_file) {
        fprintf(log_file, "[%s] ", time_buffer);
        va_start(args, format);
        vfprintf(log_file, format, args);
        fprintf(log_file, "\n");
        fflush(log_file);
        va_end(args);
    }
}
