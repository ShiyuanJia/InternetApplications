#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
	struct hostent *h;
	long addr;

	if (argc != 2) {
		printf("Usage:\n%s sample.com\nor\n%s 8.8.8.8\n", argv[0], argv[0]);
		return 1;
	}

	if (inet_addr(argv[1]) != -1) {
		addr = inet_addr(argv[1]);
		if ((h = gethostbyaddr((char *) &addr, sizeof(addr), AF_INET)) == NULL) {
			printf("gethostbyaddr error for address: %s\n", argv[1]);
			return 1;
		}
	} else {
		if ((h = gethostbyname(argv[1])) == NULL) {
			printf("gethostbyname error for name: %s\n", argv[1]);
			return 1;
		}
	}

	printf("Looking information for host: %s\n", argv[1]);
	printf("Official Name: %s\n", h->h_name);

	while ((h->h_aliases)[0])
		printf("Host aliases: %s\n", *h->h_aliases++);
	while ((h->h_addr_list)[0])
		printf("IP address: %s\n", inet_ntoa(*(struct in_addr *) *h->h_addr_list++));

	return 0;
}
