# **Section 3: Shutdown of Communication**
For this section, you need to know followings:

Note that when we discuss **Shutdown of Communication**, we're talking about different ways to close a connection; that is to say only **connection-oriented C/S** will be discussed in this section. 
## "Rough Closure"
When a client process invokes *close()* (under Linux) or *closesocket()* (under Windows), or even the user just directly kill the client process, we call such a situation **Rough Closure**. So how server process know and deal with these situations? Let's answer this question through experiments.
### (0) Prepare
Above all, to intuitively see what's going on during these experiments, I strongly recommend you to install a network packet capture software such as WireShark and run the following programs under debug mode.
### (1) Invoke **close()**(under Linux) or **closesocket()**(under Windows)
Firstly, compile and run the server process under *./rough/server/*, and the client process under *./rough/client_1/*. \
As it's shown below, a *TCP_FIN* from client to server is captured after the client invoking *closesocket() / close()*\
![image](https://github.com/zobinHuang/TCP-UDP-socket-notes/blob/master/0.diagram/sec3/sec3-1.png) \
And as we mentioned in Section 2, recv() will return 0 when the peer closed the connection. So when server detects that recv() returns a 0, it will know the client had close the connection, then it will jump out the receiving cycle as you can see in the code. And after the server invoking *closesocket() / close()*, another *TCP_FIN* from server to client is captured, as shown below: \
![image](https://github.com/zobinHuang/TCP-UDP-socket-notes/blob/master/0.diagram/sec3/sec3-2.png) \
So you can see, after invoking *closesocket()/close()*, the process will actively close one-way TCP connection by sending *TCP-FIN*, and the peer process will know this situation by checking the return value of socket functions such as *recv()* and *send()*. So it is a good habit for a socket program to check the return value of these functions to confirm whether the connection is still valid.
### (2) Directly kill the client process
Now we try to directly kill the client process after it establishing connection to the server instead of invoking *closesocket() / close()*. Let's restart the server process under *./rough/server/*, compile and run the client process under *./rough/client_2/*. \
After the client successfully establish connection to the server, try to directly close the terminal, and normally a *TCP_RST* will be captured after this operation, as shown below: \
![image](https://github.com/zobinHuang/TCP-UDP-socket-notes/blob/master/0.diagram/sec3/sec3-3.png) \
And on the server side, normally the *recv()* will return -1. Under windows, you can use WSAGetLastError() to get the error code, and you will get a 10054 error which indicates that "Connection reset by peer". Under Linux, you can get the error code via errno, and you will get a ECONNRESET (104) which indicates the same reason.

