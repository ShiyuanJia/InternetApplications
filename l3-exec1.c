#include <unistd.h>
#include <stdio.h>

int main() {
	char *arg[] = {"/bin/ls", 0};

	/* fork, and exec within child process */
	if (fork() == 0) {
		printf("In child process:\n");
		execv(arg[0], arg);
		printf("I will never be called\n");
	}

	printf("Execution continues in parent process\n");

	return 0;
}