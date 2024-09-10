#include "stdio.h"
#include "sys/types.h"
#include "unistd.h"
#include "stdlib.h"
#include "sys/wait.h"

int main() {

    int x, y;

    scanf("%d %d", &x, &y);

    if (x > y || x > 10 || x < 0 || y > 10 || y < 0) {
        printf("input error\n");
    }

    pid_t pid;

    pid = fork();

    if (pid > 0) {
        int result = x;

        for (int i = 1; i < y; i++) {
            result *= x;
        }

        int child;

        wait(&child);

        printf("result: %d\n", result + (child >> 8));
    } else if (pid == 0) {
        int result = 0;

        for (int i = x; i <= y; i++) {
            result += i;
        }

        exit(result);
    } else {
        printf("fail to fork\n");
    }
}
