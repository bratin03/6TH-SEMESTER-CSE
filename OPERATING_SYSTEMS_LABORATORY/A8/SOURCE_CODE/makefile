CC = gcc
CFLAGS = -Wall -Wextra -g

all: master mmu sched process
	./master

master: master.c
	$(CC) $(CFLAGS) $< -o $@

mmu: mmu.c
	$(CC) $(CFLAGS) $< -o $@

sched: sched.c
	$(CC) $(CFLAGS) $< -o $@

process: process.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f master mmu sched process
