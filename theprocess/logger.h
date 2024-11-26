#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"

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
    snprintf(buffer, sizeof(buffer), "%s :: %s", level, message);

    // ERROR 로그의 경우 perror 사용
    if (strcmp(level, LEVEL_ERROR) == 0) {
        char buffer2[TEMP_SIZE];
        vsnprintf(buffer2, sizeof(buffer2), buffer, args);
        perror(buffer2);
    } else {
        // INFO 및 WARN 로그의 경우 vprintf 사용
        vprintf(buffer, args);
    }
}