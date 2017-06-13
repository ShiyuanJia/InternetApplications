#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if.h>
#include <net/if.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dhcp-project.h"

void usage(char *program);

void doargs(int argc, char **argv);

void dhcp_setup();

void dhcp_close();

void dhcp_discover(struct dhcp_t *dhcp_ack);

uint16_t construct_dhcp_packet(struct dhcp_t *dhcp);

//网络接口信息结构体，表示一个网卡
struct ifreq if_eth;

//DHCP的socket
int dhcp_socket;

//两个地址结构体
struct sockaddr_in dhcp_server_addr, dhcp_client_addr;

//网卡名称
char *dev_name;

//Transaction ID
uint32_t xid;

//Your (client) IP address //服务器端offer包提供的IP
uint32_t yiaddr;

//DHCP Server Identifier //实际为服务器的IP
uint32_t dhcp_server_identifier;

//DHCP客户端的几种状态 //Option: (53) DHCP Message Type (Discover/Request/Release/Inform)
int discover = 0;
int request = 0;
int release = 0;
int inform = 0;
int renew = 0;
int rebind = 0;

int main(int argc, char **argv) {
	if (geteuid() != 0) {
		printf("This program should only be ran by root or be installed as setuid root.\n");
		exit(0);
	}

	//处理参数
	doargs(argc, argv);

	//初始化socket和地址结构体
	dhcp_setup();

	//判断状态
	if (discover) {
		printf("-----> Discover Mode\n");
		struct dhcp_t dhcp_ack;
		dhcp_discover(&dhcp_ack);
		//setup_interface(dhcp_ack); //-----------------------------------------------------------------------------------------------
	} else if (release) {
		printf("-----> Release Mode\n");
	} else if (inform) {
		printf("-----> Inform Mode\n");
	} else if (renew) {
		printf("-----> Renew Mode\n"); //单播
	} else if (rebind) {
		printf("-----> Rebind Mode\n"); //广播
	}

	dhcp_close();
	return 0;
}

//打印命令参数，并退出
void usage(char *program) {
	printf("Usage: %s <interface> [options]\n"
			       "[Options:]\n"
			       "-r           release\n"
			       "-i           inform\n"
			       "--renew      renew\n"
			       "--rebind     rebind\n", program);
	exit(0);
}

//处理参数
void doargs(int argc, char **argv) {
	//参数数目不够或者过多或者是"-h"的时候
	if (argc < 2 || argc > 3 || !strcmp(argv[1], "-h"))
		usage(argv[0]);

	//第二个参数即为指定的网卡名称
	dev_name = argv[1];

	//除了指定网卡外不带其他参数启动，默认从discover开始
	if (argc == 2) {
		discover = 1;
		return;
	}

	if (!strcmp(argv[2], "-r")) {
		release = 1;
	} else if (!strcmp(argv[2], "-i")) {
		inform = 1;
	} else if (!strcmp(argv[2], "--renew")) {
		renew = 1;
	} else if (!strcmp(argv[2], "--rebind")) {
		rebind = 1;
	} else
		usage(argv[0]);
}

