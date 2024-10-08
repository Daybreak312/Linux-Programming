#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char **environ

void main() {
    while (*environ) {
        printf("%s\n", *environ++);
    }

}
