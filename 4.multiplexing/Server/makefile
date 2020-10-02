CONNECT-CLIENT-OBJ = server.o SockList.o
CC = gcc

server: $(CONNECT-CLIENT-OBJ)
	$(CC) -o server $(CONNECT-CLIENT-OBJ)

server.o: server.c SockList.h
	$(CC) -c server.c

SockList.o: SockList.c
	$(CC) -c SockList.c

#clean
.PHONY : clean
clean:
	rm server *.o