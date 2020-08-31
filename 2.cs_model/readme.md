# Demonstrating Programs of Communication Flow
For this section, you need to know followings:
## 1. Connection-oriented & Connectionless-oriented
* **Sequence of Connection-oriented C/S:**
    ![image](https://github.com/zobinHuang/TCP-UDP-socket-notes/blob/master/0.diagram/sec2/Sequence%20of%20Connection-oriented.png)
* **Sequence of Connectionless-oriented C/S:**
    ![image](https://github.com/zobinHuang/TCP-UDP-socket-notes/blob/master/0.diagram/sec2/Sequence%20of%20Connectionless-oriented.png)
## 2. struct **sockaddr_in** and struct **sockaddr**:
Firstly, sockaddr_ in & sockaddr are structures that contain informations of socket address. \
And the drawback of struct sockaddr is that it mixes the information of ip address and port together, as shown below:
```C
    struct sockaddr {  
        sa_family_t      sin_family;  //network communication domain, commonly use AF_INET (ipv4)
        char             sa_data[14]; //14 bytes, containing the destination address and port information of the socket               
    };
```
struct sockaddr_in fixes this problem, usually programmer use sockaddr_in to config the parameters, then cast to struct sockaddr as the parameter of some socket functions such as bind(), connect(), recvfrom(), sendto(), etc. But there exist some differences of struct sockaddr_in under different operating system, see below.
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

## 3. Related functions:
### 3.1 Commonly used:
```C
int bind(int sockfd, const struct sockaddr *addr,socklen_t *addrlen);
```
* parameters:
    * **sockfd:** socket index
    * ***addr:** The socket address to bind. Usually programmers use struct sockaddr_in to config the parameters, then cast to struct sockaddr, and pass the pointer of sockaddr as this parameter (a.k.a (struct sockaddr\*)&sockaddr_in).
    * **addrlen:** sizeof **addr**
* return value:
    * If bind successfully, it will return 0.
    * If it failed, it will return -1.\
    Under Windows, you can use WSAGetLastError() to get the error code. \
    Under Linux, you can get the error code via errno.
### 3.2 Connection-oriented commonly used:
#### 3.2.1 Server Side:
```C
int listen( int sockfd, int backlog);
```
* parameters:
    * **sockfd:** socket index.
    * **backlog:** The maximum length of the waiting connection queue.
* return value: 
    * If successfully set as listen , it will return 0.
    * If it failed, it will return -1.\
    Under Windows, you can use WSAGetLastError() to get the error code. \
    Under Linux, you can get the error code via errno.

```C
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```
* parameters:
    * **sockfd:** socket index.
    * ***addr:** The socket address of newly accepted socket. Usually pass a struct sockaddr_in, then cast to struct sockaddr, and pass the pointer as this parameter (a.k.a (struct sockaddr\*)&sockaddr_in).
    * ***addrlen:** NOTE! This is the ***address*** of the int which represents the sizeof **addr**.
* return value: 
    * If accept successfully, it will return a new socket which connect to the client.
    * If it failed, it will return -1.\
    Under Windows, you can use WSAGetLastError() to get the error code. \
    Under Linux, you can get the error code via errno.

#### 3.2.2 Client Side:
```C
int connect(int sockfd, struct sockaddr *addr, int addrlen);
```
* parameters:
    * **sockfd:** socket index
    * ***addr:**  a pointer to struct sockaddr which include the information of remote server 
    * **addrlen:** length of **addr** 
* note:
    *Actually, connect() can also be used in connectionless oriented communication. See below:*\
    * For stream sockets, the connect() call attempts to establish a connection between two sockets. The connect() call performs two tasks when called for a stream socket. First, it completes the binding necessary for a stream socket (in case it has not been previously bound using the bind() call). Second, it attempts to make a connection to another socket.
    * For datagram sockets, the connect() call specifies the peer for a socket (i.e. ipaddr & port). 
* return value:
    * If connect successfully, it will return 0
    * If it failed, it will returns -1.\
    Under Windows, you can use WSAGetLastError() to get the error code. \
    Under Linux, you can get the error code via errno.
    
#### 3.2.3 Server & Client Common:
```C
int send(int sockfd, const char *buf, int len, int flags);
```
* parameters:
    * **sockfd:** send socket index.
    * ***buf:** indicates a buffer that stores the send data.
    * **len:** the number of bytes of data that desire to be sent.
    * **flags:** commonly set as 0.
* execution process (blocked mode):
    * Firstly, it will compare the length of the data to be sent (**len**) and the length of the send buffer of the socket (**sockfd**). If **len** is greater than the length of the send buffer of (**sockfd**), the function returns *SOCKET_ERROR*.
    * If **len** is less than or equal to the length of the send buffer of **sockfd**, then it will check whether the kernel is sending the data in the sending buffer of **sockfd**. If so, wait for the send process to complete.
    * If the kernel has not started sending the data in the sending buffer of **sockfd** or there is no data in the buffer, then it will compare the remaining space of **sockfd**'s send buffer with **len**. If **len** is larger than the size of the remaining space, then it will wait for the kernel to send the data in the buffer of **sockfd**. Otherwise, it will copy the data in **buf** to the send buffer of **sockfd**.
* return value:
    * If it successfully copies data from **buf** to the send buffer of **sockfd**, it will return the actual number of bytes it copied.
    * If an error happened during copying data, it will return *SOCKET_ERROR*.
    * If the network was disconnected while waiting for the kernel to send data, 
      * under Windows: it will return *SOCKET_ERROR*.
      * under Linux: The process will receives a *SIGPIPE* signal, and the default processing of this signal is process termination.
  
```C
int recv( SOCKET sockfd, char *buf, int len, int flags);
```
* parameters:
    * **sockfd:** index of recv socket.
    * ***buf:** indicates a buffer that want to store the receiving data.
    * **len:** the length of **buf**.
    * **flags:** commonly set as 0.
* execution process (blocked mode):
    * Firstly, it will wait for the data in the sending buffer of **sockfd** to be snet by the kernel. If a network error happened during transmition, then it will returns *SOCKET_ERROR*.
    * If there is no data in **sockfd**'s sending buffer or the data is successfully sent by the kernel, then it will check the receive buffer of **sockfd**. If there is no data in the receive buffer or the kernel is receiving data, then it will wait until the data is completly received by the kernel.
    * After the kernel received the data, then it will copy the data in the receive buffer of **sockfd** to **buf** (note that the data received by the kernel may be larger than the length of **buf** (**len**), so in this case, it is necessary to call the recv() for several times to copy all the data in the receive buffer of **sockfd**.
* return value:
    * If it successfully copies data from the send buffer of **sockfd** to **buf**, it will return the actual number of bytes it copied.
    * If an error happened during copying data, it will return *SOCKET_ERROR*.
    * If the network was disconnected while waiting for the kernel to receiving data, 
      * under Windows: it will return *SOCKET_ERROR*.
      * under Linux: The process will receives a *SIGPIPE* signal, and the default processing of this signal is process termination.
### 3.3 Connectionless-oriented commonly used:
#### 3.3.1 Server & Client Common:
```C
int sendto (int sockfd, const void *buf, int len, unsigned int flags, const struct sockaddr *dest_addr, int addrlen);
```
* parameters:
    * **sockfd:** send socket index.
    * ***buf:** indicates the buf that stores the data to be sent.
    * **len:** indicates the length of the data to send.
    * **flag:** commonly set as 0.
    * ***dest_addr:** address of a struct sockaddr that contains the destinated host's ip address and port number.
    * **addrlen:** length of **dest_addr**
* return value:
    * If it successfully copies data from **buf** to the send buffer of **sockfd**, it will return the actual number of bytes it copied.
    * If an error happened during copying data, it will return -1. 
    Under Windows, you can use WSAGetLastError() to get the error code. \
    Under Linux, you can get the error code via errno.
```C
int recvfrom(int sockfd, void *buf, int len, unsigned int flags, struct sockaddr *sour_addr, int *addrlen);
```
* parameters:
  * **sockfd:** send socket index.
  * ***buf:** indicates the buf that wants to store the recv data.
  * **len:** indicates the length of **buf**.
  * **flag:** commonly set as 0.
  * ***sour_addr:** address of a struct sockaddr that contains the source host's ip address and port number.
  * ***addrlen:** NOTE! This is the ***address*** of the int which represents the length of **sour_addr**
* return value:
    * If it successfully receive data and store in **buf** from the receive buffer of **sockfd**, it will return the actual number of bytes it copied.
    * If an error happened during copying data, it will return -1.
    Under Windows, you can use WSAGetLastError() to get the error code. \
    Under Linux, you can get the error code via errno.

### 3.4 Others
Different CPUs have different endian types. These endianness refer to the order in which integers are stored in memory. This is called **host byte order**. There are two most common ones: 
1. **Little endian:** low byte stores high address, high byte stores low address
2. **Big endian:** low byte stores low address, high byte stores high address.

The **network byte order** is the data representation format specified in TCP/IP. It has nothing to do with the specific CPU type, operating system, etc., so as to ensure that the data can be interpreted correctly when transmitted between different hosts. Commonly, the network byte order adopts big endian order.

Now let's see some conversion functions which help us to configure the ip address and port number of socket. You need to know the conversion process, see below:

  * IP address:
    ![image](https://github.com/zobinHuang/TCP-UDP-socket-notes/blob/master/0.diagram/sec2/ipaddr%20conversion%20process.png)
  * Port number:
    ![image](https://github.com/zobinHuang/TCP-UDP-socket-notes/blob/master/0.diagram/sec2/port%20conversion%20process.png)
\
Note that to use these functions:
Under windows, they're already inside the header file: winsock.h
```C
#include <winsock.h>
```
Under Linux, you need to include header file: arpa/inet.h
```C
#include <arpa/inet.h>
```
