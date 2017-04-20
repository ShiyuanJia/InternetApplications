#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
	char buf[10];
	int fd1, fd2;

	if ((fd1 = open("test1", O_RDWR | O_APPEND)) == -1) {
		printf("Error in opening-1\n");
		return 1;
	}

	if ((fd2 = open("test2", O_RDONLY)) == -1) {
		printf("Error in opening-2\n");
		return 1;
	}

	while ((read(fd2, buf, 1)) != 0) {
		write(fd1, buf, 1);
	}

	close(fd1);
	close(fd2);

	return 0;
}