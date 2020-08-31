# How to create a socket under Windows/Linux?
For this section, you need to know followings:
## 1. Preparation
### (1) under Windows:
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
Note that under Windows, socket is implemented in the form of DLL. A counter is maintained inside the DLL. Only the first call to WSAStartup() can actually load the DLL into memory, so you need to call WSAStartup() at the beginning so you can use other socket API. And the subsequent call of WSAStartup() just simply increase the counter. Note that WSACleanup() is just the opposite. Each call of WSACleanup() decreases the counter by 1, and when the counter decreases to 0, the DLL will be unloaded from memory! Therefore, you should call WSACleanup() as many times as you call WSAStartup().
```C
//at the beginning of program
WSAData wsa;
WSAStartup(0x101, &wsa);

//at the end of program
WSACleanup();
```
### (2) under Linux:
Under Linux, you need to:
* include header file:
```C
#include <netinet/in.h> //contains most of socket APIs
#include <unistd.h> //contains lots of function prototypes of system services: read(), write(), getpid(), close(), etc
```
## 2. Related functions:

```C
int socket( int domain, int type, int protocol);
```
* parameters:
    * **domain:**        setup network communication domain, commonly use *AF_INET* (for TCP/IP-ipv4), others contain *AF_UNIX* (for local communication), *AF_INET6* (for TCP/IP–IPv6)
    * **type:**          socket type, such as SOCK_STREAM (for TCP), SOCK_DGRAM (for UDP), SOCK_RAW, etc. 
    * **protocol:**      assign the protocol that the socket used, such as IPPROTO_TCP, IPPROTO_UDP, IPPROTO_STCP, etc. If don't want to assign, use 0
* return value:
    * If the socket create successfully, it will return a index of the created socket which is an int. \
    * If it failed, it will return -1.\
    Under Windows, you can use WSAGetLastError() to get the error code. \
    Under Linux, you can get the error code via errno

