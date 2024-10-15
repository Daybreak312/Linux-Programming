#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

int handled = 0;

void handle(int signum) {
    if (signum == SIGINT) {
        printf("[parent] Got signal.\n");
        handled = 1;
    }
}

int main() {
    pid_t pid;
    int count = 5;

    static struct sigaction act;

    act.sa_handler = handle;
    sigfillset(&(act.sa_mask));

    if ((pid = fork()) > 0) {

        while (!handled) {
            sleep(1);
        }

        printf("[parent] end..\n");
    } else if (pid == 0) {
        while (count) {
            printf("[child] count is %d\n", count--);
            sleep(1);
        }
        printf("[child] initializing end\n");
        kill(getppid(), SIGINT);
    } else {
        printf("failed to fork.\n");
    }
}
