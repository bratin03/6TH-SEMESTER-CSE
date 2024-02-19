# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra

# Default target (all) builds both server and client
all: server.out client.out

# Target to build and run the server
server.out: file_server.c
	$(CC) $(CFLAGS) -o server.out file_server.c
	./server.out

# Target to build and run the client
client.out: file_client.c
	$(CC) $(CFLAGS) -o client.out file_client.c
	./client.out

# Target to build the comparison tool
compare.out: compare.c
	$(CC) $(CFLAGS) -o compare.out compare.c

# Target to clean up generated files
clean:
	rm -f *.out *.enc 127.*

