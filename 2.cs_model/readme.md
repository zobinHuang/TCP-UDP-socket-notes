# Demonstrating Programs of Client/Server Model
For this section, you need to know followings:

## 1. What is Client/Server Model and Why?
**Client/Server** model is the most common used model in the design of network communication. In such a model, the **Client** is the active one and **Server** is the passive one. **Client** process will send requests to **Server** process which will then response corresponding data back to the **Client**. \
You might wonder why such a design makes sense, can both sides of communication are active (i.e. two client processes communicate with each other directly)? In the development of network software, our answer is No and several reasons are listed below.
1. Let's consider the TCP protocol which requires both sides of TCP entities to establish connection (aka three-way handshaking) before they start to transfer data to guarantee reliability, the process as the figure shown below (Souce: **TCP/IP Protocol Suite (Fourth Edition)** by  *Behrouz A. Forouzan*, Page 443). \
![image](https://github.com/zobinHuang/TCP-UDP-socket-notes/blob/master/0.diagram/sec2/2-2.png) \
In common C/S model, **Client** will actively send *SYN* to **Server** to start three-way handshaking and **Server** will passively accept the connection request and start to serve **Client**. **Client** won't response any *SYN* it received and **Server** will never send *SYN* to others. However, TCP standard also described such a rare situation actually: In a no-server situation, two application processes need to simultaneously send *SYN* to start three-way handshaking so that they are able to establish connection eventually, as the figure below illustrates (Souce: **TCP/IP Protocol Suite (Fourth Edition)** by  *Behrouz A. Forouzan*, Page 454). \
![image](https://github.com/zobinHuang/TCP-UDP-socket-notes/blob/master/0.diagram/sec2/2-1.png) \
This model unlikely to make any sense since **usually there is only one active side in network communication**. It's very difficult and wierd to require both sides to actively start establishing connection to another side at the same time. (Consider the phone call, isn't it rediculous that both sides of users need to dial number at the same time?)
2. If a network software adopts no-server design, then it will require every process to record information of other processes' communication endpoint. That will be enormous data occupied a big part of memory space. Not a good idea.

In all, it would be a reasonable design to have a central scheduler (**Server**) to coordinate **Client** processes distributed on different hosts. When the **Client** process needs service, it communicates with the **Server** to obtain the corresponding data (may need to establish a connection in advance). The **Server** is only responsible for responding to the needs of the client, and will not actively interact with the **Client**.

