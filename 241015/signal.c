#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

void handle(int signum) {
    int pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("SIGCHLD, %d %d\n", pid, signum);
        if (WIFEXITED(status)) {
            printf("exit %d\n", WEXITSTATUS(status));
        }
        if (WIFSIGNALED(status)) {
            printf("signal %d]n", WTERMSIG(status));
        }
    }
}

int flag = 5;

int main() {

    struct sigaction act;
    sigset_t set;

    sigfillset(&(act.sa_mask));
    act.sa_handler = handle;

    pid_t pid = fork();

    if (pid == 0) {
        sleep(2);
        // kill(getpid(), 9);
        exit(10);
    } else {
        for (int i = 0; i < 5; i++) {
            sleep(1);
        }
    }
}
