#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

int main(int argc, char* argv[]) {

	DIR *dirp;

	if ((dirp = opendir(argv[1])) == NULL) {
		fprintf(stderr, "Failed to open dir\n");
		exit(1);
	}

	struct dirent *dentry;

	printf("directories list\n");
	while (dentry = readdir(dirp)) {
	
		if (dentry->d_ino != 0) {
	
			printf("%s\n", dentry->d_name);
		}
	}


	closedir(dirp);
}
