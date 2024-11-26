#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("Process started: %s\n", argv[0]);

    while (1) {
        sleep(5);  // 실행 유지
    }
    return 0;
}
