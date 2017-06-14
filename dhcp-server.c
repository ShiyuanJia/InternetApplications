#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dhcp-project.h"

void dhcp_setup();

void dhcp_close();

uint16_t construct_dhcp_packet(struct dhcp_t *dhcp, uint8_t state);

void change_socket_setup(uint32_t server_addr, uint32_t client_addr);

void dhcp_handle_discover();

void dhcp_handle_request();

void dhcp_handle_release();

void dhcp_handle_inform();

int check_ipaddr(uint32_t *ip, uint8_t *mac);

uint32_t allocate_ip(uint8_t *mac);

void add_lease(uint32_t *new_ip, uint8_t *mac);

void delete_lease(uint8_t *mac);

//DHCP的socket
int dhcp_socket;

//两个地址结构体 和 接收地址结构体
struct sockaddr_in dhcp_server_addr, dhcp_client_addr, receiveAddr;

//收到的dhcp包
struct dhcp_t dhcp_recv;

//网络接口信息结构体，表示一个网卡
struct ifreq if_eth;

//网卡名称
char *dev_name;

//记录分配租约的个数
int num_of_lease = 0;

//定义租约结构体的头指针，准备使用链表
struct lease_t *lease_head;

void main(int argc, char **argv) {
	if (geteuid() != 0) {
		printf("This program should only be ran by root or be installed as setuid root.\n");
		exit(0);
	}

	//参数数目不对或者是"-h"的时候
	if (argc != 2 || !strcmp(argv[1], "-h")) {
		printf("Usage: %s <interface>\n", argv[0]);
		exit(0);
	}

	//第二个参数即为指定的网卡名称
	dev_name = argv[1];

	//初始化socket和地址结构体
	dhcp_setup();

	uint8_t *option_value;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
	while (1) {
		//接收dhcp包 Discover/Request/Decline/Release/Inform
		printf("Waiting for the dhcp packet...\n");
		socklen_t receiveAddrLen = sizeof(struct sockaddr_in);
		change_socket_setup(INADDR_ANY, INADDR_BROADCAST); //0.0.0.0 -> 255.255.255.255
		if (recvfrom(dhcp_socket, &dhcp_recv, sizeof(struct dhcp_t), 0, (struct sockaddr *) &receiveAddr,
		             &receiveAddrLen) < 0) {
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
						printf("Received a dhcp discover packet from %02x:%02x:%02x:%02x:%02x:%02x\n",
						       dhcp_recv.chaddr[0], dhcp_recv.chaddr[1], dhcp_recv.chaddr[2], dhcp_recv.chaddr[3],
						       dhcp_recv.chaddr[4], dhcp_recv.chaddr[5]);
						dhcp_handle_discover();
						break;
					case DHCPREQUEST:
						printf("Received a dhcp request packet from %02x:%02x:%02x:%02x:%02x:%02x\n",
						       dhcp_recv.chaddr[0], dhcp_recv.chaddr[1], dhcp_recv.chaddr[2], dhcp_recv.chaddr[3],
						       dhcp_recv.chaddr[4], dhcp_recv.chaddr[5]);
						dhcp_handle_request();
						break;
					case DHCPRELEASE:
						printf("Received a dhcp release packet from %02x:%02x:%02x:%02x:%02x:%02x\n",
						       dhcp_recv.chaddr[0], dhcp_recv.chaddr[1], dhcp_recv.chaddr[2], dhcp_recv.chaddr[3],
						       dhcp_recv.chaddr[4], dhcp_recv.chaddr[5]);
						dhcp_handle_release();
						break;
					case DHCPINFORM:
						printf("Received a dhcp inform packet from %02x:%02x:%02x:%02x:%02x:%02x\n",
						       dhcp_recv.chaddr[0], dhcp_recv.chaddr[1], dhcp_recv.chaddr[2], dhcp_recv.chaddr[3],
						       dhcp_recv.chaddr[4], dhcp_recv.chaddr[5]);
						dhcp_handle_inform();
						break;
					case DHCPDECLINE: //Decline没有要求
						printf("Received a dhcp decline packet from %02x:%02x:%02x:%02x:%02x:%02x\n",
						       dhcp_recv.chaddr[0], dhcp_recv.chaddr[1], dhcp_recv.chaddr[2], dhcp_recv.chaddr[3],
						       dhcp_recv.chaddr[4], dhcp_recv.chaddr[5]);
						continue;
					default:
						printf("Received a wrong packet from %02x:%02x:%02x:%02x:%02x:%02x\n", dhcp_recv.chaddr[0],
						       dhcp_recv.chaddr[1], dhcp_recv.chaddr[2], dhcp_recv.chaddr[3], dhcp_recv.chaddr[4],
						       dhcp_recv.chaddr[5]);
						continue;
				}
			}
		}
	}
