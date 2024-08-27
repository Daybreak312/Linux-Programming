#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {

	if (access(argv[1], R_OK) == -1) {
		printf("oh no cannot read it!\n");
		exit(1);
	}

	printf("can read that file!\n");

	exit(0);
}
