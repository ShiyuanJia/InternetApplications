#include <unistd.h>
#include <stdio.h>

int main() {

	pid_t t;

	printf("Original program, pid = %d\n", getpid());

	t = fork();
	if (t == 0) {
		printf("In child process, pid = %d, ppid = %d\n", getpid(), getppid());
	} else {
		printf("In parent, pid = %d, for returned = %d\n", getpid(), t);
	}

	return 0;
}
