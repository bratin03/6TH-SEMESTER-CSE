#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>

int main()
{
    int val;
    scanf("%d", &val);
    int semid = semget(20, 1, 0666 | IPC_CREAT);
    semctl(semid, 0, SETVAL, val);
}