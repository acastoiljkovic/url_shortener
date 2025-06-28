#ifndef LOGGER_H
#define LOGGER_H

void logger_init(const char *logfile);
void logger_close();
void log_message(int level, const char *format, ...);

#endif
