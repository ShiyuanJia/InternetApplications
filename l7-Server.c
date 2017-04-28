#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), sendto() and recvfrom() */
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <fcntl.h>

void main(int argc, char *argv[]) {
	int serverSock, clientSock;
	struct sockaddr_in serverAddr, clientAddr;
	socklen_t clientAddrSize = sizeof(clientAddr);
	char fileName[20], buffer[50];

	if ((serverSock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("socket() failed.\n");

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverAddr.sin_port = htons(1234);

	if ((bind(serverSock, (struct sockaddr *) &serverAddr, sizeof(serverAddr))) < 0)
		printf("bind() failed.\n");

	listen(serverSock, SOMAXCONN);

	while (1) {
		clientSock = accept(serverSock, (struct sockaddr *) &clientAddr, &clientAddrSize);
		printf("*********************************\n");
		printf("Accept client %s on TCP Port %d\n", inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port);
		int fileNameLength;
		if (fileNameLength = recv(clientSock, fileName, 20, 0)) {
			fileName[fileNameLength] = '\0';
			printf("This client request for file name: %s\n", fileName);
			int file = open(fileName, O_RDONLY);
			printf("Entering file transfer...\n");
			int transferSize = 0;
			size_t nCount;
			while ((nCount = fread(buffer, 1, 50, (FILE *) file)) > 0) {
				send(clientSock, buffer, nCount, 0);
				transferSize += nCount;
			}
			printf("End of the file\n");
			printf("%d BYTES data have been sent\n", transferSize);
			close(file);
		}
		close(clientSock);
	}
}