//设置socket
void dhcp_setup() {
	//创建socket
	printf("Creating a socket...\n");
	if ((dhcp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		printf("socket() failed.\n");

	//绑定socket到网卡上
	printf("Binding the socket to interface: %s...\n", dev_name);
	strcpy(if_eth.ifr_name, dev_name);
	if_eth.ifr_addr.sa_family = AF_INET;
	if (setsockopt(dhcp_socket, SOL_SOCKET, SO_BINDTODEVICE, (char *) &if_eth, sizeof(if_eth)) < 0) {
		printf("Bind the socket to %s failed!\n", dev_name);
		exit(1);
	}

	//设置socket广播
	printf("Setting the socket broadcast...\n");
	int flag = 1;
	if (setsockopt(dhcp_socket, SOL_SOCKET, SO_BROADCAST, &flag, sizeof flag) < 0) {
		printf("Set the socket broadcast failed!\n");
		exit(1);
	}

	//设置两个地址结构体
	printf("Initializing two socket addresses...\n");
	memset(&dhcp_server_addr, 0, sizeof(dhcp_server_addr));
	memset(&dhcp_client_addr, 0, sizeof(dhcp_client_addr));
	dhcp_server_addr.sin_family = AF_INET;
	dhcp_server_addr.sin_port = htons(DHCP_SERVER_PORT);
//	if (rebind || release)
//		dhcp_server_addr.sin_addr.s_addr =; //之前记录的DHCP服务器地址
//	else
	dhcp_server_addr.sin_addr.s_addr = INADDR_BROADCAST;
	dhcp_client_addr.sin_family = AF_INET;
	dhcp_client_addr.sin_port = htons(DHCP_CLIENT_PORT);
	dhcp_client_addr.sin_addr.s_addr = INADDR_ANY;

	//把dhcp_client_addr绑定到dhcp_socket上
	printf("Binding the socket address to socket...\n");
	if (bind(dhcp_socket, (struct sockaddr *) &dhcp_client_addr, sizeof(struct sockaddr_in)) < 0) {
		printf("Bind the socket failed!\n");
		exit(1);
	}
}

void dhcp_close() {
	printf("Closing the socket...\n");
	close(dhcp_socket);
}

void dhcp_discover(struct dhcp_t *dhcp_ack) {
	struct dhcp_t dhcp_discover, dhcp_offer, dhcp_request;
	struct sockaddr_in receiveAddr;
	uint8_t *option_value;
	srand((unsigned int) time(NULL)); //随机数播种
	xid = (uint32_t) rand();

	//发送dhcp discover包
	size_t dhcp_discover_size = construct_dhcp_packet(&dhcp_discover);
	printf("Sending the dhcp discover packet...\n");
	if (sendto(dhcp_socket, &dhcp_discover, dhcp_discover_size, 0, (struct sockaddr *) &dhcp_server_addr,
	           sizeof(struct sockaddr_in)) < 0) {
		printf("Send dhcp discover packet failed!\n");
		exit(1);
	}

	//接收dhcp offer包
	printf("Receiving the dhcp offer packet...\n");
	while (1) {
		if (recvfrom(dhcp_socket, &dhcp_offer, sizeof(struct dhcp_t), 0, (struct sockaddr *) &receiveAddr,
		             (socklen_t *) sizeof(struct sockaddr_in)) < 0) {
			printf("Cannot receive dhcp offer packet!\n");
			exit(1);
		} else {
			printf("Received an udp packet! Checking...\n");
			if (dhcp_offer.opcode == BOOTP_MESSAGE_TYPE_REPLY && dhcp_offer.xid == xid) {
				get_dhcp_option(&dhcp_offer, OPTION_DHCP_SERVER_IDENTIFIER, &option_value);
				memcpy(&dhcp_server_identifier, option_value, sizeof(struct in_addr));
				int addr = ntohl(receiveAddr.sin_addr.s_addr);
				printf("Received a dhcp offer packet from %d.%d.%d.%d!\n", (addr >> 24) & 0xFF, (addr >> 16) & 0xFF,
				       (addr >> 8) & 0xFF, (addr) & 0xFF);
				break;
			}
		}
	}

	//发送dhcp request包
	request = 1;
	size_t dhcp_request_size = construct_dhcp_packet(&dhcp_request);
	printf("Sending the dhcp request packet...\n");
	if (sendto(dhcp_socket, &dhcp_request, dhcp_request_size, 0, (struct sockaddr *) &dhcp_server_addr,
	           sizeof(struct sockaddr_in)) < 0) {
		printf("Send dhcp request packet failed!\n");
		exit(1);
	}

	//接收dhcp ack包
	printf("Receiving the dhcp ack packet...\n");
	while (1) {
		if (recvfrom(dhcp_socket, dhcp_ack, sizeof(struct dhcp_t), 0, (struct sockaddr *) &receiveAddr,
		             (socklen_t *) sizeof(struct sockaddr_in)) < 0) {
			printf("Cannot receive dhcp ack packet!\n");
			exit(1);
		} else {
			printf("Received an udp packet! Checking...\n");
			if (dhcp_ack->opcode == BOOTP_MESSAGE_TYPE_REPLY && dhcp_ack->xid == xid) {
				memcpy(&dhcp_server_identifier, option_value, sizeof(struct in_addr));
				int addr = ntohl(receiveAddr.sin_addr.s_addr);
				printf("Received a dhcp ack packet from %d.%d.%d.%d!\n", (addr >> 24) & 0xFF, (addr >> 16) & 0xFF,
				       (addr >> 8) & 0xFF, (addr) & 0xFF);
				break;
			}
		}
	}
}

int get_mac_address(char *dev_name, char *mac) {
	struct ifreq s;
	int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	int result;

	strcpy(s.ifr_name, dev_name);
	result = ioctl(fd, SIOCGIFHWADDR, &s);
	close(fd);
	if (result != 0)
		return -1;

	memcpy(mac, s.ifr_addr.sa_data, IFHWADDRLEN);
	return 0;
}

uint16_t construct_dhcp_packet(struct dhcp_t *dhcp) {
	uint16_t len = 0;
	memset(dhcp, 0, sizeof(struct dhcp_t));

	printf("Constructing a dhcp packet...\n");

	//Message Type //Boot Request
	dhcp->opcode = BOOTP_MESSAGE_TYPE_REQUEST;

	//Hardware Type
	dhcp->htype = HARDWARE_TYPE;

	//Hardware Address Length
	dhcp->hlen = HARDWARE_ADDRESS_LENGTH;

	//Hops
	dhcp->hops = DHCP_HOPS;

	//Transaction ID
	dhcp->xid = htonl(xid);

	//Seconds elapsed
	dhcp->secs = htons(SECONDS_ELAPSED);

	//Bootp flags
	dhcp->flags = htons(BOOTP_FLAGS);

	//Client IP address
	if (discover) {
		dhcp->ciaddr = htonl(INADDR_ANY);
	} else {
//		dhcp->ciaddr = htonl(); //读取当前网卡IP-----------------------------
	}

	//Your (client) IP address
	dhcp->yiaddr = htonl(INADDR_ANY);

	//Next server IP address
	dhcp->siaddr = htonl(INADDR_ANY);

	//Relay agent IP address
	dhcp->giaddr = htonl(INADDR_ANY);

	//Client MAC address
	if (get_mac_address(dev_name, (char *) dhcp->chaddr) < 0) {
		printf("Cannot obtain the MAC address: %s\n", dev_name);
		exit(1);
	}

	//Server host name

	//Boot file name

	//Magic cookie
	dhcp->magic_cookie = htonl(DHCP_MAGIC_COOKIE);

	//OPTIONS
	uint8_t option;
	//Option: (53) DHCP Message Type (Discover/Request/Decline/Release/Inform)
	if (discover) {
		if (request)
			option = DHCPREQUEST;
		else
			option = DHCPDISCOVER;
	} else if (release)
		option = DHCPRELEASE;
	else if (inform)
		option = DHCPINFORM;
	else
		option = DHCPREQUEST;
	len += fill_dhcp_option(&dhcp->options[len], OPTION_DHCP_MESSAGE_TYPE, &option, (uint16_t) sizeof(option));

	if (request) {
		//Option: (54) DHCP Server Identifier
		len += fill_dhcp_option(&dhcp->options[len], OPTION_DHCP_MESSAGE_TYPE, (uint8_t *) &dhcp_server_identifier,
		                        (uint16_t) sizeof(dhcp_server_identifier));
	}

	if (discover) {
		//Option: (50) Requested IP Address
		uint32_t req_ip;
		if (request)
			req_ip = yiaddr; //读取上一个offer包的地址 dhcp->yiaddr
		else
			req_ip = htonl(INADDR_ANY);
		len += fill_dhcp_option(&dhcp->options[len], OPTION_REQUESTED_IP, (uint8_t *) &req_ip,
		                        (uint16_t) sizeof(req_ip));
	}

	//Option: (55) Parameter Request List
	len += fill_dhcp_option(&dhcp->options[len], OPTION_PARAMETER_REQUEST_LIST, (uint8_t *) &parameter_req_list,
	                        (uint16_t) sizeof(parameter_req_list));

	//Option: (60) Vendor class identifier //Student Number
	char stuNo[DHCP_VENDOR_CLASS_ID_LENGTH] = DHCP_CLIENT_VENDOR_CLASS_ID;
	len += fill_dhcp_option(&dhcp->options[len], OPTION_VENDOR_CLASS_IDENTIFIER, (uint8_t *) &stuNo,
	                        (uint16_t) sizeof(stuNo));

	//Option: (255) End
	option = 0;
	len += fill_dhcp_option(&dhcp->options[len], OPTION_END, &option, (uint16_t) sizeof(option));

	//计算DHCP包的真实长度
	return sizeof(struct dhcp_t) - DHCP_MAX_OPTION_LEN + len;
}
