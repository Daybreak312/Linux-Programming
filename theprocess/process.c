#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    printf("Process started: %s\n", argv[0]);

    int status;
    waitpid(getppid(), &status);

    return 0;
}
