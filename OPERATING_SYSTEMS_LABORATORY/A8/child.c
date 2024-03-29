#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include<sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_descriptor>\n", argv[0]);
        return 1;
    }

    // Convert the argument to an integer
    int fd = atoi(argv[1]);
    char *buffer=(char*)shmat(fd,0,0);
    sprintf(buffer, "Hello from child with file descriptor %d\n", fd);
    write(fd, buffer, strlen(buffer));
    exit(0);
}
