#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dhcp-project.h"

void usage(char *program);

void dhcp_setup();

void dhcp_close();

uint8_t get_dhcp_option(struct dhcp_t *dhcp, uint8_t option_type, uint8_t **option_value);

uint16_t fill_dhcp_option(uint8_t *packet, uint8_t code, uint8_t *data, uint16_t len);

uint16_t construct_dhcp_packet(struct dhcp_t *dhcp, int state);

//DHCP的socket
int dhcp_socket;

//两个地址结构体
struct sockaddr_in dhcp_server_addr, dhcp_client_addr;

//收到的dhcp包
struct dhcp_t dhcp_recv;

//Transaction ID
uint32_t xid;

//记录分配租约的个数
int num_of_lease = 0;

//定义租约结构体的头指针，准备使用链表
struct lease_t *lease_head = NULL;

void main(int argc, char **argv) {
	if (geteuid() != 0) {
		printf("This program should only be ran by root or be installed as setuid root.\n");
		exit(0);
	}

	//初始化socket和地址结构体
	dhcp_setup();

	uint8_t *option_value;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
	while (1) {
		//接收dhcp包 Discover/Request/Decline/Release/Inform
		printf("Waiting for the dhcp packet...\n");
		if (recvfrom(dhcp_socket, (void *) &dhcp_recv, sizeof(struct dhcp_t), 0, (struct sockaddr *) &dhcp_client_addr,
		             (socklen_t *) sizeof(struct sockaddr_in)) < 0) {
			printf("Cannot receive dhcp packet!\n");
			continue;
		} else {
			printf("Received an udp packet! Checking...\n");
			//检查包是不是request类型 //检查option 53的长度是不是1 //减少出错的概率
			if (dhcp_recv.opcode != BOOTP_MESSAGE_TYPE_REQUEST ||
			    get_dhcp_option(&dhcp_recv, OPTION_DHCP_MESSAGE_TYPE, &option_value) != 1) {
				printf("Received a wrong packet!\n");
				continue;
			} else {
				//判断接收的包的类型
				switch (*option_value) {
					case DHCPDISCOVER:
						printf("Received a dhcp discover packet!\n");
						dhcp_handle_discover();
						break;
					case DHCPREQUEST:
						printf("Received a dhcp request packet!\n");
						dhcp_handle_request();
						break;
					case DHCPRELEASE:
						printf("Received a dhcp release packet!\n");
						dhcp_handle_release();
						break;
					case DHCPINFORM:
						printf("Received a dhcp inform packet!\n");
						dhcp_handle_inform();
						break;
					case DHCPDECLINE: //Decline没有要求
						printf("Received a dhcp decline packet!\n");
						continue;
					default:
						printf("Received a wrong packet!\n");
						continue;
				}
			}
		}
	}
#pragma clang diagnostic pop
}

