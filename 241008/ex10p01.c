#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

void handle(int sigNum) {
    int file = open("text.txt", O_WRONLY);
    char buffer[100] = "interrupted.";

    write(file, buffer, 100);
    close(file);

    exit(0);
}

int main() {
    static struct sigaction act;

    act.sa_handler = handle;
    sigfillset(&(act.sa_mask));
    sigaction(SIGINT, &act, NULL);
}
