## DHCP

### No.1

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

### No.2

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

### No.3

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

### No.4

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

### No.5

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

## DNS-A

![DNS-A](https://github.com/questionyu/InternetApplications/raw/master/l8-images/A-questionyu.com.png)

### No.1

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

### No.2

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

## DNS-MX

### No.1

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

### No.2

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

## DNS-PTR

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

## DNS-AAAA

### No.1

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

### No.2

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

## DNS-TXT

### No.1

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

### No.2

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


