#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>

int main() {
    sigset_t set;
    int count = 3;

    sigemptyset($set);
    // sigaddset(&set, SiGINT);
    sigfillset(&set);

    sigprocmask(SIG_BLOCK, &set, NULL);

    while (count) {
        printf("Don't disturb me (%d)\n", count--);
        sleep(1);
    }

    printf("Disturb me\n");

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigprocmask(SIG_UNBLOCK, &set, NULL);
}

