# /**********************************************************
#                     Student Information:
# -----------------------------------------------------------
#                     Name: Bratin Mondal
#                     Student ID: 21CS10016
# -----------------------------------------------------------
#                     Name: Somya Kumar
#                     Student ID: 21CS30050
# -----------------------------------------------------------
#         Department of Computer Science and Engineering,
#         Indian Institute of Technology Kharagpur.
# ***********************************************************/

# Run as `make all` to compile all the files
# Run './initmsocket' to start the server
# Run './user1' to start the first client
# Run './user2' to start the second client

CC = gcc
CFLAGS = -Wall -Wextra -pedantic -pthread
TARGETS = initmsocket user1 user2 
LIBRARY = libsocket.a
LIBRARY_OBJS = msocket.o

all: $(TARGETS)

$(LIBRARY): $(LIBRARY_OBJS)
	ar rcs $@ $^

initmsocket: initmsocket.c $(LIBRARY)
	$(CC) $(CFLAGS) -o $@ initmsocket.c -L. -lsocket

user1: user1.c $(LIBRARY)
	$(CC) $(CFLAGS) -o $@ user1.c -L. -lsocket

user2: user2.c $(LIBRARY)
	$(CC) $(CFLAGS) -o $@ user2.c -L. -lsocket

.PHONY: clean

clean:
	rm -f $(TARGETS) $(LIBRARY) $(LIBRARY_OBJS)
