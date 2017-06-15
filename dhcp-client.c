#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <net/route.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "dhcp-project.h"

void interact();

void discover();

void release();

void inform();

void renew();

void rebind();

void dhcp_setup();

void change_socket_setup(uint32_t client_addr, uint32_t server_addr);

void dhcp_close();

int dhcp_discover(struct dhcp_t *dhcp_ack);

void dhcp_release();

int dhcp_inform(struct dhcp_t *dhcp_ack);

int dhcp_renew(struct dhcp_t *dhcp_ack);

void dhcp_rebind(struct dhcp_t *dhcp_ack);

int get_mac_address(uint8_t *mac);

uint32_t get_ip_address();

uint16_t construct_dhcp_packet(struct dhcp_t *dhcp, uint8_t state);

void setup_interface(struct dhcp_t *dhcp_ack);

void setup_interface_release();

void set_alarm(struct dhcp_t *dhcp_ack);

void lease_time_out();

//租约时间
uint32_t lease_time, t1_time, t2_time;

//网络接口信息结构体，表示一个网卡
struct ifreq if_eth;

//DHCP的socket
int dhcp_socket;

//两个地址结构体
struct sockaddr_in dhcp_server_addr, dhcp_client_addr, receiveAddr;

//网卡名称
char *dev_name;

//Transaction ID
uint32_t xid;

//Your (client) IP address //服务器端offer包提供的IP
uint32_t yiaddr; //网络字节序的

//DHCP Server Identifier //实际为服务器的IP
uint32_t dhcp_server_identifier; //未转换的 网络字节序的

//socket address结构体的长度
socklen_t receiveAddrLen = sizeof(struct sockaddr_in);

int main(int argc, char **argv) {
	//参数数目不对或者是"-h"的时候
	if (argc != 3 || !strcmp(argv[1], "-h")) {
		printf("Usage: %s <interface> <option>\n"
				       "Options:\n"
				       "--interact   interact\n"
				       "--default    default\n", argv[0]);
		exit(0);
	}

	//应该以root权限运行
	if (geteuid() != 0) {
		printf("This program should only be ran by root or be installed as setuid root.\n");
		exit(0);
	}

	//第二个参数即为指定的网卡名称
	dev_name = argv[1];

	//初始化socket和地址结构体
	dhcp_setup();

	//各种模式
	if (!strcmp(argv[2], "--interact")) {
		interact();
	} else if (!strcmp(argv[2], "--default")) {
		discover();
	} else {
		printf("Wrong option! Use \"-h\" to get usage!\n");
	}

	dhcp_close();
	return 0;
}

//交互菜单
void interact() {
	printf("-----> Interactive mode\n");
	char input[1024];
	while (1) {
		printf("Choose what you want to do:\n");
		printf("1.Discover\n");
		printf("2.Release\n");
		printf("3.Inform\n");
		printf("4.Renew\n");
		printf("5.Rebind\n");
		printf("0.Exit\n");
		gets(input);
		switch (input[0]) {
			case 49: //1.Discover
				discover();
				break;
			case 50: //2.Release
				release();
				break;
			case 51: //3.Inform
				inform();
				break;
			case 52: //4.Renew
				renew();
				break;
			case 53: //5.Rebind
				rebind();
				break;
			case 48: //0.Exit
				return;
			default:
				printf("Input wrong! Try again:\n");
				break;
		}
	}
}

void discover() {
	printf("-----> Discover Mode\n"); //广播
	if (get_ip_address() != 0) {
		printf("%s already have IP address!\n", dev_name);
		return;
	}
	struct dhcp_t dhcp_ack;
	int state = dhcp_discover(&dhcp_ack);
	if (state < 0) {
		printf("Cannot receive dhcp packet, please try again later...\n");
		return;
	} else if (state > 0) {
		printf("Received a dhcp nak packet!\n");
		setup_interface_release();
		printf("Automatically set IP to 0.0.0.0.\n");
		return;
	}
	setup_interface(&dhcp_ack);
}

void release() {
	printf("-----> Release Mode\n"); //单播
	alarm(0);
	dhcp_release();
	setup_interface_release();
}

void inform() {
	printf("-----> Inform Mode\n"); //广播
	struct dhcp_t dhcp_ack;
	int state = dhcp_inform(&dhcp_ack);
	if (state < 0) {
		printf("Cannot receive dhcp packet, please try again later...\n");
		return;
	} else if (state > 0) {
		printf("Received a dhcp nak packet!\n");
		return;
	}
}

