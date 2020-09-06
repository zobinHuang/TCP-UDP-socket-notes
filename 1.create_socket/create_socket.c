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
    #include <unistd.h> //contains many function prototypes of system services: read(), write(), getpid(), close(), etc.
#endif

void printf_error();

int main()
{
    int retval;
    int s;

    #ifdef _WIN32
        WSAData wsa;
        WSAStartup(0x101, &wsa);
    #endif
    
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        printf_error();
        goto exit;
    }
    else {
        fprintf(stdout, "create socket successfully! Socket id: %d\n", s);
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
        int retval = WSAGetLastError();
        fprintf(stderr, "socket error: %d\n", retval);
    #elif __linux__
        fprintf(stderr, "socket error: %s(errno: %d)\n",strerror(errno),errno);
    #endif
}
