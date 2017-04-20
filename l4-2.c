#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
	int fd1;
	off_t off;
	char s[50];

	if ((fd1 = open("test1", O_RDWR | O_APPEND)) == -1) {
		printf("Error in opening-1\n");
		return 1;
	}

	write(fd1, "Hiiii", 5);
	off = lseek(fd1, 0, SEEK_SET);
	printf("Using append, offset: %lli\n", (long long) off);

//	off = lseek(fd1, -502, SEEK_CUR);
//	printf("%lli\n", (long long)off);

	read(fd1, s, 50);
	printf("String: %s\n", s);
	lseek(fd1, 0, SEEK_SET);
	write(fd1, "Abbbb", 5);

	close(fd1);

	return 0;
}