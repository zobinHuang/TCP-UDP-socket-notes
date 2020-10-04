# **Section 3: Shutdown of Communication**
For this section, you need to know followings:

Note that when we discuss **Shutdown of Communication**, we're talking about different ways to close a connection; that is to say only **connection-oriented C/S** will be discussed in this section. 
## "Rough Closure"
When a client process invokes *close()* (under Linux) or *closesocket()* (under Windows), or even the user just directly kill the client process, we call such a situation **Rough Closure**. So how server process know and deal with these situations? Let's do some observation first.
### (1) Invoke ***close()**(under Linux)*  or ***closesocket()**(under Windows)* 
Above all, to intuitively see what's going on during these operations, we strongly recommend you to install a network packet capture software such as WireShark and run the following programs under debug mode. \
Firstly, compile and run the server process under *./rough/server/server*, and the first client process under *./rough/client/client_1*. Note that We did this experient under Windows. \
If it works, you can capture a *TCP_FIN* from client to server after the client invoking *closesocket()*, see below:\
![image](https://github.com/zobinHuang/TCP-UDP-socket-notes/blob/master/0.diagram/sec3/sec3-1.png) \
And cpature another *TCP_FIN* from server to client after the server invoking *recv()* to try to receive data from client, see below: \
![image](https://github.com/zobinHuang/TCP-UDP-socket-notes/blob/master/0.diagram/sec3/sec3-2.png) \
### (2) Directly kill the client process