#pragma clang diagnostic pop
}

//设置socket
void dhcp_setup() {
	//初始化头指针
	lease_head = (struct lease_t *) malloc(sizeof(struct lease_t));
	lease_head->next = NULL;

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
	if (setsockopt(dhcp_socket, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &flag, sizeof(flag)) < 0) {
		printf("Set the socket broadcast failed!\n");
		exit(1);
	}

	//设置地址结构体
	printf("Initializing two socket addresses...\n");
	memset(&dhcp_server_addr, 0, sizeof(struct sockaddr_in));
	dhcp_server_addr.sin_family = AF_INET;
	dhcp_server_addr.sin_port = htons(DHCP_SERVER_PORT);
	dhcp_server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //设为服务器地址时，服务器不处理广播的包
	memset(&dhcp_client_addr, 0, sizeof(struct sockaddr_in));
	dhcp_client_addr.sin_family = AF_INET;
	dhcp_client_addr.sin_port = htons(DHCP_CLIENT_PORT);
	dhcp_client_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

	//把dhcp_client_addr绑定到dhcp_socket上
	printf("Binding the socket address to socket...\n");
	if (bind(dhcp_socket, (struct sockaddr *) &dhcp_server_addr, sizeof(struct sockaddr_in)) < 0) {
		printf("Bind the socket failed!\n");
		exit(1);
	}
}

//关闭socket
void dhcp_close() {
	close(dhcp_socket);
}

//修改dhcp packet
uint16_t construct_dhcp_packet(struct dhcp_t *dhcp, uint8_t state) {
	uint16_t len = 0;

	//Message Type //Boot Reply
	dhcp->opcode = BOOTP_MESSAGE_TYPE_REPLY;

	//Hardware Type

	//Hardware Address Length

	//Hops

	//Transaction ID

	//Seconds elapsed

	//Bootp flags

	//Client IP address

	//Your (client) IP address
	if (state == DHCPNAK) {
		dhcp->yiaddr = htonl(INADDR_ANY); //NAK的时候返回0.0.0.0
	} else if (state == DHCPOFFER) {
		uint32_t offer_ip = allocate_ip(dhcp->chaddr); //从地址池中找一个可用的地址
		if (offer_ip == 0) {
			printf("IP address pool is not available!\n");
			return 0;
		}
		dhcp->yiaddr = htonl(offer_ip);
	} else if (state == DHCPACK && dhcp->ciaddr == htonl(INADDR_ANY)) {
		uint8_t *option_value;
		get_dhcp_option(&dhcp_recv, OPTION_REQUESTED_IP, &option_value); //使用客户端请求的地址作为yiaddr
		add_lease((uint32_t *) option_value, dhcp->chaddr);
		dhcp->yiaddr = *option_value;
	} else {
		uint32_t offer_ip = allocate_ip(dhcp->chaddr); //从租约中找到相同MAC的记录的地址
		dhcp->yiaddr = htonl(offer_ip);
	}

	//Next server IP address

	//Relay agent IP address
	dhcp->giaddr = htonl(RELAY_AGENT_IP_ADDRESS);

	//Client MAC address

	//Server host name

	//Boot file name

	//Magic cookie

	//----------OPTIONS----------

	uint8_t *msg_type;
	get_dhcp_option(&dhcp_recv, OPTION_DHCP_MESSAGE_TYPE, &msg_type);

	//Option: (53) DHCP Message Type (Offer/Ack/Nak)
	len += fill_dhcp_option(&dhcp->options[len], OPTION_DHCP_MESSAGE_TYPE, &state, (uint16_t) sizeof(state));

	//Option: (54) DHCP Server Identifier
	uint32_t dhcp_server_identifier = htonl(DHCP_SERVER_IP_ADDRESS);
	len += fill_dhcp_option(&dhcp->options[len], OPTION_DHCP_SERVER_IDENTIFIER, (uint8_t *) &dhcp_server_identifier,
	                        (uint16_t) sizeof(dhcp_server_identifier));

	//Option: (60) Vendor class identifier //Student Number
	char stuNo[DHCP_VENDOR_CLASS_ID_LENGTH] = DHCP_SERVER_VENDOR_CLASS_ID;
	len += fill_dhcp_option(&dhcp->options[len], OPTION_VENDOR_CLASS_IDENTIFIER, (uint8_t *) &stuNo,
	                        (uint16_t) sizeof(stuNo));

	if (state != DHCPNAK) {
		//Option: (1) Subnet Mask
		uint32_t subnet_mask = htonl(SUBNET_MASK);
		len += fill_dhcp_option(&dhcp->options[len], OPTION_SUBNET_MASK, (uint8_t *) &subnet_mask,
		                        (uint16_t) sizeof(subnet_mask));

		//Option: (3) Router
		uint32_t router_adress = htonl(ROUTER_IP_ADDRESS);
		len += fill_dhcp_option(&dhcp->options[len], OPTION_ROUTER, (uint8_t *) &router_adress,
		                        (uint16_t) sizeof(router_adress));

		//Option: (6) Domain Name Server
		uint32_t dns_address = htonl(DOMAIN_NAME_SERVER_ADDRESS);
		len += fill_dhcp_option(&dhcp->options[len], OPTION_DOMAIN_NAME_SERVER, (uint8_t *) &dns_address,
		                        (uint16_t) sizeof(dns_address));

		if (*msg_type != DHCPINFORM) {
			//Option: (51) IP Address Lease Time
			uint32_t lease_time = htonl(IP_ADDRESS_LEASE_TIME);
			len += fill_dhcp_option(&dhcp->options[len], OPTION_IP_ADDRESS_LEASE_TIME, (uint8_t *) &lease_time,
			                        sizeof(lease_time));

			//Option: (58) Renewal Time Value
			uint32_t t1_renew_time = htonl((uint32_t) (T1_RENEWAL_TIME));
			len += fill_dhcp_option(&dhcp->options[len], OPTION_IP_T1_RENEWAL_TIME, (uint8_t *) &t1_renew_time,
			                        sizeof(t1_renew_time));

			//Option: (59) Rebinding Time Value
			uint32_t t2_rebind_time = htonl((uint32_t) (T2_REBIND_TIME));
			len += fill_dhcp_option(&dhcp->options[len], OPTION_IP_T2_REBIND_TIME, (uint8_t *) &t2_rebind_time,
			                        sizeof(t2_rebind_time));
		}
	}

	//Option: (255) End
	uint8_t option = 0;
	len += fill_dhcp_option(&dhcp->options[len], OPTION_END, &option, (uint16_t) sizeof(option));

	//计算DHCP包的真实长度
	return sizeof(struct dhcp_t) - DHCP_MAX_OPTION_LEN + len;
}

