#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {

	pid_t pid;
	int input;

	scanf("%d", &input);

	pid = fork();

	if (pid > 0) {

		int result = 0;

		for (int i = 1; i <= input; i++) {
	
			result += i;
		}

		printf("parent result: %d\n", result);

	} else if (pid == 0) {
		int result = 1;
	
		for (int i = 1; i <= input; i++) {
			result *= i;
		}
		
		printf("child result: %d\n", result);
	} else {
	
		printf("fail to fork\n");
	}
}
