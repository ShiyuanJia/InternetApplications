/*
 * DHCP project header file.
 * Created by 余博问 2014212880 李昕龙 2014212873
 */

#define DHCP_SERVER_PORT                      67
#define DHCP_CLIENT_PORT                      68

//-----> DHCP Packet Arguments
#define BOOTP_MESSAGE_TYPE_REQUEST            1
#define BOOTP_MESSAGE_TYPE_REPLY              2

#define HARDWARE_TYPE                         1
#define HARDWARE_ADDRESS_LENGTH               IFHWADDRLEN

#define DHCP_HOPS                             0

#define SECONDS_ELAPSED                       0

//0x0000 -> Unicast  0x8000 -> Broadcast
#define BOOTP_FLAGS                           0x8000

#define DHCP_MAGIC_COOKIE                     0x63825363

#define DHCP_UDP_OVERHEAD                     (20 + 8) //IP header + UDP header
#define DHCP_FIXED_NON_UDP                    240
#define DHCP_FIXED_LEN                        (DHCP_FIXED_NON_UDP + DHCP_UDP_OVERHEAD)
#define DHCP_MTU_MAX                          1500
#define DHCP_MAX_OPTION_LEN                   (DHCP_MTU_MAX - DHCP_FIXED_LEN)

#define OPTION_SUBNET_MASK                    1
#define OPTION_ROUTER                         3
#define OPTION_DOMAIN_NAME_SERVER             6
#define OPTION_HOST_NAME                      12
#define OPTION_DOMAIN_NAME                    15
#define OPTION_REQUESTED_IP                   50
#define OPTION_IP_ADDRESS_LEASE_TIME          51
#define OPTION_DHCP_MESSAGE_TYPE              53
#define OPTION_DHCP_SERVER_IDENTIFIER         54
#define OPTION_PARAMETER_REQUEST_LIST         55
#define OPTION_IP_T1_RENEWAL_TIME             58
#define OPTION_IP_T2_REBIND_TIME              59
#define OPTION_VENDOR_CLASS_IDENTIFIER        60
#define OPTION_END                            255

//DHCP Message Type <- OPTION_DHCP_MESSAGE_TYPE
#define DHCPDISCOVER                          1
#define DHCPOFFER                             2
#define DHCPREQUEST                           3
#define DHCPDECLINE                           4
#define DHCPACK                               5
#define DHCPNAK                               6
#define DHCPRELEASE                           7
#define DHCPINFORM                            8

//Vendor class identifier <- OPTION_VENDOR_CLASS_IDENTIFIER //Student Number
#define DHCP_VENDOR_CLASS_ID_LENGTH           10
#define DHCP_CLIENT_VENDOR_CLASS_ID           {'2','0','1','4','2','1','2','8','8','0'} //2014212880 //余博问
#define DHCP_SERVER_VENDOR_CLASS_ID           {'2','0','1','4','2','1','2','8','7','3'} //2014212873 //李昕龙

//-----> DHCP Server Arguments
#define IP_ADDRESS_POOL_START                 0xC0A80002 //192.168.0.2
#define IP_ADDRESS_POOL_AMOUNT                100
#define IP_ADDRESS_LEASE_TIME                 3600 //3600 s -> 1 hour //最好是8的倍数 //单位有待确定

#define RELAY_AGENT_IP_ADDRESS                DHCP_SERVER_IP_ADDRESS

#define DHCP_SERVER_IP_ADDRESS                0xC0A80001 //192.168.0.1

#define SUBNET_MASK                           0xFFFFFF00 //255.255.255.0
#define ROUTER_IP_ADDRESS                     DHCP_SERVER_IP_ADDRESS
#define DOMAIN_NAME_SERVER_ADDRESS            DHCP_SERVER_IP_ADDRESS

#define T1_RENEWAL_TIME                       IP_ADDRESS_LEASE_TIME * 0.5
#define T2_REBIND_TIME                        IP_ADDRESS_LEASE_TIME * 0.875

struct dhcp_t {
	uint8_t opcode;
	uint8_t htype;
	uint8_t hlen;
	uint8_t hops;
	uint32_t xid;
	uint16_t secs;
	uint16_t flags;
	uint32_t ciaddr;
	uint32_t yiaddr;
	uint32_t siaddr;
	uint32_t giaddr;
	uint8_t chaddr[16];
	char sname[64];
	char fname[128];
	uint32_t magic_cookie;
	uint8_t options[DHCP_MAX_OPTION_LEN]; //把options的内容按每一字节存储 //简单粗暴
};

struct lease_t {
	time_t time_stamp; //记录设置租约的时候的时间
	uint32_t addr;
	u_int8_t chaddr[16];
	struct lease_t *next; //骚骚的使用链表
};

u_int8_t parameter_req_list[] = {OPTION_SUBNET_MASK, OPTION_ROUTER, OPTION_DOMAIN_NAME_SERVER,
                                 OPTION_IP_ADDRESS_LEASE_TIME, OPTION_IP_T1_RENEWAL_TIME, OPTION_IP_T2_REBIND_TIME,
                                 OPTION_DOMAIN_NAME, OPTION_VENDOR_CLASS_IDENTIFIER};

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
