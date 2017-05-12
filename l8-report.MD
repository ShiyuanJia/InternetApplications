# Lab8 – Capture DHCP&DNS Packets

Author: Bowen Yu
BUPT ID: 2014212880 / QMUL ID: 140918780

---

[TOC]

## The Configuration of Wireshark

Capture the interface "enp2s0" which is my Ethernet adapter.
The filter rule is "udp port 67" for capturing DHCP packets, "udp port 53" for capturing DNS packets.

## DHCP

![DHCP](https://github.com/questionyu/InternetApplications/raw/master/l8-images/DHCP.png)

### Explanation

* Message Type: 1 = BOOTREQUEST, 2 = BOOTREPLY
* Hops: Client sets to zero, optionally used by relay agents when booting via a relay agent.
* Transaction ID: A random number chosen by the client, used by the client and server to associate messages and responses between a client and a server.
* Client IP address:  Only filled in if client is in BOUND, RENEW or REBINDING state and can respond to ARP requests.
* Your (client) IP address: 'your' (client) IP address.
* Server IP address: IP address of next server to use in bootstrap; returned in DHCPOFFER, DHCPACK by server.
* Router IP address:
* Option: (1) Subnet Mask - The subnet mask option specifies the client's subnet mask.
* Option: (3) Router Option - The router option specifies a list of IP addresses for routers on the client's subnet.  Routers SHOULD be listed in order of preference.
* Option: (6) Domain Name Server Option - The domain name server option specifies a list of Domain Name System name servers available to the client.  Servers SHOULD be listed in order of preference.
* Option: (12) Host Name Option - This option specifies the name of the client. The name may or may not be qualified with the local domain name.
* Option: (15) Domain Name - This option specifies the domain name that client should use when resolving hostnames via the Domain Name System.
* Option: (50) Requested IP Address - This option is used in a client request (DHCPDISCOVER) to allow the client to request that a particular IP address be assigned.
* Option: (51) IP Address Lease Time - This option is used in a client request (DHCPDISCOVER or DHCPREQUEST) to allow the client to request a lease time for the IP address. In a server reply (DHCPOFFER), a DHCP server uses this option to specify the lease time it is willing to offer.
* Option: (53) DHCP Message Type - This option is used to convey the type of the DHCP message. 1 = DHCPDISCOVER, 2 = DHCPOFFER, 3 = DHCPREQUEST, 5 = DHCPACK, 7 = DHCPRELEASE
* Option: (54) Server Identifier - This option is used in DHCPOFFER and DHCPREQUEST messages, and may optionally be included in the DHCPACK and DHCPNAK messages.  DHCP servers include this option in the DHCPOFFER in order to allow the client to distinguish between lease offers.  DHCP clients use the contents of the 'server identifier' field as the destination address for any DHCP messages unicast to the DHCP server. DHCP clients also indicate which of several lease offers is being accepted by including this option in a DHCPREQUEST message. The identifier is the IP address of the selected server.
* Option: (55) Parameter Request List - This option is used by a DHCP client to request values for specified configuration parameters.  The list of requested parameters is specified as n octets, where each octet is a valid DHCP option code as defined in this document. The client MAY list the options in order of preference. The DHCP server is not required to return the options in the requested order, but MUST try to insert the requested options in the order requested by the client.
* Option: (58) Renewal (T1) Time Value - This option specifies the time interval from address assignment until the client transitions to the RENEWING state.
* Option: (59) Rebinding (T2) Time Value - This option specifies the time interval from address assignment until the client transitions to the REBINDING state.

* Frame address: 
* IP address: 
* Port number: 

* By the packets I have captured, all of DHCP packets were sent by unicast.

* Differences from lecture notes:

### Message Sequence Chart

```sequence
Note over Client: Graceful shutdown
Client->Server: DHCPRELEASE
Note over Server: Discards lease
Note over Client: Begins initialization 
Client->Server: DHCPDISCOVER
Note over Server: Determines configuration
Server-->Client: DHCPOFFER
Note over Client: Collects replies
Client->Server: DHCPREQUEST
Note over Server: Commits configuration
Server-->Client: DHCPACK
Note over Client: Initialization complete
```

### No.1 DHCP Release

	Bootstrap Protocol (Release)
		Message type: Boot Request (1)
		Hops: 0
		Transaction ID: 0x6bb0247f
		Bootp flags: 0x0000 (Unicast)
			0... .... .... .... = Broadcast flag: Unicast
			.000 0000 0000 0000 = Reserved flags: 0x0000
		Client IP address: 10.205.0.212
		Your (client) IP address: 0.0.0.0
		Option: (53) DHCP Message Type (Release)
			Length: 1
			DHCP: Release (7)
		Option: (54) DHCP Server Identifier
			Length: 4
			DHCP Server Identifier: 10.3.9.3
		Option: (12) Host Name
			Length: 16
			Host Name: i.questionyu.com

* Frame address: Source: HewlettP_3c:7c:0f (e4:11:5b:3c:7c:0f) Destination: Hangzhou_56:f1:51 (00:23:89:56:f1:51) 
* IP address: Source: 10.205.0.212 Destination: 10.3.9.3
* Port number: Source Port: 68 Destination Port: 67

### No.2 DHCP Discover

    Bootstrap Protocol (Discover)
	    Message type: Boot Request (1)
	    Hops: 0
	    Transaction ID: 0x7cf5b152
	    Bootp flags: 0x0000 (Unicast)
	        0... .... .... .... = Broadcast flag: Unicast
	        .000 0000 0000 0000 = Reserved flags: 0x0000
	    Client IP address: 0.0.0.0
	    Your (client) IP address: 0.0.0.0
	    Option: (53) DHCP Message Type (Discover)
	        Length: 1
	        DHCP: Discover (1)
	    Option: (50) Requested IP Address
	        Length: 4
	        Requested IP Address: 10.205.0.212
	    Option: (12) Host Name
	        Length: 16
	        Host Name: i.questionyu.com
	    Option: (55) Parameter Request List
	        Length: 13
	        Parameter Request List Item: (1) Subnet Mask
	        Parameter Request List Item: (28) Broadcast Address
	        Parameter Request List Item: (2) Time Offset
	        Parameter Request List Item: (3) Router
	        Parameter Request List Item: (15) Domain Name
	        Parameter Request List Item: (6) Domain Name Server
	        Parameter Request List Item: (119) Domain Search
	        Parameter Request List Item: (12) Host Name
	        Parameter Request List Item: (44) NetBIOS over TCP/IP Name Server
	        Parameter Request List Item: (47) NetBIOS over TCP/IP Scope
	        Parameter Request List Item: (26) Interface MTU
	        Parameter Request List Item: (121) Classless Static Route
	        Parameter Request List Item: (42) Network Time Protocol Servers

* Frame address: Source: HewlettP_3c:7c:0f (e4:11:5b:3c:7c:0f) Destination: Broadcast (ff:ff:ff:ff:ff:ff) 
* IP address: Source: 0.0.0.0 Destination: 255.255.255.255
* Port number: Source Port: 68 Destination Port: 67

### No.3 DHCP Offer

    Bootstrap Protocol (Offer)
	    Message type: Boot Reply (2)
	    Hops: 1
	    Transaction ID: 0x7cf5b152
	    Bootp flags: 0x0000 (Unicast)
	        0... .... .... .... = Broadcast flag: Unicast
	        .000 0000 0000 0000 = Reserved flags: 0x0000
	    Client IP address: 0.0.0.0
	    Your (client) IP address: 10.205.0.212
	    Option: (53) DHCP Message Type (Offer)
	        Length: 1
	        DHCP: Offer (2)
	    Option: (54) DHCP Server Identifier
	        Length: 4
	        DHCP Server Identifier: 10.3.9.3
	    Option: (51) IP Address Lease Time
	        Length: 4
	        IP Address Lease Time: (3600s) 1 hour
	    Option: (1) Subnet Mask
	        Length: 4
	        Subnet Mask: 255.255.255.224
	    Option: (3) Router
	        Length: 4
	        Router: 10.205.0.193
	    Option: (15) Domain Name
	        Length: 11
	        Domain Name: bupt.edu.cn
	    Option: (6) Domain Name Server
	        Length: 12
	        Domain Name Server: 10.3.9.5
	        Domain Name Server: 10.3.9.4
	        Domain Name Server: 10.3.9.6

* Frame address: Source: Hangzhou_56:f1:51 (00:23:89:56:f1:51) Destination: HewlettP_3c:7c:0f (e4:11:5b:3c:7c:0f)
* IP address: Source: 10.205.0.193 Destination: 10.205.0.212
* Port number: Source Port: 67 Destination Port: 68

### No.4 DHCP Request

    Bootstrap Protocol (Request)
	    Message type: Boot Request (1)
	    Hops: 0
	    Transaction ID: 0x7cf5b152
	    Bootp flags: 0x0000 (Unicast)
	        0... .... .... .... = Broadcast flag: Unicast
	        .000 0000 0000 0000 = Reserved flags: 0x0000
	    Client IP address: 0.0.0.0
	    Your (client) IP address: 0.0.0.0
	    Option: (53) DHCP Message Type (Request)
	        Length: 1
	        DHCP: Request (3)
	    Option: (54) DHCP Server Identifier
	        Length: 4
	        DHCP Server Identifier: 10.3.9.3
	    Option: (50) Requested IP Address
	        Length: 4
	        Requested IP Address: 10.205.0.212
	    Option: (12) Host Name
	        Length: 16
	        Host Name: i.questionyu.com
	    Option: (55) Parameter Request List
	        Length: 13
	        Parameter Request List Item: (1) Subnet Mask
	        Parameter Request List Item: (28) Broadcast Address
	        Parameter Request List Item: (2) Time Offset
	        Parameter Request List Item: (3) Router
	        Parameter Request List Item: (15) Domain Name
	        Parameter Request List Item: (6) Domain Name Server
	        Parameter Request List Item: (119) Domain Search
	        Parameter Request List Item: (12) Host Name
	        Parameter Request List Item: (44) NetBIOS over TCP/IP Name Server
	        Parameter Request List Item: (47) NetBIOS over TCP/IP Scope
	        Parameter Request List Item: (26) Interface MTU
	        Parameter Request List Item: (121) Classless Static Route
	        Parameter Request List Item: (42) Network Time Protocol Servers

* Frame address: Source: HewlettP_3c:7c:0f (e4:11:5b:3c:7c:0f) Destination: Broadcast (ff:ff:ff:ff:ff:ff)
* IP address: Source: 0.0.0.0 Destination: 255.255.255.255
* Port number: Source Port: 68 Destination Port: 67

### No.5 DHCP ACK

	Bootstrap Protocol (ACK)
	    Message type: Boot Reply (2)
	    Hops: 1
	    Transaction ID: 0x7cf5b152
	    Bootp flags: 0x0000 (Unicast)
	        0... .... .... .... = Broadcast flag: Unicast
	        .000 0000 0000 0000 = Reserved flags: 0x0000
	    Client IP address: 0.0.0.0
	    Your (client) IP address: 10.205.0.212
	    Option: (53) DHCP Message Type (ACK)
	        Length: 1
	        DHCP: ACK (5)
	    Option: (54) DHCP Server Identifier
	        Length: 4
	        DHCP Server Identifier: 10.3.9.3
	    Option: (51) IP Address Lease Time
	        Length: 4
	        IP Address Lease Time: (3600s) 1 hour
	    Option: (1) Subnet Mask
	        Length: 4
	        Subnet Mask: 255.255.255.224
	    Option: (3) Router
	        Length: 4
	        Router: 10.205.0.193
	    Option: (15) Domain Name
	        Length: 11
	        Domain Name: bupt.edu.cn
	    Option: (6) Domain Name Server
	        Length: 12
	        Domain Name Server: 10.3.9.5
	        Domain Name Server: 10.3.9.4
	        Domain Name Server: 10.3.9.6

* Frame address: Source: Hangzhou_56:f1:51 (00:23:89:56:f1:51) Destination: HewlettP_3c:7c:0f (e4:11:5b:3c:7c:0f)
* IP address: Source: 10.205.0.193 Destination: 10.205.0.212
* Port number: Source Port: 67 Destination Port: 68

## DNS-A questionyu.com

![DNS-A](https://github.com/questionyu/InternetApplications/raw/master/l8-images/A-questionyu.com.png)

### Explanation

* Frame address: 
* IP address: 
* Port number: 

* Differences from lecture notes:

### No.1 DNS Query

	Domain Name System (query)
	    [Response In: 2]
	    Transaction ID: 0x3a72
	    Flags: 0x0100 Standard query
	        0... .... .... .... = Response: Message is a query
	        .000 0... .... .... = Opcode: Standard query (0)
	        .... ..0. .... .... = Truncated: Message is not truncated
	        .... ...1 .... .... = Recursion desired: Do query recursively
	        .... .... .0.. .... = Z: reserved (0)
	        .... .... ...0 .... = Non-authenticated data: Unacceptable
	    Questions: 1
	    Answer RRs: 0
	    Authority RRs: 0
	    Additional RRs: 0
	    Queries
	        questionyu.com: type A, class IN
	            Name: questionyu.com
	            [Name Length: 14]
	            [Label Count: 2]
	            Type: A (Host Address) (1)
	            Class: IN (0x0001)

* Frame address: Source: HewlettP_3c:7c:0f (e4:11:5b:3c:7c:0f) Destination: Hangzhou_56:f1:51 (00:23:89:56:f1:51)
* IP address: Source: 2001:da8:215:c506::beef Destination: 2001:4860:4860::8888
* Port number: Source Port: 48542 Destination Port: 53

### No.2 DNS Response

	Domain Name System (response)
	    [Request In: 1]
	    [Time: 0.100708994 seconds]
	    Transaction ID: 0x3a72
	    Flags: 0x8180 Standard query response, No error
	        1... .... .... .... = Response: Message is a response
	        .000 0... .... .... = Opcode: Standard query (0)
	        .... .0.. .... .... = Authoritative: Server is not an authority for domain
	        .... ..0. .... .... = Truncated: Message is not truncated
	        .... ...1 .... .... = Recursion desired: Do query recursively
	        .... .... 1... .... = Recursion available: Server can do recursive queries
	        .... .... .0.. .... = Z: reserved (0)
	        .... .... ..0. .... = Answer authenticated: Answer/authority portion was not authenticated by the server
	        .... .... ...0 .... = Non-authenticated data: Unacceptable
	        .... .... .... 0000 = Reply code: No error (0)
	    Questions: 1
	    Answer RRs: 2
	    Authority RRs: 0
	    Additional RRs: 0
	    Queries
	        questionyu.com: type A, class IN
	            Name: questionyu.com
	            [Name Length: 14]
	            [Label Count: 2]
	            Type: A (Host Address) (1)
	            Class: IN (0x0001)
	    Answers
	        questionyu.com: type A, class IN, addr 104.31.77.20
	            Name: questionyu.com
	            Type: A (Host Address) (1)
	            Class: IN (0x0001)
	            Time to live: 299
	            Data length: 4
	            Address: 104.31.77.20
	        questionyu.com: type A, class IN, addr 104.31.76.20
	            Name: questionyu.com
	            Type: A (Host Address) (1)
	            Class: IN (0x0001)
	            Time to live: 299
	            Data length: 4
	            Address: 104.31.76.20

* Frame address: Source: Hangzhou_56:f1:51 (00:23:89:56:f1:51) Destination: HewlettP_3c:7c:0f (e4:11:5b:3c:7c:0f)
* IP address: Source: 2001:4860:4860::8888 Destination: 2001:da8:215:c506::beef
* Port number: Source Port: 53 Destination Port: 48542

## DNS-MX questionyu.com

![DNS-MX](https://github.com/questionyu/InternetApplications/raw/master/l8-images/MX-questionyu.com.png)

### No.1 DNS Query

	Domain Name System (query)
	    [Response In: 2]
	    Transaction ID: 0xfec1
	    Flags: 0x0100 Standard query
	        0... .... .... .... = Response: Message is a query
	        .000 0... .... .... = Opcode: Standard query (0)
	        .... ..0. .... .... = Truncated: Message is not truncated
	        .... ...1 .... .... = Recursion desired: Do query recursively
	        .... .... .0.. .... = Z: reserved (0)
	        .... .... ...0 .... = Non-authenticated data: Unacceptable
	    Questions: 1
	    Answer RRs: 0
	    Authority RRs: 0
	    Additional RRs: 0
	    Queries
	        questionyu.com: type MX, class IN
	            Name: questionyu.com
	            [Name Length: 14]
	            [Label Count: 2]
	            Type: MX (Mail eXchange) (15)
	            Class: IN (0x0001)

* Frame address: Source: HewlettP_3c:7c:0f (e4:11:5b:3c:7c:0f) Destination: Hangzhou_56:f1:51 (00:23:89:56:f1:51)
* IP address: Source: 2001:da8:215:c506::beef Destination: 2001:4860:4860::8888
* Port number: Source Port: 38337 Destination Port: 53

### No.2 DNS Response

	Domain Name System (response)
	    [Request In: 1]
	    [Time: 0.067065156 seconds]
	    Transaction ID: 0xfec1
	    Flags: 0x8180 Standard query response, No error
	        1... .... .... .... = Response: Message is a response
	        .000 0... .... .... = Opcode: Standard query (0)
	        .... .0.. .... .... = Authoritative: Server is not an authority for domain
	        .... ..0. .... .... = Truncated: Message is not truncated
	        .... ...1 .... .... = Recursion desired: Do query recursively
	        .... .... 1... .... = Recursion available: Server can do recursive queries
	        .... .... .0.. .... = Z: reserved (0)
	        .... .... ..0. .... = Answer authenticated: Answer/authority portion was not authenticated by the server
	        .... .... ...0 .... = Non-authenticated data: Unacceptable
	        .... .... .... 0000 = Reply code: No error (0)
	    Questions: 1
	    Answer RRs: 1
	    Authority RRs: 0
	    Additional RRs: 0
	    Queries
	        questionyu.com: type MX, class IN
	            Name: questionyu.com
	            [Name Length: 14]
	            [Label Count: 2]
	            Type: MX (Mail eXchange) (15)
	            Class: IN (0x0001)
	    Answers
	        questionyu.com: type MX, class IN, preference 10, mx mxdomain.qq.com
	            Name: questionyu.com
	            Type: MX (Mail eXchange) (15)
	            Class: IN (0x0001)
	            Time to live: 299
	            Data length: 16
	            Preference: 10
	            Mail Exchange: mxdomain.qq.com

* Frame address: Source: Hangzhou_56:f1:51 (00:23:89:56:f1:51) Destination: HewlettP_3c:7c:0f (e4:11:5b:3c:7c:0f)
* IP address: Source: 2001:4860:4860::8888 Destination: 2001:da8:215:c506::beef
* Port number: Source Port: 53 Destination Port: 38337

## DNS-PTR 8.8.8.8

![DNS-PTR](https://github.com/questionyu/InternetApplications/raw/master/l8-images/PTR-8.8.8.8.png)

### No.1

	Domain Name System (query)
	    [Response In: 2]
	    Transaction ID: 0x7806
	    Flags: 0x0100 Standard query
	        0... .... .... .... = Response: Message is a query
	        .000 0... .... .... = Opcode: Standard query (0)
	        .... ..0. .... .... = Truncated: Message is not truncated
	        .... ...1 .... .... = Recursion desired: Do query recursively
	        .... .... .0.. .... = Z: reserved (0)
	        .... .... ...0 .... = Non-authenticated data: Unacceptable
	    Questions: 1
	    Answer RRs: 0
	    Authority RRs: 0
	    Additional RRs: 0
	    Queries
	        8.8.8.8.in-addr.arpa: type PTR, class IN
	            Name: 8.8.8.8.in-addr.arpa
	            [Name Length: 20]
	            [Label Count: 6]
	            Type: PTR (domain name PoinTeR) (12)
	            Class: IN (0x0001)

* Frame address: Source: HewlettP_3c:7c:0f (e4:11:5b:3c:7c:0f) Destination: Hangzhou_56:f1:51 (00:23:89:56:f1:51)
* IP address: Source: 2001:da8:215:c506::beef Destination: 2001:4860:4860::8888
* Port number: Source Port: 33287 Destination Port: 53

### No.2

	Domain Name System (response)
	    [Request In: 1]
	    [Time: 0.055456438 seconds]
	    Transaction ID: 0x7806
	    Flags: 0x8180 Standard query response, No error
	        1... .... .... .... = Response: Message is a response
	        .000 0... .... .... = Opcode: Standard query (0)
	        .... .0.. .... .... = Authoritative: Server is not an authority for domain
	        .... ..0. .... .... = Truncated: Message is not truncated
	        .... ...1 .... .... = Recursion desired: Do query recursively
	        .... .... 1... .... = Recursion available: Server can do recursive queries
	        .... .... .0.. .... = Z: reserved (0)
	        .... .... ..0. .... = Answer authenticated: Answer/authority portion was not authenticated by the server
	        .... .... ...0 .... = Non-authenticated data: Unacceptable
	        .... .... .... 0000 = Reply code: No error (0)
	    Questions: 1
	    Answer RRs: 1
	    Authority RRs: 0
	    Additional RRs: 0
	    Queries
	        8.8.8.8.in-addr.arpa: type PTR, class IN
	            Name: 8.8.8.8.in-addr.arpa
	            [Name Length: 20]
	            [Label Count: 6]
	            Type: PTR (domain name PoinTeR) (12)
	            Class: IN (0x0001)
	    Answers
	        8.8.8.8.in-addr.arpa: type PTR, class IN, google-public-dns-a.google.com
	            Name: 8.8.8.8.in-addr.arpa
	            Type: PTR (domain name PoinTeR) (12)
	            Class: IN (0x0001)
	            Time to live: 86399
	            Data length: 32
	            Domain Name: google-public-dns-a.google.com

* Frame address: Source: Hangzhou_56:f1:51 (00:23:89:56:f1:51) Destination: HewlettP_3c:7c:0f (e4:11:5b:3c:7c:0f)
* IP address: Source: 2001:4860:4860::8888 Destination: 2001:da8:215:c506::beef
* Port number: Source Port: 53 Destination Port: 33287

## DNS-AAAA questionyu.com

![DNS-AAAA](https://github.com/questionyu/InternetApplications/raw/master/l8-images/AAAA-questionyu.com.png)

### No.1 DNS Query

	Domain Name System (query)
	    [Response In: 2]
	    Transaction ID: 0x96a2
	    Flags: 0x0100 Standard query
	        0... .... .... .... = Response: Message is a query
	        .000 0... .... .... = Opcode: Standard query (0)
	        .... ..0. .... .... = Truncated: Message is not truncated
	        .... ...1 .... .... = Recursion desired: Do query recursively
	        .... .... .0.. .... = Z: reserved (0)
	        .... .... ...0 .... = Non-authenticated data: Unacceptable
	    Questions: 1
	    Answer RRs: 0
	    Authority RRs: 0
	    Additional RRs: 0
	    Queries
	        questionyu.com: type AAAA, class IN
	            Name: questionyu.com
	            [Name Length: 14]
	            [Label Count: 2]
	            Type: AAAA (IPv6 Address) (28)
	            Class: IN (0x0001)

* Frame address: Source: HewlettP_3c:7c:0f (e4:11:5b:3c:7c:0f) Destination: Hangzhou_56:f1:51 (00:23:89:56:f1:51)
* IP address: Source: 2001:da8:215:c506::beef Destination: 2001:4860:4860::8888
* Port number: Source Port: 52731 Destination Port: 53

### No.2 DNS Response

	Domain Name System (response)
	    [Request In: 1]
	    [Time: 0.068031471 seconds]
	    Transaction ID: 0x96a2
	    Flags: 0x8180 Standard query response, No error
	        1... .... .... .... = Response: Message is a response
	        .000 0... .... .... = Opcode: Standard query (0)
	        .... .0.. .... .... = Authoritative: Server is not an authority for domain
	        .... ..0. .... .... = Truncated: Message is not truncated
	        .... ...1 .... .... = Recursion desired: Do query recursively
	        .... .... 1... .... = Recursion available: Server can do recursive queries
	        .... .... .0.. .... = Z: reserved (0)
	        .... .... ..0. .... = Answer authenticated: Answer/authority portion was not authenticated by the server
	        .... .... ...0 .... = Non-authenticated data: Unacceptable
	        .... .... .... 0000 = Reply code: No error (0)
	    Questions: 1
	    Answer RRs: 2
	    Authority RRs: 0
	    Additional RRs: 0
	    Queries
	        questionyu.com: type AAAA, class IN
	            Name: questionyu.com
	            [Name Length: 14]
	            [Label Count: 2]
	            Type: AAAA (IPv6 Address) (28)
	            Class: IN (0x0001)
	    Answers
	        questionyu.com: type AAAA, class IN, addr 2400:cb00:2048:1::681f:4d14
	            Name: questionyu.com
	            Type: AAAA (IPv6 Address) (28)
	            Class: IN (0x0001)
	            Time to live: 299
	            Data length: 16
	            AAAA Address: 2400:cb00:2048:1::681f:4d14
	        questionyu.com: type AAAA, class IN, addr 2400:cb00:2048:1::681f:4c14
	            Name: questionyu.com
	            Type: AAAA (IPv6 Address) (28)
	            Class: IN (0x0001)
	            Time to live: 299
	            Data length: 16
	            AAAA Address: 2400:cb00:2048:1::681f:4c14

* Frame address: Source: Hangzhou_56:f1:51 (00:23:89:56:f1:51) Destination: HewlettP_3c:7c:0f (e4:11:5b:3c:7c:0f)
* IP address: Source: 2001:4860:4860::8888 Destination: 2001:da8:215:c506::beef
* Port number: Source Port: 53 Destination Port: 52731

## DNS-TXT google-public-dns-a.google.com

![DNS-TXT](https://github.com/questionyu/InternetApplications/raw/master/l8-images/TXT-google-public-dns-a.google.com.png)

### No.1 DNS Query

	Domain Name System (query)
	    [Response In: 2]
	    Transaction ID: 0x8836
	    Flags: 0x0100 Standard query
	        0... .... .... .... = Response: Message is a query
	        .000 0... .... .... = Opcode: Standard query (0)
	        .... ..0. .... .... = Truncated: Message is not truncated
	        .... ...1 .... .... = Recursion desired: Do query recursively
	        .... .... .0.. .... = Z: reserved (0)
	        .... .... ...0 .... = Non-authenticated data: Unacceptable
	    Questions: 1
	    Answer RRs: 0
	    Authority RRs: 0
	    Additional RRs: 0
	    Queries
	        google-public-dns-a.google.com: type TXT, class IN
	            Name: google-public-dns-a.google.com
	            [Name Length: 30]
	            [Label Count: 3]
	            Type: TXT (Text strings) (16)
	            Class: IN (0x0001)

* Frame address: Source: HewlettP_3c:7c:0f (e4:11:5b:3c:7c:0f) Destination: Hangzhou_56:f1:51 (00:23:89:56:f1:51)
* IP address: Source: 2001:da8:215:c506::beef Destination: 2001:4860:4860::8888
* Port number: Source Port: 58105 Destination Port: 53

### No.2 DNS Response

	Domain Name System (response)
	    [Request In: 1]
	    [Time: 0.056486861 seconds]
	    Transaction ID: 0x8836
	    Flags: 0x8180 Standard query response, No error
	        1... .... .... .... = Response: Message is a response
	        .000 0... .... .... = Opcode: Standard query (0)
	        .... .0.. .... .... = Authoritative: Server is not an authority for domain
	        .... ..0. .... .... = Truncated: Message is not truncated
	        .... ...1 .... .... = Recursion desired: Do query recursively
	        .... .... 1... .... = Recursion available: Server can do recursive queries
	        .... .... .0.. .... = Z: reserved (0)
	        .... .... ..0. .... = Answer authenticated: Answer/authority portion was not authenticated by the server
	        .... .... ...0 .... = Non-authenticated data: Unacceptable
	        .... .... .... 0000 = Reply code: No error (0)
	    Questions: 1
	    Answer RRs: 1
	    Authority RRs: 0
	    Additional RRs: 0
	    Queries
	        google-public-dns-a.google.com: type TXT, class IN
	            Name: google-public-dns-a.google.com
	            [Name Length: 30]
	            [Label Count: 3]
	            Type: TXT (Text strings) (16)
	            Class: IN (0x0001)
	    Answers
	        google-public-dns-a.google.com: type TXT, class IN
	            Name: google-public-dns-a.google.com
	            Type: TXT (Text strings) (16)
	            Class: IN (0x0001)
	            Time to live: 86399
	            Data length: 22
	            TXT Length: 21
	            TXT: http://xkcd.com/1361/

* Frame address: Source: Hangzhou_56:f1:51 (00:23:89:56:f1:51) Destination: HewlettP_3c:7c:0f (e4:11:5b:3c:7c:0f)
* IP address: Source: 2001:4860:4860::8888 Destination: 2001:da8:215:c506::beef
* Port number: Source Port: 53 Destination Port: 58105
