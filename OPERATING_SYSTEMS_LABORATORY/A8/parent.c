#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include<sys/wait.h>

int main()
{
    // Open a file
    key_t key = ftok("/usr/bin", 1);
    int shmid = shmget(key, 1024, 0666 | IPC_CREAT);
    char *str = (char *)shmat(shmid, (void *)0, 0);

    if (fork() == 0)
    {
        char shmid_str[10];
        sprintf(shmid_str, "%d", shmid);
        char *args[] = {"./child", shmid_str, NULL};
        execvp(args[0], args);
        fprintf(stderr, "Failed to execute child\n");
    }
    wait(NULL);
    printf("Data read from shared memory: %s\n", str);
    
}
