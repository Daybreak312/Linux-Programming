#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void func1() {

	printf("func1\n");
}

void func2() {
	
	printf("func2\n");
}

int main() {

	printf("hello!\n");

	atexit(func1);
	atexit(func2);

	printf("bye1\n");
	exit(0);
}
