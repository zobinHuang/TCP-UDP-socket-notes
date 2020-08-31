CLIENT-2-OBJ = client_2.o
CC = gcc

client_2: $(CLIENT-2-OBJ)
	$(CC) -o client_2 $(CLIENT-2-OBJ)

$(CLIENT-2-OBJ): client_2.c
	$(CC) -c client_2.c

#clean
.PHONY : clean
clean:
	rm client_2 *.o