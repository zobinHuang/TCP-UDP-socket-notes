#ifndef _SOCKLIST_H_
#define _SOCKLIST_H_

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
#endif

#define SOCKETLIST_LENGTH 64

typedef struct socket_list{
	int MainSock;
	int num;
	int sock_array[SOCKETLIST_LENGTH];
}socket_list;

void init_list(socket_list *list);
void insert_list(int s, socket_list *list);
void delete_list(int s,socket_list *list);
void make_fdlist(socket_list *list, fd_set *fd_list);

#endif