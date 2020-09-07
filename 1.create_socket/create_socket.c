//create_socket.c: successfully create a socket
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

#define MY_IP "127.0.0.1"
#define MY_PORT 0x1234

void printf_error();

int main()
{
    int retval, len;
    int s;

    struct sockaddr_in my_addr;

    #ifdef _WIN32
        WSAData wsa;
        WSAStartup(0x101, &wsa);
    #endif
    
    //1. create socket
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        printf_error();
        goto exit;
    }
    fprintf(stdout, "create socket successfully! Socket id: %d\n", s);
    
    //2. bind socket to a socket address
    //2.1 configure the socket address
    my_addr.sin_family = AF_INET;
    #ifdef _WIN32
        //my_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
        my_addr.sin_addr.S_un.S_addr = inet_addr(MY_IP);
    #elif __linux__
        //my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        my_addr.sin_addr.s_addr = inet_addr(MY_IP);
    #endif
    my_addr.sin_port = htons(MY_PORT);
    
    //2.2 bind
    len = sizeof(my_addr);
    retval = bind(s, (struct sockaddr*)&my_addr, len);
    if (s < 0) {
        printf_error();
        goto exit;
    }
    fprintf(stdout, "Successfully bind socket to address:\n    IP: %s\n    Port: %d\n", inet_ntoa(my_addr.sin_addr) ,ntohs(my_addr.sin_port));

    //3. close socket
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
