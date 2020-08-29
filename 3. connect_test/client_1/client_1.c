//client_1.c: connect to another client, after successfully connected send and receive message.
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

#define CLIENT_2_IP "127.0.0.1"
#define CLIENT_2_TCP_PORT 0x1234
#define CLIENT_1_IP "127.0.0.1"
#define CLIENT_1_TCP_PORT 0X1235

void printf_error();

int main()
{
    int retval;
    int s;
    int connum = 0;
    char recvbuf[128];
    char sendbuf[128];
    struct sockaddr_in client_addr, remote_addr;

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
    client_addr.sin_family = AF_INET;
    #ifdef _WIN32
        //client_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
        client_addr.sin_addr.S_un.S_addr = inet_addr(CLIENT_1_IP);
    #elif __linux__
        //client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        client_addr.sin_addr.s_addr = inet_addr(CLIENT_1_IP);
    #endif
    client_addr.sin_port = htons(CLIENT_1_TCP_PORT);
    retval = bind(s, (struct sockaddr*)&client_addr, sizeof(client_addr));
    if(retval < 0){
        printf_error();
        goto exit;
    }

    remote_addr.sin_family = AF_INET;
    #ifdef _WIN32
        //remote_addr.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);
        remote_addr.sin_addr.S_un.S_addr = inet_addr(CLIENT_2_IP);
    #elif __linux__
        //remote_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        remote_addr.sin_addr.s_addr = inet_addr(CLIENT_2_IP);
    #endif
    remote_addr.sin_port = htons(CLIENT_2_TCP_PORT);

    //connect
    do{
		retval = connect(s, (struct sockaddr*)&remote_addr, sizeof(remote_addr));
		if(retval < 0){
			printf_error();
			connum++;
		}
	}while(retval != 0);
    fprintf(stdout, "Successfully connected, try %d times\n", connum);

    //send
    memset(sendbuf, '\0', sizeof(sendbuf));
    strcpy(sendbuf, "Hi! I'm client 1");
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