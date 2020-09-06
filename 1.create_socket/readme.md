# **Section 1: Hello Socket!**
For this section, you need to know followings:

## 1. About **Socket** 
* Actually, socket is a data structure that mainntained by opreating system which contains lots of necessary information for communication. Application progress can use socket structure to exploit the transmitting service offered by TCP/UDP entity.
* When the process needs a socket structure for communication, it will call the function *socket()*. Once the socket structure is successfully created, *socket()* will return the id of this newly created socket structure, and subsequent operations on the socket structure are performed via this socket id. So you will see that each socket structre has a socket id, and note that a socket structure can only be used by one process and every process will hold a index table that matches the socket ids to socket structures that owned by it. 
* It's important to know that the socket index table of a process is independent to others, which means that two process might use the same socket id, but actually they refer to different socket structure.
* In all, if application wants to use TCP/UDP entity to implement network communication, it needs to apply for a (layer 4) socket structure. And we will call socket structure as **socket** for short from now on.

## 2. Relationship among **socket**, **socket address**, **port** and **ip address**
It's very significant to understand the relation among these four concepts and it's always very confusing for beginners. Let's use a figure to explain this!\
![Figure-1](https://github.com/zobinHuang/TCP-UDP-socket-notes/blob/master/0.diagram/sec1/1-1.png) \
Firstly, the layer-4 socket works between application layer and transformation layer, it offers the interfaces (API) for application process to exploit the service from transformation entities. \
Then as shown in illustration, suppose we have three processes running on the host "Zobin" which equipped with two NIC, and two IP addresses accordingly ("192.168.1.2" and "192.168.1.3"). As you can see in the figure, PROCESS_1 uses SOCKET_1, PROCESS_2 uses SOCKET_2 and PROCESS_3 uses SOCKET_3, and all three processes use "3" as socket id —— As mentioned above, socket id is independent across processes. And as the connection relationship shown in the figure: 
* SOCKET_1 bind to (**port:** 10086, **IP address:** 192.168.1.2, **protocol:** TCP)  
* SOCKET_2 bind to (**port:** 10087, **IP address:** 192.168.1.3, **protocol:** TCP) 
* SOCKET_3 bind to (**port:** 10087, **IP address:** 192.168.1.3, **protocol:** UDP)

So you can see, a triple **(Layer-4 port, IP address, Communication Protocol)** can uniquely determine a communication endpoint on a host. Such triple needs to be assigned to socket which the process use and it will specify the endpoint on which this socket work, and we call such a triple the **socket address**.

In the code, we use *structure* ***sockaddr_ in*** & ***sockaddr*** to load the informations of socket address. And the drawback of ***struct sockaddr*** is that it mixes the information of ip address and port together, as shown below:
```C
    struct sockaddr {  
        sa_family_t      sin_family;  //network communication domain, commonly use AF_INET (ipv4)
        char             sa_data[14]; //14 bytes, containing the destination address and port information of the socket               
    };
```
***struct sockaddr_in*** fixes this problem, usually programmer use ***struct sockaddr_in*** to config the parameters, then cast to ***struct sockaddr*** as the parameter of some socket functions such as ***bind()***, ***connect()***, ***recvfrom()***, ***sendto()***, etc. Note that there exist some differences of ***struct sockaddr_in*** under different operating system, see below.
### struct sockaddr_in:
```C
    //under Windows & Linux:
    typedef unsigned short u_short;
    struct sockaddr_in {
        short   sin_family;         //network communication domain, commonly use AF_INET (ipv4)
        u_short sin_port;           //16 bits, port number of TCP/UDP
        struct  in_addr sin_addr;   //Exists differences under Windows/Linux, check below
        char    sin_zero[8];        //not used  
    };
```
### struct in_addr:
```C
    //under Windows:
    typedef struct in_addr
    {
        union{
            struct { unsigned char s_b1,s_b2,s_b3,s_b4; } S_un_b;
            struct { unsigned short s_w1,s_w2; } S_un_w;
            unsigned long S_addr;
            }S_un;
        }in_addr;
    
    //under Linux:
    typedef uint32_t in_addr_t;
    struct in_addr
    {
        In_addr_t        s_addr;  //32 bits, ipv4 address.
    }
```
We will use these two structures in our next section. Now you just need to know what these two structures used for and their relationship.

