CLIENT-OBJ = client.o
CC = gcc

client: $(CONNECT-CLIENT-OBJ)
	$(CC) -o client $(CONNECT-CLIENT-OBJ)

client.o: client.c
	$(CC) -c client.c

#clean
.PHONY : clean
clean:
	rm client *.o