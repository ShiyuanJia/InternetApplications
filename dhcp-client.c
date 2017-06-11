#include <sys/types.h>
#include <sys/socket.h>
//#include <asm-generic/socket.h>
#include <netinet/in.h>
//#include <linux/if.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "dhcp-project.h"

uint16_t construct_dhcp_packet(struct dhcp_t *dhcp);

//网络接口信息结构体，表示一个网卡
struct ifreq if_eth;

//DHCP的socket
int dhcp_socket;

struct sockaddr_in dhcp_server_addr, dhcp_client_addr;

//记录时间结构体
struct timeval timeout;

char *dev_name;

//Transaction ID
uint32_t xid;

//Your (client) IP address //服务器端offer包提供的IP
uint32_t yiaddr;

//DHCP Server Identifier //实际为服务器的IP
uint32_t server_identifier;

//DHCP客户端的几种状态 //Option: (53) DHCP Message Type (Discover/Request/Decline/Release/Inform)
int discover = 0;
int request = 0;
int decline = 0;
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

	dhcp_setup();

	//判断状态
	if (discover) {
		struct dhcp_t dhcp_ack;
		dhcp_discover(&dhcp_ack);
		//setup_interface(dhcp_ack); //-----------------------------------------------------------------------------------------------
	} else if (release) {

	} else if (renew) {

	} else if (rebind) {

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
			       "--rebind     rebind", program);
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
	if ((dhcp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		printf("socket() failed.\n");

	//绑定socket到网卡上
	strcpy(if_eth.ifr_name, dev_name);
	if_eth.ifr_addr.sa_family = AF_INET;
	if (setsockopt(dhcp_socket, SOL_SOCKET, SO_BINDTODEVICE, (char *) &if_eth, sizeof(if_eth)) < 0) {
		printf("Bind the socket to %s failed!\n", dev_name);
		exit(1);
	}

	//给socket设置超时时间
	timeout.tv_sec = MAX_WAIT_TIME;
	timeout.tv_usec = 0;
	setsockopt(dhcp_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

	//设置socket广播
	int flag = 1;
	if (setsockopt(dhcp_socket, SOL_SOCKET, SO_BROADCAST, &flag, sizeof flag) < 0) {
		printf("Set the socket broadcast failed!\n");
		exit(1);
	}

	//设置两个地址结构体
	memset(&dhcp_server_addr, 0, sizeof(dhcp_server_addr));
	memset(&dhcp_client_addr, 0, sizeof(dhcp_client_addr));
	dhcp_server_addr.sin_family = AF_INET;
	dhcp_server_addr.sin_port = DHCP_SERVER_PORT;
	if (rebind || release)
		dhcp_server_addr.sin_addr.s_addr =; //之前记录的DHCP服务器地址
	else
		dhcp_server_addr.sin_addr.s_addr = INADDR_BROADCAST;
	dhcp_client_addr.sin_family = AF_INET;
	dhcp_client_addr.sin_port = DHCP_CLIENT_PORT;
	dhcp_client_addr.sin_addr.s_addr = INADDR_ANY;

	//把dhcp_client_addr绑定到dhcp_socket上
	if (bind(dhcp_socket, (struct sockaddr *) &dhcp_client_addr, sizeof(dhcp_client_addr)) < 0) {
		printf("Bind the socket failed!\n");
		exit(1);
	}
}

void dhcp_close() {
	close(dhcp_socket);
}

void dhcp_discover(struct dhcp_t *dhcp_ack) {
	struct dhcp_t dhcp_discover, dhcp_offer, dhcp_request;
	struct sockaddr_in receiveAddr;
	uint8_t *option_value;
	xid = (uint32_t) random();

	//发送dhcp discover包
	size_t dhcp_discover_size = construct_dhcp_packet(&dhcp_discover);
	if (sendto(dhcp_socket, (void *) &dhcp_discover, dhcp_discover_size, 0, (struct sockaddr *) &dhcp_server_addr,
	           sizeof(struct sockaddr_in)) < 0) {
		printf("Send dhcp discover packet failed!\n");
		exit(1);
	}

	//接收dhcp offer包
	while (1) {
		if (recvfrom(dhcp_socket, (void *) &dhcp_offer, sizeof(struct dhcp_t), 0, (struct sockaddr *) &receiveAddr,
		             (socklen_t *) sizeof(struct sockaddr_in)) < 0) {
			printf("Cannot receive dhcp offer packet!\n");
			exit(1);
		} else {
			printf("Received an udp packet! Checking...\n");
			if (dhcp_offer.opcode == BOOTP_MESSAGE_TYPE_REPLY && dhcp_offer.xid == xid) {
				get_dhcp_option(&dhcp_offer, OPTION_DHCP_SERVER_IDENTIFIER, &option_value);
				memcpy(&server_identifier, option_value, sizeof(struct in_addr));
				printf("Received a dhcp offer packet!\n");
				break;
			}
		}
	}

	//发送dhcp request包
	request = 1;
	size_t dhcp_request_size = construct_dhcp_packet(&dhcp_request);
	if (sendto(dhcp_socket, (void *) &dhcp_request, dhcp_request_size, 0, (struct sockaddr *) &dhcp_server_addr,
	           sizeof(struct sockaddr_in)) < 0) {
		printf("Send dhcp request packet failed!\n");
		exit(1);
	}

	//接收dhcp ack包
	while (1) {
		if (recvfrom(dhcp_socket, dhcp_ack, sizeof(struct dhcp_t), 0, (struct sockaddr *) &receiveAddr,
		             (socklen_t *) sizeof(struct sockaddr_in)) < 0) {
			printf("Cannot receive dhcp ack packet!\n");
			exit(1);
		} else {
			printf("Received an udp packet! Checking...\n");
			if (dhcp_ack->opcode == BOOTP_MESSAGE_TYPE_REPLY && dhcp_ack->xid == xid) {
				memcpy(&server_identifier, option_value, sizeof(struct in_addr));
				printf("Received a dhcp ack packet!\n");
				break;
			}
		}
	}
}

uint8_t get_dhcp_option(struct dhcp_t *dhcp, uint8_t option_type, uint8_t **option_value) {
	uint8_t *i;
	i = dhcp->options;
	while (*i != option_type && *i != 0) {
		i++;
		i += *(i);
		i++;
	}
	if (*i == 0)
		return 0;
	i++;
	uint8_t val_len = *i;
	*option_value = i + 1;
	return val_len;
}

uint16_t fill_dhcp_option(uint8_t *packet, uint8_t code, uint8_t *data, uint16_t len) {
	packet[0] = code;
	packet[1] = (uint8_t) len;
	memcpy(&packet[2], data, len);

	return len + (uint16_t) (sizeof(uint8_t) * ((uint16_t) 2));
}

uint16_t construct_dhcp_packet(struct dhcp_t *dhcp) {
	static time_t l = 0;
	uint16_t len = 0;
	memset(dhcp, 0, sizeof(struct dhcp_t));

	//Message Type //Boot Request
	dhcp->opcode = BOOTP_MESSAGE_TYPE_REQUEST;

	//Hardware Type
	dhcp->htype = HARDWARD_TYPE;

	//Hardware Address Length
	dhcp->hlen = IFHWADDRLEN;

	//Hops
	dhcp->hops = DHCP_HOPS;

	//Transaction ID
	dhcp->xid = xid;

	//Seconds elapsed
	dhcp->secs = SECONDS_ELAPSED;

	//Bootp flags
	dhcp->flags = BOOTP_FLAGS;

	//Client IP address
	if (discover) {
		dhcp->ciaddr = htonl(INADDR_ANY);
	} else {
		dhcp->ciaddr = htonl(); //读取当前网卡IP
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

	//Server host name //不初始化此变量，拼接的时候自动补0，下同

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
		len += fill_dhcp_option(&dhcp->options[len], OPTION_DHCP_MESSAGE_TYPE, (uint8_t *) &server_identifier,
		                        (uint16_t) sizeof(server_identifier));
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
	uint64_t stuNo = DHCP_CLIENT_VENDOR_CLASS_ID;
	len += fill_dhcp_option(&dhcp->options[len], OPTION_VENDOR_CLASS_IDENTIFIER, (uint8_t *) &stuNo,
	                        (uint16_t) sizeof(stuNo));

	//Option: (255) End
	option = 0;
	len += fill_dhcp_option(&dhcp->options[len], OPTION_END, &option, (uint16_t) sizeof(option));

	//计算DHCP包的真实长度
	return sizeof(struct dhcp_t) - DHCP_MAX_OPTION_LEN + len;
}

void dhcp_read(struct dhcp_t *dhcp_reply) {

	struct sockaddr_in fromsock;
	socklen_t fromlen = sizeof(struct sockaddr_in);

	ssize_t i = recvfrom(dhcp_socket, dhcp_reply, sizeof(struct dhcp_t), 0, (struct sockaddr *) &fromsock, &fromlen);
	int addr = ntohl(fromsock.sin_addr.s_addr);

	printf("Got answer from: %d.%d.%d.%d\n",
	       (addr >> 24) & 0xFF, (addr >> 16) & 0xFF,
	       (addr >> 8) & 0xFF, (addr) & 0xFF
	);

	dhcp_dump(dhcp_reply, i);
}

//DHCP包输出显示
void dhcp_dump(struct dhcp_t *dhcp_reply, ssize_t size) {
	int i;

	printf("Packet %zi bytes\n", size);

	//DHCP包16进制全部输出
	for (i = 0; i < size; i++) {
		printf("%02x ", buffer[i]);
		if (i % 16 == 15)
			printf(" (%2d)\n", i / 16);
	}
	printf("\n");

	//把可打印字符全部打印出来
	for (i = 0; i < size; i++) {
		if (isprint(buffer[i]))
			printf("%c ", buffer[i]);
		else
			printf("  ");
		if (i % 16 == 15)
			printf(" (%2d)\n", i / 16);
	}
	printf("\n");

	//Message type
	printf("op: %d\n", buffer[0]);

	//Hardware type
	printf("htype: %d\n", buffer[1]);

	//Hardware address length
	printf("hlen: %d\n", buffer[2]);

	//Hops
	printf("hops: %d\n", buffer[3]);

	//Transaction ID
	printf("xid: %02x%02x%02x%02x\n",
	       buffer[4], buffer[5], buffer[6], buffer[7]);

	//Seconds elapsed
	printf("secs: %d\n", 255 * buffer[8] + buffer[9]);

	//Bootp flags
	printf("flags: %x\n", 255 * buffer[10] + buffer[11]);

	//Client IP address
	printf("ciaddr: %d.%d.%d.%d\n",
	       buffer[12], buffer[13], buffer[14], buffer[15]);

	//Your (client) IP address
	printf("yiaddr: %d.%d.%d.%d\n",
	       buffer[16], buffer[17], buffer[18], buffer[19]);

	//Next server IP address
	printf("siaddr: %d.%d.%d.%d\n",
	       buffer[20], buffer[21], buffer[22], buffer[23]);

	//Relay agent IP address
	printf("giaddr: %d.%d.%d.%d\n",
	       buffer[24], buffer[25], buffer[26], buffer[27]);

	//Client MAC address and Client hardware address padding
	printf("chaddr: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
	       buffer[28], buffer[29], buffer[30], buffer[31],
	       buffer[32], buffer[33], buffer[34], buffer[35],
	       buffer[36], buffer[37], buffer[38], buffer[39],
	       buffer[40], buffer[41], buffer[42], buffer[43]);

	//Server host name
	printf("sname : %s.\n", buffer + 44);

	//Boot file name
	printf("fname : %s.\n", buffer + 108);

	i = 236;
	i += 4;  //Magic cookie: DHCP

	while (i < size && buffer[i] != 255) {
		printf("option %2d %s ", buffer[i], dhcp_options[buffer[i]]);

		switch (buffer[i]) {
			case OPTION_DHCP_SERVER_IDENTIFIER:
				memcpy(server_identifier, buffer + i + 2, 4);
			case OPTION_SUBNET_MASK:
			case OPTION_ROUTER:
			case OPTION_DOMAIN_NAME_SERVER:
			case OPTION_REQUESTED_IP:
				printip(&buffer[i + 2]);
				break;
			case OPTION_IP_ADDRESS_LEASE_TIME:

				break;
			case OPTION_DHCP_MESSAGE_TYPE:
				printf("%d (%s)\n", buffer[i + 2], dhcp_message_types[buffer[i + 2]]);
				break;
			case OPTION_VENDOR_CLASS_IDENTIFIER: //Student Number
				for (int j = 0; j < buffer[i + 1]; ++j) {
					printf("%02x", buffer[i + 2 + j]);
				}
				printf("\n");
				break;
			default:
				break;
		}

		i += buffer[i + 1] + 2;
	}
	printf("\n");
}

