#include <unistd.h>
#include <stdio.h>

int main() {

	char *arg[] = {"ls", "-l", (char *) 0};
	printf("before executing ls -l\n");
	execl("/bin/ls", &arg);
	// execl("/bin/ls", "ls", "-l", (char*)0);
	printf("after executing ls -l\n");
}
