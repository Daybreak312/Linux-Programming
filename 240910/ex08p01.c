#include "stdio.h"
#include "sys/types.h"
#include "unistd.h"
#include "stdlib.h"
#include "math.h"

int main() {

    int x, y;

    scanf("%d %d", &x, &y);

    if (x > y || x > 10 || x < 0 || y > 10 || y < 0) {
        printf("input error\n");
    }

    pid_t pid;

    pid = fork();

    if (pid > 0) {
        int result = (int) pow((double) x, (double) y);
        int child;

        wait(&child);

        printf("result: %d", result + child);
    }
}
