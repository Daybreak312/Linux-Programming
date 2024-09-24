#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

void main() {

	printf("[ex0806.c] PPID: %d, PID: %d\n", getppid(), getpid());
}