void renew() {
	printf("-----> Renew Mode\n"); //单播
	struct dhcp_t dhcp_ack;
	int state = dhcp_renew(&dhcp_ack);
	if (state < 0) {
		printf("Cannot receive dhcp packet, please try again later...\n");
		return;
	} else if (state > 0) {
		printf("Received a dhcp nak packet!\n");
		setup_interface_release();
		printf("Automatically set IP to 0.0.0.0.\n");
		discover();
		return;
	}
	set_alarm(&dhcp_ack); //更新T1 T2计时器
}

void rebind() {
	printf("-----> Rebind Mode\n"); //广播
	struct dhcp_t dhcp_ack;
	dhcp_rebind(&dhcp_ack);
	set_alarm(&dhcp_ack); //更新T1 T2计时器
}

//设置socket
void dhcp_setup() {
	//初始化yiaddr为0.0.0.0
	yiaddr = htonl(INADDR_ANY);

	//创建socket
	printf("Creating a socket...\n");
	if ((dhcp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		printf("socket() failed.\n");
		exit(1);
	}

	//设置socket超时时间
	printf("Setting the socket timeout option...\n");
	struct timeval timeout;
	timeout.tv_sec = MAX_WAIT_TIME;
	timeout.tv_usec = 0;
	if (setsockopt(dhcp_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout.tv_sec, sizeof(struct timeval)) < 0) {
		printf("Setting the socket timeout option failed!\n");
		dhcp_close();
		exit(1);
	}

	//绑定socket到网卡上
	printf("Binding the socket to interface: %s...\n", dev_name);
	strcpy(if_eth.ifr_name, dev_name);
	if_eth.ifr_addr.sa_family = AF_INET;
	if (setsockopt(dhcp_socket, SOL_SOCKET, SO_BINDTODEVICE, &if_eth, sizeof(if_eth)) < 0) {
		printf("Bind the socket to %s failed!\n", dev_name);
		dhcp_close();
		exit(1);
	}

	//设置socket广播
	printf("Setting the socket broadcast...\n");
	int flag = 1;
	if (setsockopt(dhcp_socket, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &flag, sizeof(flag)) < 0) {
		printf("Set the socket broadcast failed!\n");
		dhcp_close();
		exit(1);
	}

	//设置两个地址结构体
	printf("Initializing two socket addresses...\n");
	memset(&dhcp_server_addr, 0, sizeof(struct sockaddr_in));
	dhcp_server_addr.sin_family = AF_INET;
	dhcp_server_addr.sin_port = htons(DHCP_SERVER_PORT);
	dhcp_server_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	memset(&dhcp_client_addr, 0, sizeof(struct sockaddr_in));
	dhcp_client_addr.sin_family = AF_INET;
	dhcp_client_addr.sin_port = htons(DHCP_CLIENT_PORT);
	dhcp_client_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//把dhcp_client_addr绑定到dhcp_socket上
	printf("Binding the socket address to socket...\n");
	if (bind(dhcp_socket, (struct sockaddr *) &dhcp_client_addr, sizeof(struct sockaddr_in)) < 0) {
		printf("Bind the socket failed!\n");
		dhcp_close();
		exit(1);
	}
}

//修改两个sockaddr的地址
void change_socket_setup(uint32_t client_addr, uint32_t server_addr) {
	//修改客户端地址结构体
	dhcp_client_addr.sin_addr.s_addr = htonl(client_addr);

	//修改服务器地址结构体
	dhcp_server_addr.sin_addr.s_addr = htonl(server_addr);
}

//关闭socket
void dhcp_close() {
	printf("Closing the socket...\n");
	close(dhcp_socket);
}

//discover过程
int dhcp_discover(struct dhcp_t *dhcp_ack) {
	struct dhcp_t dhcp_discover, dhcp_offer, dhcp_request;
	uint8_t *option_value;
	srand((unsigned int) time(NULL)); //随机数播种
	xid = (uint32_t) rand();

	//discover(+request)包 0.0.0.0 -> 255.255.255.255
	change_socket_setup(INADDR_ANY, INADDR_BROADCAST);

	//发送dhcp discover包
	size_t dhcp_discover_size = construct_dhcp_packet(&dhcp_discover, DHCPDISCOVER);
	printf("Sending the dhcp discover packet...\n");
	sendto(dhcp_socket, &dhcp_discover, dhcp_discover_size, 0, (struct sockaddr *) &dhcp_server_addr,
	       sizeof(struct sockaddr_in));

	//接收dhcp offer包
	while (1) {
		printf("Receiving the dhcp offer packet...\n");
		if (recvfrom(dhcp_socket, &dhcp_offer, sizeof(struct dhcp_t), 0, (struct sockaddr *) &receiveAddr,
		             &receiveAddrLen) < 0) {
			return -1;
		}
		printf("Received an udp packet! Checking...\n");
		dhcp_dump(&dhcp_offer);
		if (dhcp_offer.opcode == BOOTP_MESSAGE_TYPE_REPLY && dhcp_offer.xid == htonl(xid)) {
			get_dhcp_option(&dhcp_offer, OPTION_DHCP_SERVER_IDENTIFIER, &option_value);
			memcpy(&dhcp_server_identifier, option_value, sizeof(struct in_addr));
			yiaddr = dhcp_offer.yiaddr; //获取服务器提供的地址 //网络字节序的
			int addr = ntohl(dhcp_server_identifier);
			printf("Received a dhcp offer packet from %d.%d.%d.%d!\n", (addr >> 24) & 0xFF, (addr >> 16) & 0xFF,
			       (addr >> 8) & 0xFF, (addr) & 0xFF);
			break;
		}
	}

	//发送dhcp request包
	size_t dhcp_request_size = construct_dhcp_packet(&dhcp_request, DHCPREQUEST);
	printf("Sending the dhcp request packet...\n");
	sendto(dhcp_socket, &dhcp_request, dhcp_request_size, 0, (struct sockaddr *) &dhcp_server_addr,
	       sizeof(struct sockaddr_in));

	//接收dhcp ack包
	while (1) {
		printf("Receiving the dhcp ack packet...\n");
		if (recvfrom(dhcp_socket, dhcp_ack, sizeof(struct dhcp_t), 0, (struct sockaddr *) &receiveAddr,
		             &receiveAddrLen) < 0) {
			return -1;
		}
		printf("Received an udp packet! Checking...\n");
		if (dhcp_ack->opcode == BOOTP_MESSAGE_TYPE_REPLY && dhcp_ack->xid == htonl(xid)) {
			get_dhcp_option(dhcp_ack, OPTION_DHCP_MESSAGE_TYPE, &option_value);
			if (*option_value == DHCPNAK) {
				return 1;
			}
			get_dhcp_option(dhcp_ack, OPTION_DHCP_SERVER_IDENTIFIER, &option_value);
			memcpy(&dhcp_server_identifier, option_value, sizeof(struct in_addr));
			int addr = ntohl(dhcp_server_identifier);
			printf("Received a dhcp ack packet from %d.%d.%d.%d!\n", (addr >> 24) & 0xFF, (addr >> 16) & 0xFF,
			       (addr >> 8) & 0xFF, (addr) & 0xFF);
			break;
		}

	}

	return 0;
}

//release过程
void dhcp_release() {
	struct dhcp_t dhcp_release;
	srand((unsigned int) time(NULL)); //随机数播种
	xid = (uint32_t) rand();

	//release包 x.x.x.x -> server_ip_address
	change_socket_setup(ntohl(get_ip_address()), ntohl(dhcp_server_identifier));

	//发送dhcp release包
	size_t dhcp_release_size = construct_dhcp_packet(&dhcp_release, DHCPRELEASE);
	printf("Sending the dhcp discover packet...\n");
	sendto(dhcp_socket, &dhcp_release, dhcp_release_size, 0, (struct sockaddr *) &dhcp_server_addr,
	       sizeof(struct sockaddr_in));
}

//inform过程
int dhcp_inform(struct dhcp_t *dhcp_ack) {
	struct dhcp_t dhcp_inform;
	uint8_t *option_value;
	srand((unsigned int) time(NULL)); //随机数播种
	xid = (uint32_t) rand();

	//inform包 x.x.x.x -> 255.255.255.255
	change_socket_setup(ntohl(get_ip_address()), INADDR_BROADCAST);

	//发送dhcp inform包
	size_t dhcp_inform_size = construct_dhcp_packet(&dhcp_inform, DHCPINFORM);
	printf("Sending the dhcp inform packet...\n");
	sendto(dhcp_socket, &dhcp_inform, dhcp_inform_size, 0, (struct sockaddr *) &dhcp_server_addr,
	       sizeof(struct sockaddr_in));

	//接收dhcp ack包
	while (1) {
		printf("Receiving the dhcp ack packet...\n");
		if (recvfrom(dhcp_socket, dhcp_ack, sizeof(struct dhcp_t), 0, (struct sockaddr *) &receiveAddr,
		             &receiveAddrLen) < 0) {
			return -1;
		}
		printf("Received an udp packet! Checking...\n");
		if (dhcp_ack->opcode == BOOTP_MESSAGE_TYPE_REPLY && dhcp_ack->xid == htonl(xid)) {
			get_dhcp_option(dhcp_ack, OPTION_DHCP_MESSAGE_TYPE, &option_value);
			if (*option_value == DHCPNAK) {
				return 1;
			}
			get_dhcp_option(dhcp_ack, OPTION_DHCP_SERVER_IDENTIFIER, &option_value);
			memcpy(&dhcp_server_identifier, option_value, sizeof(struct in_addr));
			int addr = ntohl(dhcp_server_identifier);
			printf("Received a dhcp ack packet from %d.%d.%d.%d!\n", (addr >> 24) & 0xFF, (addr >> 16) & 0xFF,
			       (addr >> 8) & 0xFF, (addr) & 0xFF);
			break;
		}
	}

	return 0;
}

//renew过程
int dhcp_renew(struct dhcp_t *dhcp_ack) {
	struct dhcp_t dhcp_renew;
	uint8_t *option_value;
	srand((unsigned int) time(NULL)); //随机数播种
	xid = (uint32_t) rand();

	//renew包 x.x.x.x -> server_ip_address
	change_socket_setup(ntohl(get_ip_address()), ntohl(dhcp_server_identifier));

	//发送dhcp renew包
	size_t dhcp_renew_size = construct_dhcp_packet(&dhcp_renew, DHCPREQUEST);
	printf("Sending the dhcp renew packet...\n");
	sendto(dhcp_socket, &dhcp_renew, dhcp_renew_size, 0, (struct sockaddr *) &dhcp_server_addr,
	       sizeof(struct sockaddr_in));

	//接收dhcp ack包
	while (1) {
		printf("Receiving the dhcp ack packet...\n");
		if (recvfrom(dhcp_socket, dhcp_ack, sizeof(struct dhcp_t), 0, (struct sockaddr *) &receiveAddr,
		             &receiveAddrLen) < 0) {
			return -1;
		}
		printf("Received an udp packet! Checking...\n");
		if (dhcp_ack->opcode == BOOTP_MESSAGE_TYPE_REPLY && dhcp_ack->xid == htonl(xid)) {
			get_dhcp_option(dhcp_ack, OPTION_DHCP_MESSAGE_TYPE, &option_value);
			if (*option_value == DHCPNAK) {
				return 1;
			}
			get_dhcp_option(dhcp_ack, OPTION_DHCP_SERVER_IDENTIFIER, &option_value);
			memcpy(&dhcp_server_identifier, option_value, sizeof(struct in_addr));
			int addr = ntohl(dhcp_server_identifier);
			printf("Received a dhcp ack packet from %d.%d.%d.%d!\n", (addr >> 24) & 0xFF, (addr >> 16) & 0xFF,
			       (addr >> 8) & 0xFF, (addr) & 0xFF);
			break;
		}
	}

	return 0;
}

//rebind过程
void dhcp_rebind(struct dhcp_t *dhcp_ack) {
	struct dhcp_t dhcp_rebind;
	uint8_t *option_value;
	srand((unsigned int) time(NULL)); //随机数播种
	xid = (uint32_t) rand();

	//rebind包 x.x.x.x -> 255.255.255.255
	change_socket_setup(ntohl(get_ip_address()), INADDR_BROADCAST);

	//发送dhcp rebind包
	size_t dhcp_rebind_size = construct_dhcp_packet(&dhcp_rebind, DHCPREQUEST);
	printf("Sending the dhcp rebind packet...\n");
	sendto(dhcp_socket, &dhcp_rebind, dhcp_rebind_size, 0, (struct sockaddr *) &dhcp_server_addr,
	       sizeof(struct sockaddr_in));

	//接收dhcp ack包
	while (1) {
		printf("Receiving the dhcp ack packet...\n");
		recvfrom(dhcp_socket, dhcp_ack, sizeof(struct dhcp_t), 0, (struct sockaddr *) &receiveAddr,
		         &receiveAddrLen);
		printf("Received an udp packet! Checking...\n");
		if (dhcp_ack->opcode == BOOTP_MESSAGE_TYPE_REPLY && dhcp_ack->xid == htonl(xid)) {
			get_dhcp_option(dhcp_ack, OPTION_DHCP_SERVER_IDENTIFIER, &option_value);
			memcpy(&dhcp_server_identifier, option_value, sizeof(struct in_addr));
			int addr = ntohl(dhcp_server_identifier);
			printf("Received a dhcp ack packet from %d.%d.%d.%d!\n", (addr >> 24) & 0xFF, (addr >> 16) & 0xFF,
			       (addr >> 8) & 0xFF, (addr) & 0xFF);
			break;
		}
	}
}

//获取mac地址
int get_mac_address(uint8_t *mac) {
	int result = ioctl(dhcp_socket, SIOCGIFHWADDR, &if_eth);
	if (result != 0)
		return -1;

	memcpy(mac, if_eth.ifr_hwaddr.sa_data, HARDWARE_ADDRESS_LENGTH);
	return 0;
}

//获取ip地址
uint32_t get_ip_address() { //返回值为网络字节序
	int result = ioctl(dhcp_socket, SIOCGIFADDR, &if_eth);
	if (result != 0)
		return 0;

	uint32_t ip;
	memcpy(&ip, &((struct sockaddr_in *) (&if_eth.ifr_addr))->sin_addr, sizeof(uint32_t));
	return ip;
}

//构造dhcp packet
uint16_t construct_dhcp_packet(struct dhcp_t *dhcp, uint8_t state) {
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
	if (state == DHCPRELEASE || state == DHCPDECLINE)
		dhcp->flags = htons(0x0000);
	else
		dhcp->flags = htons(BOOTP_FLAGS);

	//Client IP address
	dhcp->ciaddr = get_ip_address();

	//Your (client) IP address
	dhcp->yiaddr = htonl(INADDR_ANY);

	//Next server IP address
	dhcp->siaddr = htonl(INADDR_ANY);

	//Relay agent IP address
	dhcp->giaddr = htonl(INADDR_ANY);

	//Client MAC address
	if (get_mac_address(dhcp->chaddr) < 0) {
		printf("Cannot obtain the MAC address: %s\n", dev_name);
		dhcp_close();
		exit(1);
	}

	//Server host name

	//Boot file name

	//Magic cookie
	dhcp->magic_cookie = htonl(DHCP_MAGIC_COOKIE);

	//----------OPTIONS----------

	//Option: (53) DHCP Message Type (Discover/Request/Decline/Release/Inform)
	len += fill_dhcp_option(&dhcp->options[len], OPTION_DHCP_MESSAGE_TYPE, &state, (uint16_t) sizeof(state));

	if (state == DHCPREQUEST || state == DHCPRELEASE) {
		//Option: (54) DHCP Server Identifier
		len += fill_dhcp_option(&dhcp->options[len], OPTION_DHCP_SERVER_IDENTIFIER, (uint8_t *) &dhcp_server_identifier,
		                        (uint16_t) sizeof(dhcp_server_identifier));
	}

	if (dhcp->ciaddr == htonl(INADDR_ANY)) { //如果当前网卡的IP是空的话，就是处于discover过程中，就会有option 50
		//Option: (50) Requested IP Address
		uint32_t req_ip = yiaddr; //初始是0.0.0.0 或 读取上一个offer包的地址 或 再次discover的时候用上一次可用的地址
		len += fill_dhcp_option(&dhcp->options[len], OPTION_REQUESTED_IP, (uint8_t *) &req_ip,
		                        (uint16_t) sizeof(req_ip));
	}

	if (state != DHCPRELEASE && state != DHCPDECLINE) {
		//Option: (55) Parameter Request List
		len += fill_dhcp_option(&dhcp->options[len], OPTION_PARAMETER_REQUEST_LIST, (uint8_t *) &parameter_req_list,
		                        (uint16_t) sizeof(parameter_req_list));

		//Option: (60) Vendor class identifier //Student Number
		char stuNo[DHCP_VENDOR_CLASS_ID_LENGTH] = DHCP_CLIENT_VENDOR_CLASS_ID;
		len += fill_dhcp_option(&dhcp->options[len], OPTION_VENDOR_CLASS_IDENTIFIER, (uint8_t *) &stuNo,
		                        (uint16_t) sizeof(stuNo));
	}

	//Option: (255) End
	uint8_t option = 0;
	len += fill_dhcp_option(&dhcp->options[len], OPTION_END, &option, (uint16_t) sizeof(option));

	//计算DHCP包的真实长度
	return sizeof(struct dhcp_t) - DHCP_MAX_OPTION_LEN + len;
}

//设置网卡参数
void setup_interface(struct dhcp_t *dhcp_ack) {
	uint8_t *option_value;
	struct sockaddr_in *sin = (struct sockaddr_in *) &if_eth.ifr_addr;
	sin->sin_family = AF_INET;

	//设置IP地址
	memcpy(&sin->sin_addr, &dhcp_ack->yiaddr, sizeof(struct in_addr));
	printf("Setting IP: %s\n", inet_ntoa(sin->sin_addr));
	if (ioctl(dhcp_socket, SIOCSIFADDR, &if_eth) < 0) {
		printf("Cannot set ip address!\n");
		return;
	}

	//设置子网掩码
	if (get_dhcp_option(dhcp_ack, OPTION_SUBNET_MASK, &option_value) != 4) {
		printf("Cannot get subnet mask!\n");
		return;
	}
	memcpy(&sin->sin_addr, option_value, sizeof(struct in_addr));
	printf("Setting subnet mask: %s\n", inet_ntoa(sin->sin_addr));
	if (ioctl(dhcp_socket, SIOCSIFNETMASK, &if_eth) < 0) {
		printf("Cannot set subnet mask!\n");
		return;
	}

	struct rtentry route;
	memset(&route, 0, sizeof(route));
	sin = (struct sockaddr_in *) &route.rt_gateway;
	sin->sin_family = AF_INET;

	//设置网关
	if (get_dhcp_option(dhcp_ack, OPTION_ROUTER, &option_value) != 4) {
		printf("Cannot get gateway!\n");
		return;
	}
	memcpy(&sin->sin_addr, option_value, sizeof(struct in_addr));
	memcpy(&route.rt_gateway, sin, sizeof(struct sockaddr_in));
	((struct sockaddr_in *) &route.rt_dst)->sin_family = AF_INET;
	((struct sockaddr_in *) &route.rt_genmask)->sin_family = AF_INET;
	route.rt_flags = RTF_GATEWAY;
	printf("Setting gateway: %s\n", inet_ntoa(sin->sin_addr));
	if (ioctl(dhcp_socket, SIOCADDRT, &route) < 0) {
		printf("Cannot set gateway!\n");
		return;
	}

	//设置定时
	set_alarm(dhcp_ack);

	return;
}

//释放网卡地址
void setup_interface_release() {
	struct sockaddr_in *sin = (struct sockaddr_in *) &if_eth.ifr_addr;
	sin->sin_family = AF_INET;
	uint32_t blank_address = htonl(INADDR_ANY);

	//设置IP地址
	memcpy(&sin->sin_addr, &blank_address, sizeof(struct in_addr));
	printf("Setting IP: %s\n", inet_ntoa(sin->sin_addr));
	if (ioctl(dhcp_socket, SIOCSIFADDR, &if_eth) < 0) {
		printf("Cannot set ip address!\n");
		return;
	}

	return;
}

//设置定时
void set_alarm(struct dhcp_t *dhcp_ack) {
	uint8_t *option_value;

	//获取租约时间
	if (get_dhcp_option(dhcp_ack, OPTION_IP_ADDRESS_LEASE_TIME, &option_value) != 4) {
		printf("Cannnot get IP lease time!\n");
		return;
	} else {
		memcpy(&lease_time, option_value, 4);
		lease_time = ntohl(lease_time);
	}

	//获取T1
	if (get_dhcp_option(dhcp_ack, OPTION_IP_T1_RENEWAL_TIME, &option_value) != 4) {
		printf("Cannnot get IP renewal time!\n");
		t1_time = lease_time / 2;
	} else {
		memcpy(&t1_time, option_value, 4);
		t1_time = ntohl(t1_time);
	}

	//获取T2
	if (get_dhcp_option(dhcp_ack, OPTION_IP_T2_REBIND_TIME, &option_value) != 4) {
		printf("Cannnot get IP rebind time!\n");
		t2_time = lease_time / 8 * 7;
	} else {
		memcpy(&t2_time, option_value, 4);
		t2_time = ntohl(t2_time);
	}

	printf("IP lease time: %us T1: %us T2: %us\n", lease_time, t1_time, t2_time);

	signal(SIGALRM, lease_time_out);
	alarm(t1_time);
}

//alarm到时
void lease_time_out() {
	if (t1_time != 0) {
		printf("Renewing...\n");
		alarm(t2_time - t1_time);
		t1_time = 0;
		renew();
	} else if (t2_time != 0) {
		printf("Rebinding...\n");
		alarm(lease_time - t2_time);
		t2_time = 0;
		rebind();
	} else {
		printf("Lease expires!\n");
		setup_interface_release();
	}
}
