CLIENT-OBJ = client.o
CC = gcc

client: $(CLIENT-OBJ)
	$(CC) -o client $(CLIENT-OBJ)

client.o: client.c
	$(CC) -c client.c

#clean
.PHONY : clean
clean:
	rm client *.o