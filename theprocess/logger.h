#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "unistd.h"
#include "fcntl.h"

#define LOG_FILE "./log/log.txt"

#define TEMP_SIZE 256
#define LEVEL_ERROR "ERROR"
#define LEVEL_WARN "WARN"
#define LEVEL_INFO "INFO"

// 로그 함수들 선언
void info(char *message, ...);

void error(char *message, ...);

void warn(char *message, ...);

void logging(char *level, char *message, va_list args);

// info 로그 함수
void info(char *message, ...) {
    va_list args;
    va_start(args, message);
    logging(LEVEL_INFO, message, args);
    va_end(args);
}

// error 로그 함수
void error(char *message, ...) {
    va_list args;
    va_start(args, message);
    logging(LEVEL_ERROR, message, args);
    va_end(args);
}

// warn 로그 함수
void warn(char *message, ...) {
    va_list args;
    va_start(args, message);
    logging(LEVEL_WARN, message, args);
    va_end(args);
}

// 로그를 출력하는 함수
void logging(char *level, char *message, va_list args) {
    char buffer[TEMP_SIZE];
    // 로그 수준과 메시지를 결합하여 포맷
    snprintf(buffer, TEMP_SIZE, "%s :: %s\n", level, message);

    int fd;
    if ((fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0666)) < 0) {
        perror("Failed to open log file");
        return;
    }

    char logMessage[TEMP_SIZE];

    vsnprintf(logMessage, TEMP_SIZE, buffer, args);

    if ((write(fd, logMessage, strlen(logMessage))) < 0) {
        perror("Error writing to log file");
    }

    close(fd);
}