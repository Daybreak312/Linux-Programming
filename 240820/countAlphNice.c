#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int isSmallAlphabet(int asc) {
	
	if (97 <= asc && asc <= 122) {
		return 1;
	}
}

int main() {
	
	int file, fileSize, count = 0;
	char buffer[1024];

	if ((file = open("temp2.txt", O_RDWR)) == -1) {
		printf("Oh no Error!");
		close(1);
	}

	fileSize = read(file, buffer, 1024);

	for (int i = 0 ; i < fileSize; i++) {
		
		if (isSmallAlphabet(buffer[i])) {
			buffer[i] = buffer[i] - 32;
		}
	}

	lseek(file, (off_t) 0, SEEK_SET);
	write(file, buffer, fileSize - 1);

	close(file);

	return 0;
}
