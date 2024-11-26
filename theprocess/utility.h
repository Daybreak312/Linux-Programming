#include <stdio.h>
#include <string.h>
#include <ctype.h>

// trim 함수: 문자열 앞뒤 공백 제거
char *trim(char *str) {
    // 앞 공백 제거
    while (isspace((unsigned char) *str)) {
        str++;
    }

    // 빈 문자열인 경우
    if (*str == '\0') {
        return str;
    }

    // 뒤 공백 제거
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char) *end)) {
        end--;
    }

    // 끝에 널 문자 추가
    *(end + 1) = '\0';

    return str;
}
