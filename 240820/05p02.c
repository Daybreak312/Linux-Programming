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
	int directories = 0, files = 0;

	while (dentry = readdir(dirp)) {
	
		if (dentry->d_ino != 0) {

			if (opendir(dentry->d_name)) {
				directories++;	
			} else {
				files++;
			}
		}
	
	}

	printf("files : %d , directories : %d\n", files, directories);


	closedir(dirp);
}