//修改两个sockaddr的地址
void change_socket_setup(uint32_t server_addr, uint32_t client_addr) {
	//修改服务器地址结构体
	dhcp_server_addr.sin_addr.s_addr = htonl(server_addr);

	//修改客户端地址结构体
	dhcp_client_addr.sin_addr.s_addr = htonl(client_addr);
}

//处理客户端的discover
void dhcp_handle_discover() {
	struct dhcp_t *dhcp_offer = &dhcp_recv;

	//发offer包
	size_t dhcp_offer_size = construct_dhcp_packet(dhcp_offer, DHCPOFFER);
	printf("Sending the dhcp offer packet...\n");
	if (sendto(dhcp_socket, dhcp_offer, dhcp_offer_size, 0, (struct sockaddr *) &dhcp_client_addr,
	           sizeof(struct sockaddr_in)) < 0)
		printf("Send dhcp offer packet failed!\n");
}

//处理客户端的request
void dhcp_handle_request() {
	struct dhcp_t *dhcp_nack = &dhcp_recv;

	//发ack包 或 nak包
	size_t dhcp_nack_size;
	uint8_t *option_value; //判断包的Client IP address
	if (dhcp_recv.ciaddr == htonl(INADDR_ANY)) { //空地址 -> discover过程的request -> 即要在option 50申请地址
		get_dhcp_option(&dhcp_recv, OPTION_REQUESTED_IP, &option_value); //获取客户端要申请的地址
		if (check_ipaddr((uint32_t *) option_value, dhcp_recv.chaddr) > 0) //判断申请的地址是否合法
			dhcp_nack_size = construct_dhcp_packet(dhcp_nack, DHCPACK); //合法，返回ack包
		else
			dhcp_nack_size = construct_dhcp_packet(dhcp_nack, DHCPNAK); //不合法，返回nak包
		change_socket_setup(INADDR_ANY, INADDR_BROADCAST); //0.0.0.0 -> 255.255.255.255
	} else {
		dhcp_nack_size = construct_dhcp_packet(dhcp_nack, DHCPACK); //有地址 -> 普通request
		change_socket_setup(DHCP_SERVER_IP_ADDRESS, ntohl(dhcp_recv.ciaddr)); //server_ip_address -> x.x.x.x
	}

	printf("Sending the dhcp ack/nak packet...\n");
	if (sendto(dhcp_socket, dhcp_nack, dhcp_nack_size, 0, (struct sockaddr *) &dhcp_client_addr,
	           sizeof(struct sockaddr_in)) < 0)
		printf("Send dhcp ack/nak packet failed!\n");
}

