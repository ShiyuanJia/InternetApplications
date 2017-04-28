#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
	char quit = '.';
	char buf[10];
	int fd;
	if ((fd = open("out.out", O_RDWR | O_CREAT)) == -1)
		printf("Error in opening\n");
	while (buf[0] != quit) {
		read(0, buf, 1);
		write(fd, buf, 1);
		write(1, buf, 1);
	}
	close(fd);

	return 0;
}