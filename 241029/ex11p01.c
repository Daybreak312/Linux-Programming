#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

#define SIZE 512

int main() {
    pid_t pid1, pid2;

    char buffer[SIZE];
    int filedes1[2], filedes2[2];

    if (pipe(filedes1) == -1) {
        printf("call pipe failed #1\n");
        exit(1);
    }

    if ((pid1 = fork()) > 0) {

        filedes1[1] = 0;

        sleep(5);

        read(filedes1[0], buffer, SIZE);

        printf("[first] read finish!: %s\n", buffer);

    } else if (pid1 == 0) {

        if (pipe(filedes2) == -1) {
            printf("call pipe failed #2\n");
            exit(1);
        }

        if ((pid2 = fork()) > 0) {

            filedes2[1] = 0;

            sleep(5);

            read(filedes2[0], buffer, SIZE);

            printf("[second] read finish!: %s\n", buffer);

        } else if (pid2 == 0) {

            filedes1[0] = filedes2[0] = 0;

            strcpy(buffer, "wow i'm hacker!!\n");

            write(filedes1[1], buffer, SIZE);
            write(filedes2[1], buffer, SIZE);

            printf("[third] write finish. bye!\n");

        } else {
            printf("fork failed: #2\n");
        }

    } else {
        printf("fork failed: #1\n");
    }
}
