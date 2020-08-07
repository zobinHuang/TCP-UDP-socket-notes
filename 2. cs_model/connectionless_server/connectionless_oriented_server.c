//connectionless_oriented_server.c: After receiving data from client, send an ACK to the client
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
    #include <unistd.h> //contains many function prototypes of system services: read(), write(), getpid(), close(), etc.
#endif

#define SERVER_IP "127.0.0.1"
#define SERVER_UDP_PORT 0x1234

void printf_error();

int main()
{
    int retval;
    int s;
    char recvbuf[128];
    char sendbuf[128];
    struct sockaddr_in local, remote;
    #ifdef _WIN32
        WSAData wsa;
        WSAStartup(0x101, &wsa);
    #endif
    
    //create socket
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        printf_error();
        goto exit;
    }

    //bind
    local.sin_family = AF_INET;
    #ifdef _WIN32
        //local.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
        local.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
    #elif __linux__
        //local.sin_addr.s_addr = htonl(INADDR_ANY);
        local.sin_addr.s_addr = inet_addr(SERVER_IP);
    #endif
    local.sin_port = htons(SERVER_UDP_PORT);
    retval = bind(s, (struct sockaddr*)&local, sizeof(local));
    if(retval < 0){
        printf_error();
        goto exit;
    }

    while(1){
        //recv
        int len = sizeof(remote);
        //fprintf(stdout, "sizeof struct sockaddr_in remote before receiving: %d\n", len);
        retval = recvfrom(s, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&remote, &len);
        if(retval < 0){
            printf_error();
            goto exit;
        }
        fprintf(stdout, "Reveive data:%s, length:%d\n", recvbuf, retval);
        len = sizeof(remote);
        //fprintf(stdout, "sizeof struct sockaddr_in remote after receiving: %d\n", len);
        
        //send ACK
        memset(sendbuf, '\0', sizeof(sendbuf));
        strcpy(sendbuf, "ACK");
        retval = sendto(s, sendbuf, strlen(sendbuf)+1, 0, (struct sockaddr*)&remote, len);
        if(retval < 0){
            printf_error();
            goto exit;
        }
        fprintf(stdout, "Send data:%s, length:%d\n", sendbuf, retval);
    }

    exit:{
        #ifdef _WIN32
            if(s > 0){
                closesocket(s);
            }
            WSACleanup();
        #elif __linux__
            if(s > 0){
                close(s);
            }
        #endif
    }
}

void printf_error()
{
    #ifdef _WIN32
        retval = WSAGetLastError();
        fprintf(stderr, "socket error: %d\n", retval);
    #elif __linux__
        fprintf(stderr, "socket error: %s(errno: %d)\n",strerror(errno),errno);
    #endif
}
