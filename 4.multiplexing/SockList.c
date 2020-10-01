#include "SockList.h"

void init_list(socket_list *list)
{
	list->MainSock = 0;
	list->num = 0;
	for(int i = 0; i<SOCKETLIST_LENGTH; i++){
		list->sock_array[i] = 0;
	}
}

void insert_list(int s, socket_list *list)
{
    int i;
	for(i = 0; i<SOCKETLIST_LENGTH; i++){
		if(list->sock_array[i] == 0){
			list->sock_array[i] = s;
			list->num += 1;
			break;
		}
	}
}

void delete_list(int s, socket_list *list)
{
	int i;
	for(i = 0; i<SOCKETLIST_LENGTH; i++){
		if(list->sock_array[i] == s){
			list->sock_array[i] = 0;
			list->num -= 1;
			break;
		}
	}
}

void make_fdlist(socket_list *list, fd_set *fd_list)
{
	int i;
	FD_SET(list->MainSock,fd_list);
	for(i = 0; i<SOCKETLIST_LENGTH; i++){
		if(list->sock_array[i] > 0){
			FD_SET(list->sock_array[i], fd_list);
		}
	}
}