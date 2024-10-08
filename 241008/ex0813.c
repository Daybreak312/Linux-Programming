#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char **envlist;

void main() {
    while (*envlist) {
        printf("%s\n", *envlist++);
    }

}
