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
The **Server** process will call ***accept()*** to return **new socket**s which had finished connection establishment from *Finished Queue*. Note that these new sockets bind to the same local socket address as the **main socket** binds to. You may wonder how multiple sockets work on the same local socket address. Actually the new socket records the information of the remote socket such as IP address, port number and protocol (i.e the triple we have mentioned above), etc. So according to the remote information, the TCP processing module is able to deliver the data to the correct socket even though there exists another socket that work on the same local socket address. In all, this is a very interesting and important point: **A quintuple uniquely defines a connection on the Internet —— (local IP, remote IP, local port, remote port, protocol)**. \
After establishing connections, **Server** can use ***send()*** and ***recv()*** to send and receive data to&from **Client**. Finally, calling ***closesocket()*** (under Windows) or ***close()*** (under Linux) to close the socket.

* Client Side: \
After creating the socket and binding to a local socket address (Actually, the **Client** process can implicitly bind its socket to a socket address which we will explain later, let's focus on the explicit binding first), **Client** process will call ***connect()*** to start three-way handshaking to server. The process has been mentioned above so we will skip the establishing details (The state transition of **Client**: *CLOSED* -> *SYN-SENT* -> *ESTABLISHED*). Note that if the pending queue of **Server** is full, then the ***connect()*** function will return a time-out error since the **Server** will ignore the *SYN* which the **Client** sent in such a case. \
After successfully connecting to **Server**, **Client** can also use ***send()*** and ***recv()*** to interact with the **Server** process, and calls ***closesocket()*** (under Windows) or ***close()*** (under Linux) to close the socket finally.

### (2) Connectionless-oriented
Connectionless-oriented service is much more easy than connection-oriented one. The common connectionless-oriented transport layer protocol is UDP. It doesn't require any connection establishment in advance. The **Client** process only needs to set up the remote socket address of server and send data/requests to it. The **Server** will receive data/request from its **main socket**, and it will know who send these data by reading the parameter in the function ***recvfrom()*** which we will explain later, and then conducts corresponding processing and response to the **Client** process. The whole workflow is as shown as below: \
![image](https://github.com/zobinHuang/TCP-UDP-socket-notes/blob/master/0.diagram/sec2/2-6.png) \
We will skip some details since we have explained connection-oriented service in detail above, so it's easy to understand how connectionless-oriented works.

## 3. What is the difference between **Stream Service** and **Datagram Service**?
![image](https://github.com/zobinHuang/TCP-UDP-socket-notes/blob/master/0.diagram/sec2/2-7.png) \
As Shown in the figure, 
* TCP Stream Service: 
    * The sender just writes the data into the TCP send buffer, and then on the transport layer, it will pack the data from the send buffer into segments and sends it out. On the other side, the receiver's transport layer entity will read the received segments and write into the buffer (in order), and its application process can fetch the data from the buffer by calling ***recv()*** which we will explain in detail later.
    * You can see, the data is not clearly divided on the application layer, so that's why we call it "Data Byte Stream".
    * Since TCP provides the reliability of data transmission on transport layer, so it's suitable for TCP to offer stream service.
* UDP Datagram Service:
    * Different from the stream service, in datagram service, the sender will packs the data into a datagram directly on the application layer. On the receiving side, the application process receives individual packed datagrams by calling ***recvfrom()*** (which we will also explain later) instead of reading receiving buffer. Note that the order of received datagram may be out of order.
    * You can see, the data is clearly divided on application layer in datagram service. So how many times the sender calls ***sendto()*** (which we will also explain later) to send datagrams, the receiver needs to call ***recvfrom()*** for the same times to receive all sent datagram.

## 4. Basic Socket-related Interface Functions
It's very important for socket programmers to know how these common-used socket interface functions work. I mean it would be perfect to know details of them as many as you can, and I will keep to update this subsection once I figure out more details of them. \
*Note that before reading this subsection, you must ensure you're familiar with things that we explained in the former subsections, or this subsection will be meaningless.*
### 4.1 Connection-oriented commonly used:
#### 4.1.1 Server Side:
```C
int listen(int sockfd, int backlog);
```
* Parameters:
    * **sockfd:** socket index.
    * **backlog:** The maximum length of the pending queue.
* Return Value: 
    * If successfully set as listen , it will return 0.
    * If it failed, it will return -1.\
    Under Windows, you can use WSAGetLastError() to get the error code. \
    Under Linux, you can get the error code via errno.
* Function: For TCP socket, set it as *LISTEN* state.
* Notes:
    * The socket must explicitly bind to a local socket address before calling ***listen()***.
    * If the parameter **backlog** equals 0, this socket can still accept one connection.

```C
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```
* Parameters:
    * **sockfd:** socket index.
    * ***addr:** The socket address of newly accepted socket. Usually pass a struct sockaddr_in, then cast to struct sockaddr, and pass the pointer as this parameter (a.k.a (struct sockaddr\*)&sockaddr_in).
    * ***addrlen:** NOTE! This is the ***address*** of the int which represents the sizeof **addr**.
* Return value: 
    * If accept successfully, it will return a new socket which connect to the client.
    * If it failed, it will return -1.\
    Under Windows, you can use WSAGetLastError() to get the error code. \
    Under Linux, you can get the error code via errno.
* Function: Return a **new socket** from *Finished Queue* of the socket's pending queue.
* Notes:
    * The socket must set as *LISTEN* state before calling ***accept()***.
    * For blocking sockets, if the *Finished Queue* is empty (aka no connection request) while calling ***accept()***, the socket will block.

#### 4.1.2 Client Side:
```C
int connect(int sockfd, struct sockaddr *addr, int addrlen);
```
* Parameters:
    * **sockfd:** socket index
    * ***addr:**  a pointer to struct sockaddr which include the information of remote server 
    * **addrlen:** length of **addr** 
* Return value:
    * If connect successfully, it will return 0
    * If it failed, it will returns -1.\
    Under Windows, you can use WSAGetLastError() to get the error code. \
    Under Linux, you can get the error code via errno.
* Function: 
    *Actually, ***connect()*** can be used both in connection-oriented and connectionless-oriented communication. See below:*
    * For TCP sockets, the ***connect()*** call attempts to establish a connection between two sockets. The ***connect()*** call performs two tasks when called for a stream socket. First, it completes the binding necessary for a stream socket (in case it has not been previously bound using the ***bind()*** call). Second, it attempts to make a connection to another socket.
    * For UDP sockets, the ***connect()*** call specifies the peer for a socket (i.e. ipaddr, port, protocol, etc). 
* Notes:
    * The socket must explicitly bind to a local socket address before calling ***connect()***.
#### 4.1.3 Server & Client Common:
```C
int send(int sockfd, const char *buf, int len, int flags);
```
* Parameters:
    * **sockfd:** send socket index.
    * ***buf:** indicates a buffer that stores the send data.
    * **len:** the number of bytes of data that desire to be sent.
    * **flags:** commonly set as 0.
* Execution process (blocked mode):
    * Firstly, it will compare the length of the data to be sent (**len**) and the length of the send buffer of the socket (**sockfd**). If **len** is greater than the length of the send buffer of (**sockfd**), the function returns *SOCKET_ERROR*.
    * If **len** is less than or equal to the length of the send buffer of **sockfd**, then it will check whether the kernel is sending the data in the sending buffer of **sockfd**. If so, wait for the send process to complete.
    * If the kernel has not started sending the data in the sending buffer of **sockfd** or there is no data in the buffer, then it will compare the remaining space of **sockfd**'s send buffer with **len**. If **len** is larger than the size of the remaining space, then it will wait for the kernel to send the data in the buffer of **sockfd**. Otherwise, it will copy the data in **buf** to the send buffer of **sockfd**.
* Return value:
    * If it successfully copies data from **buf** to the send buffer of **sockfd**, it will return the actual number of bytes it copied.
    * If an error happened during copying data, it will return *SOCKET_ERROR*.
    * If the network was disconnected while waiting for the kernel to send data, 
      * under Windows: it will return 0.
```C
int recv( SOCKET sockfd, char *buf, int len, int flags);
```
* Parameters:
    * **sockfd:** index of recv socket.
    * ***buf:** indicates a buffer that want to store the receiving data.
    * **len:** the length of **buf**.
    * **flags:** commonly set as 0.
* Execution process (blocked mode):
    * Firstly, it will wait for the data in the sending buffer of **sockfd** to be snet by the kernel. If a network error happened during transmition, then it will returns *SOCKET_ERROR*.
    * If there is no data in **sockfd**'s sending buffer or the data is successfully sent by the kernel, then it will check the receive buffer of **sockfd**. If there is no data in the receive buffer or the kernel is receiving data, then it will wait until the data is completly received by the kernel.
    * After the kernel received the data, then it will copy the data in the receive buffer of **sockfd** to **buf** (note that the data received by the kernel may be larger than the length of **buf** (**len**), so in this case, it is necessary to call the recv() for several times to copy all the data in the receive buffer of **sockfd**.
* Return value:
    * If it successfully copies data from the send buffer of **sockfd** to **buf**, it will return the actual number of bytes it copied.
    * If an error happened during copying data, it will return *SOCKET_ERROR*.
    * If the network was disconnected while waiting for the kernel to receiving data, it will return 0.

### 4.2 Connectionless-oriented commonly used:
#### 4.2.1 Server & Client Common:
```C
int sendto (int sockfd, const void *buf, int len, unsigned int flags, const struct sockaddr *dest_addr, int addrlen);
```
* Parameters:
    * **sockfd:** send socket index.
    * ***buf:** indicates the buf that stores the data to be sent.
    * **len:** indicates the length of the data to send.
    * **flag:** commonly set as 0.
    * ***dest_addr:** address of a struct sockaddr that contains the destinated host's ip address and port number.
    * **addrlen:** length of **dest_addr**
* Return value:
    * If it successfully copies data from **buf** to the send buffer of **sockfd**, it will return the actual number of bytes it copied.
    * If an error happened during copying data, it will return -1. 
    Under Windows, you can use WSAGetLastError() to get the error code. \
    Under Linux, you can get the error code via errno.
```C
int recvfrom(int sockfd, void *buf, int len, unsigned int flags, struct sockaddr *sour_addr, int *addrlen);
```
* Parameters:
  * **sockfd:** send socket index.
  * ***buf:** indicates the buf that wants to store the recv data.
  * **len:** indicates the length of **buf**.
  * **flag:** commonly set as 0.
  * ***sour_addr:** address of a struct sockaddr that contains the source host's ip address and port number.
  * ***addrlen:** NOTE! This is the ***address*** of the int which represents the length of **sour_addr**
* Return value:
    * If it successfully receive data and store in **buf** from the receive buffer of **sockfd**, it will return the actual number of bytes it copied.
    * If an error happened during copying data, it will return -1.
    Under Windows, you can use WSAGetLastError() to get the error code. \
    Under Linux, you can get the error code via errno.

Now you can turn to the code to check for:
* How Connection-oriented C/S works. Code's in the folder ***connection_client*** & ***connection_server***
* How Connectionless-oriented C/S works. Code's in the folder ***connectionless_client*** & ***connectionless_server***

## **Appendix:**
* [**Error Code Table (Windows & Linux)**](https://github.com/zobinHuang/TCP-UDP-socket-notes/blob/master/2.cs_model/error_code.md)