## 3. How to create a socket and bind to a socket address? 
Now you know some very basic stuff of socket. Let's turn to program and see how to create a socket in the code.

### 3.1 Environment setup:
#### (1) under Windows:
Under Windows, you need to:
* include header file:
```C 
// Under new version of Windows, "winsock2.h" will also be included while complile "winsock.h"
#include <winsock.h>
```
* Explicitly notify the compiler to link the socket library
```C
#pragma comment (lib,"wsock32.lib")
```
* Initialize the DLL \
Note that under Windows, socket is implemented in the form of DLL. A counter is maintained inside the DLL. Only the first call of WSAStartup() can actually load the DLL into memory, so you need to call WSAStartup() at the beginning so you can use other socket API, and the subsequent call of WSAStartup() (by other processes) just simply increase the this counter. Note that WSACleanup() is just the opposite, each call of WSACleanup() decreases the counter by 1, and when the counter decreases to 0, the DLL will be unloaded from memory! Therefore, you should call WSACleanup() as many times as you call WSAStartup().
```C
//at the beginning of program
WSAData wsa;
WSAStartup(0x101, &wsa);

//your code...

//at the end of program
WSACleanup();
```
#### (2) under Linux:
Under Linux, you just need to:
* include header file:
```C
#include <netinet/in.h> //contains most of socket APIs
#include <unistd.h> //contains lots of function prototypes of system services: read(), write(), getpid(), close(), etc
```

### 3.2 Create a socket:
To create a socket the program needs to call ***socket( )***. Check details below:
```C
int socket(int domain, int type, int protocol);
```
* parameters:
    * **domain:**        setup network communication domain, commonly use *AF_INET* (for TCP/IP-ipv4), others contain *AF_UNIX* (for local communication), *AF_INET6* (for TCP/IP–IPv6), *AF_OSI*
    * **type:**          socket type, such as SOCK_STREAM (for TCP), SOCK_DGRAM (for UDP), SOCK_RAW, etc. 
    * **protocol:**      assign the protocol that the socket used, such as IPPROTO_TCP, IPPROTO_UDP, IPPROTO_STCP, etc. If don't want to assign, use 0
* return value:
    * If the socket create successfully, it will return a index of the created socket which is an int.
    * If it failed, it will return -1.\
    Under Windows, you can use WSAGetLastError() to get the error code. \
    Under Linux, you can get the error code via errno

Don't worry if you feel unfamiliar with all these three parameters since we haven't discuss about them until next section.

### 3.2 Explicitly bind a socket to a socket address:
After successfully creating a socket, to explicitly bind a socket to a socket address, you need to call function ***bind()***. Check details below:
```C
int bind(int sockfd, const struct sockaddr *addr,socklen_t *addrlen);
```
* parameters:
    * **sockfd:** socket id (return value of ***socket()***)
    * ***addr:** The socket address to bind. Usually programmers use struct sockaddr_in to config the parameters, then cast to struct sockaddr, and pass the pointer of sockaddr as this parameter (a.k.a (struct sockaddr\*)&sockaddr_in).
    * **addrlen:** sizeof **addr**
* return value:
    * If bind successfully, it will return 0.
    * If it failed, it will return -1.\
    Under Windows, you can use WSAGetLastError() to get the error code. \
    Under Linux, you can get the error code via errno.

### 3.3 Close a socket
After finishing all operations on a socket, you need to explicitly close the socket otherwise this socket will occupy the socket address (Layer-4 port, ip address, Layer-4 protocol) to which other process can't bind their sockets.
#### 3.3.1 Under Windows 
You need to call function ***closesocket()***. Check details below:
```C
int PASCAL FAR closesocket(SOCKET s);
```
* parameters:
    * **s**: socket id
* return value:
    * If close successfully, it will return 0.
    * If it failed, it will return -1.\
    Under Windows, you can use WSAGetLastError() to get the error code.
#### 3.3.2 Under Linux 
You need to call function ***close()***. Check details below:
    Under Linux, you can get the error code via errno.

Now please turn to the code in ***create_socket.c*** under this folder to check the full process of creating and binding a socket!
