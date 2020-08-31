//example1.c: create a socket, observe the allocated id of socket (try five times, and run socket() alternately with example2), and send message to server1 then recv answer.
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

#define LOCAL_IP "127.0.0.1"
#define LOCAL_PORT 0x1011
#define REMOTE_IP "127.0.0.1"
#define REMOTE_PORT 0x1234

void printf_error();

int main()
{
    int retval;
    int s;
    int i = 0;

    char recvbuf[128];
    char sendbuf[128];

    struct sockaddr_in remote_addr;

    #ifdef _WIN32
        WSAData wsa;
        WSAStartup(0x101, &wsa);
    #endif
    
    while(i++ < 5){
        s = socket(AF_INET, SOCK_STREAM, 0);
        if (s < 0) {
            printf_error();
            goto exit;
        }
    }
    
    /*
        //bind
        struct sockaddr_in local_addr;
        local_addr.sin_family = AF_INET;
        #ifdef _WIN32
            //local_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
            local_addr.sin_addr.S_un.S_addr = inet_addr(LOCAL_IP);
        #elif __linux__
            //local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
            local_addr.sin_addr.s_addr = inet_addr(LOCAL_IP);
        #endif
        local_addr.sin_port = htons(LOCAL_PORT);
        retval = bind(s, (struct sockaddr*)&local_addr, sizeof(local_addr));
        if(retval < 0){
            printf_error();
            goto exit;
        }
    */

    remote_addr.sin_family = AF_INET;
    #ifdef _WIN32
        //remote_addr.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);
        remote_addr.sin_addr.S_un.S_addr = inet_addr(REMOTE_IP);
    #elif __linux__
        //remote_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        remote_addr.sin_addr.s_addr = inet_addr(REMOTE_IP);
    #endif
    remote_addr.sin_port = htons(REMOTE_PORT);

    //connect
    retval = connect(s, (struct sockaddr*)&remote_addr, sizeof(remote_addr));
    if (retval < 0) {
            printf_error();
            goto exit;
        }
    
    //send
    memset(sendbuf, '\0', sizeof(sendbuf));
    strcpy(sendbuf, "Hi! I'm example 1");
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
        int retval = WSAGetLastError();
        fprintf(stderr, "socket error: %d\n", retval);
    #elif __linux__
        fprintf(stderr, "socket error: %s(errno: %d)\n",strerror(errno),errno);
    #endif
}