## 2. What do Connection-oriented and Connectionless-oriented refer to?
### (1) Connection-oriented
The common connection-oriented transport layer protocol is TCP which requires the communicating parties to perform a three-way handshake before transmitting data. It's very important to get familiar with the state transition process, check below: (Souce: **TCP/IP Protocol Suite (Fourth Edition)** by  *Behrouz A. Forouzan*, Page 449)\
![image](https://github.com/zobinHuang/TCP-UDP-socket-notes/blob/master/0.diagram/sec2/2-3.png) \
In the common socket programming, the basic connection-oriented C/S workflow is as shown in the figure below. We will explain every step of it along with the figure of state transition process. \
![image](https://github.com/zobinHuang/TCP-UDP-socket-notes/blob/master/0.diagram/sec2/2-4.png)
* Server Side: \
The **Server** will firstly create a **main socket** as the communication endpoint and explicitly bind it to a socket address. Then it will set this main socket as *LISTEN* mode by calling ***listen()***. Once receiving *SYN* on this socket, it will copy a **new socket** structure, then this **new socket** will response *SYN + ACK* and turn to *SYN-RCVD* mode. If the **Client** returns a *ACK*, the **new socket** will get to *ESTABLISHED* mode and three-way handshaking is finished which signs the connection is established. The **Server** process can use this newly generated socket to communicate with remote **Client**. \
There might be some confusions of this **new socket**. Let's make it more clear. You should know that the **Server** process needs to serve multiple **Client**s eventually. So how? The answer is, in the implementation of socket, it uses the **main socket** which the program created actively to receive TCP connection requests (aka *SYN*). Once there is a connection request, it will copy a **new socket** and use it to complete the connection establishment and subsequent data transmission. In this way, the **Server** can interact with multiple **Client**s via multiple **new socket**s. \
There's another point that we need to talk about. In the implementation of socket, there's a pending queue that holds these **new socket**s. The pending queue can be divided into two subqueues as the figure shown below. \
![image](https://github.com/zobinHuang/TCP-UDP-socket-notes/blob/master/0.diagram/sec2/2-5.png) \
(1) The *Unfinished Queue* loads those **new sockets** which have sent *SYN+ACK* after they received *SYN* but still not receive *ACK* from corresponding **Client** process yet (aka *SYN_RCVD* mode). \
(2) The *Finished Queue* loads those **new sockets** which have finished three-way handshaking (aka *ESTABLISHED* mode). \
Note that the length of pending queue depends on the parameter of the function ***listen()*** which we will discuss later. \
The **Server** process will call ***accept()*** to return **new socket**s which had finished connection establishment from *Finished Queue*. Note that these new sockets bind to the same local socket address as the **main socket** binds to. You may wonder how multiple sockets work on the same socket address. Actually the new socket records the information of the remote socket such as IP address, port number and protocol (i.e the triple we have mentioned above), etc. So according to the remote information, the TCP processing module is able to deliver the data to the correct socket even though there exists another socket that work on the same socket address. In all, this is a very interesting and important point: **A quintuple uniquely defines a connection on the Internet —— (local IP, remote IP, local port, remote port, protocol)**. \
After establishing connections, **Server** can use ***send()*** and ***recv()*** to send and receive data to&from **Client**. Finally, calling ***closesocket()*** (under Windows) or ***close()*** (under Linux) to close the socket.

* Client Side: \
After creating the socket and binding to a local socket address (Actually, the **Client** process can implicitly bind its socket to a socket address which we will explain later, let's focus on the explicit binding first), **Client** process will call ***connect()*** to start three-way handshaking to server. The process has been mentioned above so we will skip the establishing details (The state transition of **Client**: *CLOSED* -> *SYN-SENT* -> *ESTABLISHED*). Note that if the pending queue of **Server** is full, then the ***connect()*** function will return a time-out error since the **Server** will ignore the *SYN* which the **Client** sent in such a case. \
After successfully connecting to **Server**, **Client** can also use ***send()*** and ***recv()*** to interact with the **Server** process, and calls ***closesocket()*** (under Windows) or ***close()*** (under Linux) to close the socket finally.

### (2) Connectionless-orienteds
![image](https://github.com/zobinHuang/TCP-UDP-socket-notes/blob/master/0.diagram/sec2/Sequence%20of%20Connectionless-oriented.png)

## 3. Basic Socket-related Interface Functions
### 3.1 Connection-oriented commonly used:
#### 3.1.1 Server Side:
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

#### 3.1.2 Client Side:
```C
int connect(int sockfd, struct sockaddr *addr, int addrlen);
```
* parameters:
    * **sockfd:** socket index
    * ***addr:**  a pointer to struct sockaddr which include the information of remote server 
    * **addrlen:** length of **addr** 
* note:
    *Actually, connect() can also be used in connectionless oriented communication. See below:*
    * For stream sockets, the connect() call attempts to establish a connection between two sockets. The connect() call performs two tasks when called for a stream socket. First, it completes the binding necessary for a stream socket (in case it has not been previously bound using the bind() call). Second, it attempts to make a connection to another socket.
    * For datagram sockets, the connect() call specifies the peer for a socket (i.e. ipaddr & port). 
* return value:
    * If connect successfully, it will return 0
    * If it failed, it will returns -1.\
    Under Windows, you can use WSAGetLastError() to get the error code. \
    Under Linux, you can get the error code via errno.
    
#### 3.1.3 Server & Client Common:
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
### 3.2 Connectionless-oriented commonly used:
#### 3.2.1 Server & Client Common:
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

