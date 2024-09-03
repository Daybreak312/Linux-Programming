#include <unistd.h>
#include <sys/types.h>

int main(int argc, char* argv[]) {

	pid_t pid = fork();

	if (pid == 0) {
	
		sleep(3);
	} else { 

		execl("/bin/wc", "wc", argv[1], argv[2], argv[3], (char *) 0);
	}
}
