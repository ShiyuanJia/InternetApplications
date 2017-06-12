#include <sys/ioctl.h>
#include <linux/if.h>

#define DHCP_SERVER_PORT                      67
#define DHCP_CLIENT_PORT                      68

#define MAX_WAIT_TIME                         3

//DHCP Packet Arguments
#define BOOTP_MESSAGE_TYPE_REQUEST            1
#define BOOTP_MESSAGE_TYPE_REPLY              2

#define HARDWARD_TYPE                         1

#define DHCP_HOPS                             0

#define SECONDS_ELAPSED                       0

//0x0000 -> Unicast, 0x0080 -> Broadcast //此处为0x0080 抓包则为0x8000 Broadcast
#define BOOTP_FLAGS                           0x0080

#define DHCP_MAGIC_COOKIE                     0x63825363

#define DHCP_UDP_OVERHEAD	                  (20 + 8)/* IP header + UDP header */
#define DHCP_FIXED_NON_UDP	                  240
#define DHCP_FIXED_LEN		                  (DHCP_FIXED_NON_UDP + DHCP_UDP_OVERHEAD)
#define DHCP_MTU_MAX		                  1500
#define DHCP_MAX_OPTION_LEN	                  (DHCP_MTU_MAX - DHCP_FIXED_LEN)

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
#define DHCP_CLIENT_VENDOR_CLASS_ID           {'2','0','1','4','2','1','2','8','8','0'} //2014212880  //余博问
#define DHCP_SERVER_VENDOR_CLASS_ID           {'2','0','1','4','2','1','2','8','7','3'} //2014212873  //李昕龙

