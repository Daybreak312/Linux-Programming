#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    char *envlist[] = {"APPLE=0", "BANANA=1", (char *) 0};

    execle("ex0813.c", "ex0813.c", (char *) 0, envlist);
}
