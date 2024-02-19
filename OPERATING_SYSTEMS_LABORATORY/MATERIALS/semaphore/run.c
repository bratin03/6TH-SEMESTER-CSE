#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    struct sembuf sop;
    int semid = semget(0, 1, IPC_CREAT | 0666);
    sop.sem_num = 0;
    sop.sem_op = 0;
    sop.sem_flg = 0;
    semop(semid, &sop, 1);
}