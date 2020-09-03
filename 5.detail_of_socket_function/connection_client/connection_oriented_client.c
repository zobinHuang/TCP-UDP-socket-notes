//connection_oriented_client.c: After establishing connection with remote server, send a mesaage to server then wait for a ACK reply
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
#define CLIENT_IP "127.0.0.1"
#define CLIENT_TCP_PORT 0x1235

void printf_error();

int main()
{
    int retval;
    int s;
    char recvbuf[128];
    char sendbuf[128];
    struct sockaddr_in client_addr, remote_addr;

    #ifdef _WIN32
        WSAData wsa;
        WSAStartup(0x101, &wsa);
    #endif

    //1. Function: socket(), change the parameter and observe the return value and error code.
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        printf_error();
        goto exit;
    }

    //bind
    client_addr.sin_family = AF_INET;
    #ifdef _WIN32
        //client_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
        client_addr.sin_addr.S_un.S_addr = inet_addr(CLIENT_IP);
    #elif __linux__
        //client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        client_addr.sin_addr.s_addr = inet_addr(CLIENT_IP);
    #endif
    client_addr.sin_port = htons(CLIENT_TCP_PORT);
    retval = bind(s, (struct sockaddr*)&client_addr, sizeof(client_addr));
    if(retval < 0){
        printf_error();
        goto exit;
    }

    //connect
    remote_addr.sin_family = AF_INET;
    #ifdef _WIN32
        //remote_addr.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);
        remote_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
    #elif __linux__
        //remote_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        remote_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    #endif
    remote_addr.sin_port = htons(SERVER_TCP_PORT);
    retval = connect(s, (struct sockaddr*)&remote_addr, sizeof(remote_addr));
    if(retval < 0){
        printf_error();
        goto exit;
    }

    //send
    memset(sendbuf, '\0', sizeof(sendbuf));
    strcpy(sendbuf, "Hi!");
    retval = send(s, sendbuf, strlen(sendbuf) + 1, 0);
    if(retval < 0){
        printf_error();
        goto exit;
    }
    fprintf(stdout, "Successfully send data: %s, data length: %d\n", sendbuf, retval);

    //receive
    memset(recvbuf, '\0', sizeof(recvbuf));
    retval = recv(s, recvbuf, sizeof(recvbuf), 0);
    if(retval < 0){
        printf_error();
        goto exit;
    }
    fprintf(stdout, "Successfully recv data: %s, data length: %d\n", recvbuf, retval);

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