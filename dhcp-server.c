#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define RECVMAX 1024
#define DHCP_HEADER_LENTH 240

const char *broadcast_address = "255.255.255.255";

struct option {
	unsigned short type;
	unsigned short length;
	char *value;
};

struct message {
	char op;
	char htype;
	char hlen;
	char hops;
	int transactionID;
	unsigned short seconds;
	unsigned short flag;
	in_addr_t clientIP;
	in_addr_t yourIP;
	in_addr_t serverIP;
	in_addr_t routerIP;
	char hardwareAddress[16];
	char sname[64];
	char bname[128];
};

int option_subnet_mask = 0;
int option_router_option = 0;
int option_domain_name_option = 0;
int option_ip_address_lease_time = 0;
int option_dhcp_message_type = 0;
int option_server_identifier = 0;
int option_parameter_request_list = 0;
int option_renewal_t1_time_value = 0;
int option_rebinding_t2_time_value = 0;
int option_vendor_class_identifier = 0;

int main() {
	struct sockaddr_in dhcp_server_address;
	unsigned short dhcp_server_port = 68;
	struct sockaddr_in dhcp_client_address;
	unsigned short dhcp_client_port = 67;
	unsigned int dhcp_client_address_length;

	int sock;

	char recv_buffer[RECVMAX];
	struct message recv_struct;

	memset(&dhcp_server_address, 0, sizeof(dhcp_server_address));
	dhcp_server_address.sin_family = AF_INET;
	dhcp_server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	dhcp_server_address.sin_port = htons(dhcp_server_port);

	memset(&dhcp_client_address, 0, sizeof(dhcp_client_address));
	dhcp_client_address.sin_family = AF_INET;
	dhcp_client_address.sin_addr.s_addr = inet_addr(broadcast_address);
	dhcp_client_address.sin_port = htons(dhcp_client_port);

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		printf("socket() failed.\n");

	if ((bind(sock, (struct sockaddr *) &dhcp_server_address, sizeof(dhcp_server_address))) < 0)
		printf("bind() failed.\n");

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
	for (;;) {
		dhcp_client_address_length = sizeof(dhcp_client_address);

		printf("fuck\n");

		memset(recv_buffer, 0, RECVMAX);
		int message_length = (int) recvfrom(sock, recv_buffer, RECVMAX, 0, (struct sockaddr *) &dhcp_client_address,
		                                    (socklen_t *) &dhcp_client_address_length);
		if (message_length < 0)
			printf("recvfrom() failed.\n");

		printf("fuck again\n");

		memcpy(&recv_struct, recv_buffer, DHCP_HEADER_LENTH);

		int size = DHCP_HEADER_LENTH;

		for (int i = 0; i < message_length; i++) {
			printf("%d\n", i);
			printf("%c\n", recv_buffer[size + 1]);
		}

		return 0;
/*
		while (recv_buffer[size + 1] != ff) {
			if (size + 1 == 1) {
				option_subnet_mask = 1;
				size +=;
			} else if (size + 1 == 3) {
				option_router_option = 1;
				size +=;
			} else if (size + 1 == 6) {
				option_domain_name_option = 1;
				size +=;
			} else if (size + 1 == 51) {
				option_ip_address_lease_time = 1;
				size +=;
			} else if (size + 1 == 53) {
				option_dhcp_message_type = 1;
				size +=;
			} else if (size + 1 == 54) {
				option_server_identifier = 1;
				size +=;
			} else if (size + 1 == 55) {
				option_parameter_request_list = 1;
				size +=;
			} else if (size + 1 == 58) {
				option_renewal_t1_time_value = 1;
				size +=;
			} else if (size + 1 == 59) {
				option_rebinding_t2_time_value = 1;
				size +=;
			} else if (size + 1 == 60) {
				option_vendor_class_identifier = 1;
				size +=;
			}
		}


		if (option_subnet_mask) {
		}
		if (option_router_option) {
		}
		if (option_domain_name_option) {
		}
		if (option_ip_address_lease_time) {
		}
		if (option_dhcp_message_type) {
		}
		if (option_server_identifier) {
		}
		if (option_parameter_request_list) {
		}
		if (option_renewal_t1_time_value) {
		}
		if (option_rebinding_t2_time_value) {
		}
		if (option_vendor_class_identifier) {
		}


		printf("from %s:UDP%d : %s\n", inet_ntoa(dhcp_client_address.sin_addr), dhcp_client_address.sin_port,
		       recv_buffer);


		*/

//		/* Send received datagram back to the client */
//		if ((sendto(sock, recv_buffer, recvMsgSize, 0, (struct sockaddr *) &dhcp_client_address, sizeof(dhcp_client_address))) != recvMsgSize)
//			printf("sendto() sent a different number of bytes than expected.\n");
	}
#pragma clang diagnostic pop
}

//message dhcpoffer(message inputformat) {
//	message offerformat;
//	offerformat.transactionID = inputformat.transactionID;
//	offerformat.hardwareAddress = inputformat.hardwareAddress;
//	return offerformat;
//}