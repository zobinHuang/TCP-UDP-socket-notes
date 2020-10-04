CONNECT-CLIENT-OBJ = server.o SockList.o
CC = gcc

server: $(CONNECT-CLIENT-OBJ)
	$(CC) -o server $(CONNECT-CLIENT-OBJ)

server.o: server.c SockList.h
	$(CC) -c -g server.c

SockList.o: SockList.c
	$(CC) -c -g SockList.c

#clean
.PHONY : clean
clean:
	rm server *.o