# /**********************************************************
#                     Student Information:
# -----------------------------------------------------------
#                     Name: Bratin Mondal
#                     Student ID: 21CS10016
#                     Name: Somya Kumar
#                     Student ID: 21CS30050
# -----------------------------------------------------------
#         Department of Computer Science and Engineering,
#         Indian Institute of Technology Kharagpur.
# ***********************************************************/

CC = gcc
CFLAGS = -Wall -Wextra
TARGETS = server client

all: $(TARGETS)

server: simDNSServer.c
	$(CC) $(CFLAGS) -o $@ $^

client: simDNSClient.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS) $(TARGETS_PARALLEL)