//设置socket
void dhcp_setup() {
	//创建socket
	printf("Creating a socket...\n");
	if ((dhcp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		printf("socket() failed.\n");

/*
    //给socket设置超时时间
	printf("Seting the max wait time of socket...\n");
	struct timeval timeout;
    timeout.tv_sec = MAX_WAIT_TIME;
    timeout.tv_usec = 0;
    setsockopt(dhcp_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
*/

	//设置socket广播
	printf("Setting the socket broadcast...\n");
	int flag = 1;
	if (setsockopt(dhcp_socket, SOL_SOCKET, SO_BROADCAST, &flag, sizeof flag) < 0) {
		printf("Set the socket broadcast failed!\n");
		exit(1);
	}

	//设置地址结构体
	printf("Initializing the socket address...\n");
	memset(&dhcp_server_addr, 0, sizeof(dhcp_server_addr));
	dhcp_server_addr.sin_family = AF_INET;
	dhcp_server_addr.sin_port = htons(DHCP_SERVER_PORT);
	dhcp_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);;

	//把dhcp_client_addr绑定到dhcp_socket上
	printf("Binding the socket address to socket...\n");
	if (bind(dhcp_socket, (struct sockaddr *) &dhcp_server_addr, sizeof(struct sockaddr_in)) < 0) {
		printf("Bind the socket failed!\n");
		exit(1);
	}
}

void dhcp_close() {
	close(dhcp_socket);
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
	//Type
	packet[0] = code;
	//Length
	packet[1] = (uint8_t) len;
	//Value
	memcpy(&packet[2], data, len);

	return len + (uint16_t) (sizeof(uint8_t) * ((uint16_t) 2));
}

uint16_t construct_dhcp_packet(struct dhcp_t *dhcp, int state) {
	uint16_t len = 0;
	memset(dhcp, 0, sizeof(struct dhcp_t));

	//Message Type //Boot Request
	dhcp->opcode = BOOTP_MESSAGE_TYPE_REPLY;

/*
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
//		dhcp->ciaddr = htonl(); //读取当前网卡IP
	}
*/

	//Your (client) IP address
	if (state == DHCPDISCOVER) {
		//
	} else {
		dhcp->yiaddr = htonl(INADDR_ANY); //非discover状态填入客户端地址
	}

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
	char stuNo[DHCP_VENDOR_CLASS_ID_LENGTH] = DHCP_SERVER_VENDOR_CLASS_ID;
	len += fill_dhcp_option(&dhcp->options[len], OPTION_VENDOR_CLASS_IDENTIFIER, (uint8_t *) &stuNo,
	                        (uint16_t) sizeof(stuNo));

	//Option: (255) End
	option = 0;
	len += fill_dhcp_option(&dhcp->options[len], OPTION_END, &option, (uint16_t) sizeof(option));

	//计算DHCP包的真实长度
	return sizeof(struct dhcp_t) - DHCP_MAX_OPTION_LEN + len;
}

//处理客户端的discover
void dhcp_handle_discover() {
	struct dhcp_t *dhcp_offer = &dhcp_recv;

	//发offer包
	construct_dhcp_packet(dhcp_offer, DHCPDISCOVER);
}

//处理客户端的request
void dhcp_handle_request() {
	struct dhcp_t *dhcp_ack = &dhcp_recv;

	//发ack包
	uint8_t *option_value;
	if (dhcp_recv.ciaddr == htonl(INADDR_ANY)) { //判断包的Client IP address
		get_dhcp_option(&dhcp_recv, OPTION_REQUESTED_IP, &option_value);
		if (check_ipaddr((uint32_t *) option_value) > 0)
			construct_dhcp_packet(dhcp_ack, DHCPDISCOVER); //无 -> discover过程的request
		else
			construct_dhcp_packet(dhcp_ack, DHCPDISCOVER);
	} else
		construct_dhcp_packet(dhcp_ack, DHCPREQUEST); //有 -> 普通request
}

//处理客户端的release
void dhcp_handle_release() {
	//不需要回复，只需要把地址池中的对应地址设为可用即可
}

//处理客户端的inform
void dhcp_handle_inform() {
	struct dhcp_t *dhcp_offer = &dhcp_recv;

	//发ack包
	construct_dhcp_packet(dhcp_offer, DHCPINFORM);
}

//检查IP地址 //包括检查地址池 //大于0 -> 正常 //小于0 -> 错误
int check_ipaddr(uint32_t *ip) {
	struct in_addr addr;
	addr.s_addr = *ip;

	//检查地址的格式是否正确
	if (inet_pton(AF_INET, inet_ntoa(addr), &addr) != 1) {
		return -1;
	}

	//检查是否在地址池中
	uint32_t req_ip = ntohl(*ip);
	if (req_ip < IP_ADDRESS_POOL_START || req_ip > IP_ADDRESS_POOL_START + IP_ADDRESS_POOL_AMOUNT) {
		return -1;
	}

	//检查有没有重复的地址
	struct lease_t *lease = lease_head; //保留头指针不动
	while (lease) {
		if (lease->addr == *ip)
			return -1;
		lease = lease->next;
	}

	return 1;
}
