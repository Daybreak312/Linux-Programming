#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() {

	int fdin, fdout;
	ssize_t nread;
	char buffer[1024];

	if ((fdin = open("temp1.txt", O_RDONLY)) == -1) {
		printf("file open error : temp1\n");
		exit(1);
	}
	if ((fdout = open("temp2.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {
		printf("file open error : temp2\n");
		exit(1);
	}

	while ((nread = read(fdin, buffer, 1024)) > 0) {
		
		if (write(fdout, buffer, nread) < nread) {
			close(fdin);
			close(fdout);
		}
	}

	close(fdin);
	close(fdout);

	return 0;
}
