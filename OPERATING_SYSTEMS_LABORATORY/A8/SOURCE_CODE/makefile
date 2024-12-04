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

CC = gcc
CFLAGS = -Wall -Wextra -g 

all: master mmu sched process
	./master

master: Master.c mmu sched process
	$(CC) $(CFLAGS) $< -o $@

mmu: mmu.c
	$(CC) $(CFLAGS) $< -o $@

sched: sched.c
	$(CC) $(CFLAGS) $< -o $@

process: process.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f master mmu sched process

distclean: clean
	rm -f *.txt

## In the Source Code, Use the VERBOSE variable to print the details of the execution
