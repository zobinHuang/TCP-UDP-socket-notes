//connectionless_oriented_client.c: Send Data to server, and receive an ACK from server
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
#define CLIENT_IP "127.0.0.1"
#define CLIENT_UDP_PORT 0X1235
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
        local.sin_addr.S_un.S_addr = inet_addr(CLIENT_IP);
    #elif __linux__
        //local.sin_addr.s_addr = htonl(INADDR_ANY);
        local.sin_addr.s_addr = inet_addr(CLIENT_IP);
    #endif
    local.sin_port = htons(CLIENT_UDP_PORT);
    retval = bind(s, (struct sockaddr*)&local, sizeof(local));
    if(retval < 0){
        printf_error();
        goto exit;
    }

    //set up server address
    remote.sin_family = AF_INET;
    #ifdef _WIN32
        //remote.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);
        remote.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
    #elif __linux__
        //remote.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        remote.sin_addr.s_addr = inet_addr(SERVER_IP);
    #endif
    remote.sin_port = htons(SERVER_UDP_PORT);
    
    //send
    memset(sendbuf, '\0', sizeof(sendbuf));
    strcpy(sendbuf, "Hi!");
    int len = sizeof(remote);
    retval = sendto(s, sendbuf, strlen(sendbuf)+1, 0, (struct sockaddr*)&remote, len);
    if(retval < 0){
        printf_error();
        goto exit;
    }
    fprintf(stdout, "Successfully send data: %s, data length: %d\n", sendbuf, retval);
    
    //receive
    memset(recvbuf, '\0', sizeof(recvbuf));
    len = sizeof(remote);
    retval = recvfrom(s, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&remote, &len);
    if(retval < 0){
        printf_error();
        goto exit;
    }
    fprintf(stdout, "Successfully recv data: %s, data length: %d\n", recvbuf, retval);
    
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