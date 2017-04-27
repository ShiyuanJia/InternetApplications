#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), sendto() and recvfrom() */
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */

#define ECHOMAX 255 /* Longest string to echo */

int main(int argc, char *argv[]) {
	int sock; /* Socket descriptor */
	struct sockaddr_in echoServAddr; /* Echo server address */
//	struct sockaddr_in fromAddr; /* Source address of echo */
	unsigned short echoServPort; /* Echo server port */
//	unsigned int fromSize; /* In-out of address size for recvfrom() */
	char *servIP; /* IP address of server */
	char *echoString; /* String to send to echo server */
//	char echoBuffer[ECHOMAX + 1]; /* Buffer for receiving echoed string */
	size_t echoStringLen; /* Length of string to echo */
//	int respStringLen; /* Length of received response */

	if (argc < 3) { /* Test for correct number of arguments */
		printf("Usage: %s <Server IP> <Echo Word>\n", argv[0]);
		exit(1);
	}

	servIP = argv[1]; /* First arg: server IP address (dotted quad) */
	echoServPort = 56325;

	/* Create a datagram/UDP socket */
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		printf("socket() failed.\n");
	/* Construct the server address structure */
	memset(&echoServAddr, 0, sizeof(echoServAddr));/*Zero out structure*/
	echoServAddr.sin_family = AF_INET; /* Internet addr family */
	echoServAddr.sin_addr.s_addr = inet_addr(servIP);/*Server IP address*/
	echoServAddr.sin_port = htons(echoServPort); /* Server port */

	for (int i = 2; i < argc; i++) {
		echoString = argv[i]; /* Second arg: string to echo */
		if ((echoStringLen = strlen(echoString)) > ECHOMAX) /* Check input length */
			printf("Echo word too long.\n");
		/* Send the string to the server */
		if (sendto(sock, echoString, echoStringLen, 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != echoStringLen)
			printf("sendto() sent a different number of bytes than expected.\n");
		printf("sending data to '%s'\n", servIP);
	}

//	/* Recv a response */
//	fromSize = sizeof(fromAddr);
//	if ((respStringLen = recvfrom(sock, echoBuffer, ECHOMAX, 0, (struct sockaddr *) &fromAddr, &fromSize)) != echoStringLen)
//		printf("recvfrom() failed\n");
//
//	if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
//		printf("Error: received a packet from unknown source.\n");
//		exit(1);
//	}
//	/* null-terminate the received data */
//	echoBuffer[respStringLen] = '\0';
//	printf("Received: %s\n", echoBuffer);/*Print the echoed message*/
	close(sock);

	return 0;
}