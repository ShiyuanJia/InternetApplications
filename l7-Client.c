#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), sendto() and recvfrom() */
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <fcntl.h>

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Usage:\n%s <IP Address> <File Name>\n", argv[0]);
		return 1;
	}

	int serverSock;
	struct sockaddr_in serverAddr;
	char *fileName, buffer[10];

	if ((serverSock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("socket() failed.\n");

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
	serverAddr.sin_port = htons(1234);

	connect(serverSock, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
	printf("Connecting to server:  %s\n", argv[1]);

	send(serverSock, argv[2], strlen(argv[2]), 0);

	fileName = argv[2];
	strcat(fileName, ".bak\0");
	FILE *file = (FILE *) open(fileName, O_RDWR | O_CREAT);
	int transferSize = 0;
	while (recv(serverSock, buffer, 1, 0)) {
		write(file, buffer, 1);
		transferSize ++;
	}
	close((int) file);
	printf("File received\n");
	printf("%d BYTES received, and stored in %s\n", transferSize, fileName);

	return 0;
}