//处理客户端的release
void dhcp_handle_release() {
	//不需要回复，只需要把租约链表中对应的节点删除就可以了
	delete_lease(dhcp_recv.chaddr);
}

//处理客户端的inform
void dhcp_handle_inform() {
	struct dhcp_t *dhcp_ack = &dhcp_recv;

	change_socket_setup(DHCP_SERVER_IP_ADDRESS, ntohl(dhcp_recv.ciaddr)); //server_ip_address -> x.x.x.x

	//发ack包
	size_t dhcp_ack_size = construct_dhcp_packet(dhcp_ack, DHCPACK);
	printf("Sending the dhcp ack packet...\n");
	if (sendto(dhcp_socket, dhcp_ack, dhcp_ack_size, 0, (struct sockaddr *) &dhcp_client_addr,
	           sizeof(struct sockaddr_in)) < 0)
		printf("Send dhcp ack packet failed!\n");
}

//检查IP地址 //包括检查地址池 //大于0 -> 正常 //小于0 -> 错误
int check_ipaddr(uint32_t *ip, uint8_t *mac) {
	struct in_addr addr;
	addr.s_addr = *ip;

	//检查地址的格式是否正确 //其实只要判断是否在地址池范围内就足够了 //但既然已经写出来了，就留着吧，反正也没啥影响
	if (inet_pton(AF_INET, inet_ntoa(addr), &addr) != 1) {
		return -1;
	}

	//检查是否在地址池中
	uint32_t req_ip = ntohl(*ip);
	if (req_ip < IP_ADDRESS_POOL_START || req_ip > IP_ADDRESS_POOL_START + IP_ADDRESS_POOL_AMOUNT - 1) {
		return -1;
	}

	//检查现有租约中有没有重复的地址
	struct lease_t *lease = lease_head; //保留头指针不动
	while (lease->next) {
		if (lease->next->addr == *ip) { //IP相同
			time_t now = time(NULL);
			if (lease->next->time_stamp + IP_ADDRESS_LEASE_TIME > now) { //租约没有到期
				if (memcmp(lease->next->chaddr, mac, HARDWARE_ADDRESS_LENGTH) != 0) { //记录的MAC地址不相同 //相同的时候memcmp返回0
					return -1;
				} else //记录的MAC地址相同，即正确的客户端来申请原来的地址
					return 1;
			} else //租约已经到期，则地址可用
				return 1;
		}
		lease = lease->next;
	}

	return 1;
}

//从地址池中找到一个可用地址
uint32_t allocate_ip(uint8_t *mac) {
	int num;

	//检查有没有相同的MAC
	struct lease_t *lease = lease_head; //保留头指针不动
	while (lease->next) {
		if (memcmp(lease->next->chaddr, mac, HARDWARE_ADDRESS_LENGTH) == 0) { //MAC相同
			lease->next->time_stamp = time(NULL); //更新时间戳
			return lease->next->addr; //直接返回相同MAC地址对应的IP地址就可以了
		}
		lease = lease->next;
	}

	if (num_of_lease > IP_ADDRESS_POOL_AMOUNT - 1) {
		return 0; //地址池已经分配完了
	}

	//没有相同的MAC
	srand((unsigned int) time(NULL)); //随机数播种
	uint32_t new_ip;

	//判断随机生成的IP有没有重复的
	outer:
	num = rand() % IP_ADDRESS_POOL_AMOUNT;
	new_ip = IP_ADDRESS_POOL_START + num; //随机生成一个IP地址
	lease = lease_head; //保留头指针不动
	while (lease->next) {
		if (lease->next->addr == new_ip)
			goto outer;
		lease = lease->next;
	}

	return new_ip;
}

//记录租约
void add_lease(uint32_t *new_ip, uint8_t *mac) {
	struct lease_t *new_lease = (struct lease_t *) malloc(sizeof(struct lease_t));
	new_lease->time_stamp = time(NULL); //时间戳
	new_lease->addr = *new_ip;
	memcpy(new_lease->chaddr, mac, HARDWARE_ADDRESS_LENGTH);
	new_lease->next = NULL;

	struct lease_t *lease = lease_head; //保留头指针不动
	while (lease->next)
		lease = lease->next;

	lease->next = new_lease;
	num_of_lease++;
}

//删除租约链表中的节点
void delete_lease(uint8_t *mac) {
	struct lease_t *lease = lease_head; //保留头指针不动
	while (lease->next) {
		if (memcmp(lease->next->chaddr, mac, HARDWARE_ADDRESS_LENGTH) == 0) { //MAC相同
			struct lease_t *temp = lease->next;
			lease->next = lease->next->next;
			free(temp);
			num_of_lease--;
			return;
		}
		lease = lease->next;
	}
}
