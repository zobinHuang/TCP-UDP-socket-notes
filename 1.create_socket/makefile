#this makefile is just for linux
OBJECTS = create_socket.o
CC = gcc

execute: $(OBJECTS)
	$(CC) -o execute $(OBJECTS)

create_socket.o: create_socket.c
	$(CC) -c create_socket.c

.PHONY : clean
clean:
	rm execute $(OBJECTS)