#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>

void main() {

	pid_t pid;

	if ((pid = fork()) > 0) {
		sleep(1);
	} else if (pid == 0) {
		
		printf("Time printer daemon started with pid: %d\n", setsid());

		while(1) {
			time_t t = time(NULL);
			printf("Current time: %s\n", ctime(&t));
			sleep(10);
		}
	} else {
		printf("Fork failed.\n");
	}

}
