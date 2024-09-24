#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

void main() {

	pid_t pid;

	if ((pid = fork()) > 0) {
		printf("[ex0805.c] PPID: %d, PID: %d\n", getppid(), getpid());
		sleep(1);
	} else if (pid == 0) {
		printf("[ex0805.c] PPID: %d, PID: %d\n", getppid(), getpid());
		execl("0806", "0806", (char *)0);
	} else {
		printf("fail to fork\n");
	}
}
