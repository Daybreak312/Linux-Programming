#include <unistd.h>

int main() {

	execl("./a.out", "apple", "banana", (char *) 0);
}
