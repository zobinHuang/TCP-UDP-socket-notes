//connection_oriented_server.c: After establishing a connection and receiving the string sent by the client, send back an ACK string to the client.
#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
    #include <winsock.h>
    #pragma comment (lib,"wsock32.lib")
    #pragma warning(disable:4996)
#elif __linux__
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
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

    //listen
    retval = listen(s, 0);
    if(retval < 0){
        printf_error();
        goto exit;
    }

    while(1){
        //accept
        int len = sizeof(remote_addr);
        newsock = accept(s, (struct sockaddr*)&remote_addr, &len);
        if(newsock < 0){
            printf_error();
            goto exit;
        }
        fprintf(stdout, "Acccept connection: Remote port= %d, ip addr = %s\n", ntohs(remote_addr.sin_port), inet_ntoa(remote_addr.sin_addr));

        //recv
        retval = recv(newsock, recvbuf, sizeof(recvbuf), 0);
        if(retval < 0){
            printf_error();
            goto exit;
        }
        fprintf(stdout, "Reveive data:%s, length:%d\n", recvbuf, retval);

        //send ACK
        memset(sendbuf, '\0', sizeof(sendbuf));
        strcpy(sendbuf, "ACK");
        retval = send(newsock, sendbuf, strlen(sendbuf)+1, 0);
        if(retval < 0){
            printf_error();
            goto exit;
        }
        fprintf(stdout, "Send data:%s, length:%d\n", sendbuf, retval);

        //close socket
        #ifdef _WIN32
            closesocket(newsock);
        #elif __linux__
            close(newsock);
        #endif
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
        fprintf(stderr, "socket error: %s(errno: %d)\n",strerror(errno),errno);
    #endif
}
