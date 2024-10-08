#include <stdio.h>
#include <unistd.h>

int main() {
    putenv("APPLE=BANANA");
    printf("%s\n", getenv("APPLE"));

    execl("ex0811.c", "ex0811.c", (char *) 0);
}