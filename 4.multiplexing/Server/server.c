//server.c: establishing multiple connections with multiple clients
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "SockList.h"

#ifdef _WIN32
    #include <winsock.h>
    #pragma comment (lib,"wsock32.lib")
    #pragma warning(disable:4996)
#elif __linux__
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <sys/select.h>
    #include <sys/time.h> //to use struct timeval
    #include <fcntl.h> //to use fcntl()
#endif

#define SERVER_IP "127.0.0.1"
#define SERVER_TCP_PORT 0x1234

void printf_error();

int main()
{
    int retval;
    int s, newsock;
    char recvbuf[128];
    char sendbuf[128];
    struct sockaddr_in server_addr, remote_addr;
    socket_list sock_list;
    fd_set readfds, writefds, exceptfds;
	struct timeval timeout;
	
    #ifdef _WIN32
        WSAData wsa;
        WSAStartup(0x101, &wsa);
    #endif
    
    //create socket
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        printf_error();
        goto exit;
    }
    
    //bind
    server_addr.sin_family = AF_INET;
    #ifdef _WIN32
        //server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
        server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
    #elif __linux__
        //server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    #endif
    server_addr.sin_port = htons(SERVER_TCP_PORT);
    retval = bind(s, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(retval < 0){
        printf_error();
        goto exit;
    }

    //set as listen mode
    retval = listen(s, 0);
    if(retval < 0){
        printf_error();
        goto exit;
    }

    //config timeout value
    timeout.tv_sec = 1;
	timeout.tv_usec = 0;

    //config socket list
    init_list(&sock_list);
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);
	sock_list.MainSock = s;
	
    //set mainsock as non-block mode
    #ifdef _WIN32
    unsigned long arg = 1;
	ioctlsocket(sock_list.MainSock, FIONBIO, &arg);
    #elif __linux__
    int flags = fcntl(sock_list.MainSock, F_GETFL, 0);
    fcntl(sock_list.MainSock, F_SETFL, flags|O_NONBLOCK);
    #endif

    while(1){
        /*1.make up state list*/
        make_fdlist(&sock_list, &readfds);
		//make_fdlist(&sock_list, &writefds);
		//make_fdlist(&sock_list, &exceptfds);

        /*2. select*/
        retval = select(0, &readfds, &writefds, &exceptfds, NULL);
        if(retval < 0){
            printf_error();
            goto exit;
        }

        /*3. check whether MainSock is in the readfds, if so, try to accept the connection*/
        if(FD_ISSET(sock_list.MainSock, &readfds)){
            int len = sizeof(remote_addr);
            newsock = accept(s, (struct sockaddr*)&remote_addr, &len);
            if(newsock < 0)
                continue;
            fprintf(stdout, "accept a connection, from %s : %d\n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
            insert_list(newsock, &sock_list);
        }

        /*4. check whether other sockets is in the readfds/writefds/exceptfds, if so, process their read/write/except events*/
        for(int i=0; i<SOCKETLIST_LENGTH; i++){
            if(sock_list.sock_array[i] == 0)
				continue;
            newsock = sock_list.sock_array[i];

            /*check readfds*/
            if(FD_ISSET(newsock, &readfds)){
                //recv data
                memset(recvbuf, '\0', sizeof(recvbuf));
                retval = recv(newsock, recvbuf, sizeof(recvbuf), 0);
                if(retval == 0){
                    #ifdef _WIN32
                        closesocket(newsock);
                    #elif __linux__
                        close(newsock);
                    #endif
                    delete_list(newsock, &sock_list);
                    fprintf(stderr, "close a socket\n");
                    continue;
                }else if(retval == -1){
                    #ifdef _WIN32
                        retval = WSAGetLastError();
                        if(retval == WSAEWOULDBLOCK)//if it is a timeout error
						    continue;
                        closesocket(newsock);
					    continue;
                    #elif __linux__
                        if(errno == EAGAIN)//if it is a timeout error
                            continue;
                        close(newsock);
                    #endif
                    fprintf(stderr, "close a socket\n");
                    delete_list(newsock, &sock_list);
                    continue;
                }
                //show recv data
                recvbuf[retval] = 0;
                fprintf(stdout, "->%s\n", recvbuf);
            }

            /*check writefds*/
            if(FD_ISSET(newsock, &writefds)){
                /*to be added*/
            }

            /*check exceptfds*/
            if(FD_ISSET(newsock, &exceptfds)){
                /*to be added*/
            }
        }

        /*5. Clear all fds*/
        FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_ZERO(&exceptfds);
    }
    
    exit:{
        #ifdef _WIN32
            if(s >= 0){
                closesocket(s);
            }
            WSACleanup();
        #elif __linux__
            if(s >= 0){
                close(s);
            }
        #endif
    }   
}

void printf_error()
{
    #ifdef _WIN32
        int retval = WSAGetLastError();
        fprintf(stderr, "socket error: %d\n", retval);
    #elif __linux__
        fprintf(stderr, "socket error: %s(errno: %d)\n", strerror(errno), errno);
    #endif
}