SERVER-OBJ = server.o
CC = gcc

server: $(SERVER-OBJ)
	$(CC) -o server $(SERVER-OBJ)

server.o: server.c
	$(CC) -c server.c

#clean
.PHONY : clean
clean:
	rm server *.o