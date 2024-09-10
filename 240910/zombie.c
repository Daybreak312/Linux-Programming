#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {

	pid_t pid;
	int status;

	pid = fork();

	if (pid > 0) {
	
		printf("parent\n");
	} else if (pid == 0) {
	
		sleep(3);
		printf("child\n");
	} else {
	
		printf("fail to fork\n");
	}

	printf("bye!\n");

	return 0;
}
