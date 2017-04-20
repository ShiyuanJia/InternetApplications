#include <unistd.h>
#include <stdio.h>

int main() {
	pid_t t;
	t = fork();
	printf("fork returned %d\n", t);

	return 0;
}
