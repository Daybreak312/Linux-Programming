#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {

	int fd;
	mode_t oldMask;

	oldMask = umask(023);
	fd = open(argv[1], O_CREAT, 0777);

	printf("%d\n", fd);

	close(fd);
}
