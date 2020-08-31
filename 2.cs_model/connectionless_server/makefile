CONNECTLESS-SERVER-OBJ = connectionless_oriented_server.o
CC = gcc

connectionless_oriented_server: $(CONNECTLESS-SERVER-OBJ)
	$(CC) -o connectionless_oriented_server $(CONNECTLESS-SERVER-OBJ)

connectionless_oriented_server.o: connectionless_oriented_server.c
	$(CC) -c connectionless_oriented_server.c

#clean
.PHONY : clean
clean:
	rm connectionless_oriented_server *.o