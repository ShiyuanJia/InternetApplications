#include <stdio.h> /* for printf() and fprintf() */
//#include <sys/socket.h> /* for socket(), bind(), sendto() and recvfrom() */
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa() */
//#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
//#include <unistd.h> /* for close() */

#define ECHOMAX 255 /* Longest string to echo */

void main(int argc, char *argv[]) {
	int sock; /* Socket */
	struct sockaddr_in echoServAddr; /* Local address */
	struct sockaddr_in echoClntAddr; /* Client address */
	unsigned int cliAddrLen; /* Length of client address */
	char echoBuffer[ECHOMAX]; /* Buffer for echo string */
	unsigned short echoServPort; /* Server port */
	ssize_t recvMsgSize; /* Size of received message */

	echoServPort = 56325;
	/* Create socket for sending/receiving datagrams */
	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
		printf("socket() failed.\n");
	/* Construct local address structure */
	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	echoServAddr.sin_port = htons(echoServPort);
	/* Bind to the local address */
	if ((bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr))) < 0)
		printf("bind() failed.\n");

	for (;;) { /* Run forever */
		/* Set the size of the in-out parameter */
		cliAddrLen = sizeof(echoClntAddr);
		/* Block until receive message from a client */
		if ((recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0, (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
			printf("recvfrom() failed.\n");
		echoBuffer[recvMsgSize] = '\0';
		printf("from %s:UDP%d : %s\n", inet_ntoa(echoClntAddr.sin_addr), echoClntAddr.sin_port, echoBuffer);
//		/* Send received datagram back to the client */
//		if ((sendto(sock, echoBuffer, recvMsgSize, 0, (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr))) != recvMsgSize)
//			printf("sendto() sent a different number of bytes than expected.\n");
	}
}