struct dhcp_t{
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

u_int8_t parameter_req_list[] = {OPTION_SUBNET_MASK, OPTION_ROUTER, OPTION_DOMAIN_NAME_SERVER,
                                 OPTION_IP_ADDRESS_LEASE_TIME, OPTION_IP_T1_RENEWAL_TIME, OPTION_IP_T2_REBIND_TIME,
                                 OPTION_DOMAIN_NAME, OPTION_VENDOR_CLASS_IDENTIFIER};

char *dhcp_options[] = {
		"",
/*   1 */    "Subnet Mask",
/*   2 */    "Time Offset",
/*   3 */    "Router",
/*   4 */    "Time Server",
/*   5 */    "",
/*   6 */    "Domain Name Server",
/*   7 */    "",
/*   8 */    "",
/*   9 */    "",
/*  10 */    "",
/*  11 */    "",
/*  12 */    "Host Name",
/*  13 */    "",
/*  14 */    "",
/*  15 */    "Domain Name",
/*  16 */    "",
/*  17 */    "",
/*  18 */    "",
/*  19 */    "",
/*  20 */    "",
/*  21 */    "",
/*  22 */    "",
/*  23 */    "",
/*  24 */    "",
/*  25 */    "",
/*  26 */    "Interface MTU",
/*  27 */    "",
/*  28 */    "Broadcast Address",
/*  29 */    "",
/*  30 */    "",
/*  31 */    "Perform Router Discover",
/*  32 */    "",
/*  33 */    "Static Route",
/*  34 */    "",
/*  35 */    "",
/*  36 */    "",
/*  37 */    "",
/*  38 */    "",
/*  39 */    "",
/*  40 */    "",
/*  41 */    "",
/*  42 */    "Network Time Protocol Servers",
/*  43 */    "Vendor-Specific Information",
/*  44 */    "NetBIOS over TCP/IP Name Server",
/*  45 */    "",
/*  46 */    "NetBIOS over TCP/IP Node Type",
/*  47 */    "NetBIOS over TCP/IP Scope",
/*  48 */    "",
/*  49 */    "",
/*  50 */    "Requested IP Address",
/*  51 */    "IP Address Lease Time",
/*  52 */    "",
/*  53 */    "DHCP Message Type",
/*  54 */    "Server Identifier",
/*  55 */    "",
/*  56 */    "Message",
/*  57 */    "",
/*  58 */    "Renewal (T1) Time Value",
/*  59 */    "Rebinding (T2) Time Value",
/*  60 */    "Vendor class identifier",
/*  61 */    "Client-identifier",
/*  62 */    "",
/*  63 */    "",
/*  64 */    "",
/*  65 */    "",
/*  66 */    "",
/*  67 */    "",
/*  68 */    "",
/*  69 */    "",
/*  70 */    "",
/*  71 */    "",
/*  72 */    "",
/*  73 */    "",
/*  74 */    "",
/*  75 */    "",
/*  76 */    "",
/*  77 */    "",
/*  78 */    "",
/*  79 */    "",
/*  80 */    "",
/*  81 */    "",
/*  82 */    "",
/*  83 */    "",
/*  84 */    "",
/*  85 */    "",
/*  86 */    "",
/*  87 */    "",
/*  88 */    "",
/*  89 */    "",
/*  90 */    "",
/*  91 */    "",
/*  92 */    "",
/*  93 */    "",
/*  94 */    "",
/*  95 */    "",
/*  96 */    "",
/*  97 */    "",
/*  98 */    "",
/*  99 */    "",
/* 100 */    "",
/* 101 */    "",
/* 102 */    "",
/* 103 */    "",
/* 104 */    "",
/* 105 */    "",
/* 106 */    "",
/* 107 */    "",
/* 108 */    "",
/* 109 */    "",
/* 110 */    "",
/* 111 */    "",
/* 112 */    "",
/* 113 */    "",
/* 114 */    "",
/* 115 */    "",
/* 116 */    "",
/* 117 */    "",
/* 118 */    "",
/* 119 */    "Domain Search",
/* 120 */    "",
/* 121 */    "Classless Static Route",
/* 122 */    "",
/* 123 */    "",
/* 124 */    "",
/* 125 */    "",
/* 126 */    "",
/* 127 */    "",
/* 128 */    "",
/* 129 */    "",
/* 130 */    "",
/* 131 */    "",
/* 132 */    "",
/* 133 */    "",
/* 134 */    "",
/* 135 */    "",
/* 136 */    "",
/* 137 */    "",
/* 138 */    "",
/* 139 */    "",
/* 140 */    "",
/* 141 */    "",
/* 142 */    "",
/* 143 */    "",
/* 144 */    "",
/* 145 */    "",
/* 146 */    "",
/* 147 */    "",
/* 148 */    "",
/* 149 */    "",
/* 150 */    "",
/* 151 */    "",
/* 152 */    "",
/* 153 */    "",
/* 154 */    "",
/* 155 */    "",
/* 156 */    "",
/* 157 */    "",
/* 158 */    "",
/* 159 */    "",
/* 160 */    "",
/* 161 */    "",
/* 162 */    "",
/* 163 */    "",
/* 164 */    "",
/* 165 */    "",
/* 166 */    "",
/* 167 */    "",
/* 168 */    "",
/* 169 */    "",
/* 170 */    "",
/* 171 */    "",
/* 172 */    "",
/* 173 */    "",
/* 174 */    "",
/* 175 */    "",
/* 176 */    "",
/* 177 */    "",
/* 178 */    "",
/* 179 */    "",
/* 180 */    "",
/* 181 */    "",
/* 182 */    "",
/* 183 */    "",
/* 184 */    "",
/* 185 */    "",
/* 186 */    "",
/* 187 */    "",
/* 188 */    "",
/* 189 */    "",
/* 190 */    "",
/* 191 */    "",
/* 192 */    "",
/* 193 */    "",
/* 194 */    "",
/* 195 */    "",
/* 196 */    "",
/* 197 */    "",
/* 198 */    "",
/* 199 */    "",
/* 200 */    "",
/* 201 */    "",
/* 202 */    "",
/* 203 */    "",
/* 204 */    "",
/* 205 */    "",
/* 206 */    "",
/* 207 */    "",
/* 208 */    "",
/* 209 */    "",
/* 210 */    "",
/* 211 */    "",
/* 212 */    "",
/* 213 */    "",
/* 214 */    "",
/* 215 */    "",
/* 216 */    "",
/* 217 */    "",
/* 218 */    "",
/* 219 */    "",
/* 220 */    "",
/* 221 */    "",
/* 222 */    "",
/* 223 */    "",
/* 224 */    "",
/* 225 */    "",
/* 226 */    "",
/* 227 */    "",
/* 228 */    "",
/* 229 */    "",
/* 230 */    "",
/* 231 */    "",
/* 232 */    "",
/* 233 */    "",
/* 234 */    "",
/* 235 */    "",
/* 236 */    "",
/* 237 */    "",
/* 238 */    "",
/* 239 */    "",
/* 240 */    "",
/* 241 */    "",
/* 242 */    "",
/* 243 */    "",
/* 244 */    "",
/* 245 */    "",
/* 246 */    "",
/* 247 */    "",
/* 248 */    "",
/* 249 */    "Private/Classless Static Route (Microsoft)",
/* 250 */    "",
/* 251 */    "",
/* 252 */    "Private/Proxy autodiscovery",
/* 253 */    "",
/* 254 */    "",
/* 255 */    "End"
};

char *dhcp_message_types[] = {
		"",
/*   1 */    "DHCPDISCOVER",
/*   2 */    "DHCPOFFER",
/*   3 */    "DHCPREQUEST",
/*   4 */    "DHCPDECLINE",
/*   5 */    "DHCPACK",
/*   6 */    "DHCPNAK",
/*   7 */    "DHCPRELEASE",
/*   8 */    "DHCPINFORM"
};

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

void printip(unsigned char *buffer) {
	printf("%d.%d.%d.%d\n", buffer[0], buffer[1], buffer[2], buffer[3]);
}