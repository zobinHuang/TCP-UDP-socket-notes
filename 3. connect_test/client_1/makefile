CLIENT-1-OBJ = client_1.o
CC = gcc

client_1: $(CLIENT-1-OBJ)
	$(CC) -o client_1 $(CLIENT-1-OBJ)

$(CLIENT-1-OBJ): client_1.c
	$(CC) -c client_1.c

#clean
.PHONY : clean
clean:
	rm client_1 *.o