#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("Process started: %s\n", argv[0]);

    pid_t parentPid = getppid();

    while (1) {
        if (kill(parentPid, 0) == -1) {
            printf("Parent process (PID: %d) has terminated. Exiting child process.\n", (int) parentPid);
            exit(EXIT_SUCCESS);
        }
        sleep(10);
    }
}
