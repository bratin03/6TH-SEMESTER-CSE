#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/sem.h>

int main()
{
    int key = (key_t)20;
    int nsem = 1;
    int semid = semget(key, nsem, 0666 | IPC_CREAT);
    semctl(semid, 0, SETVAL, 1);
    int retval = semctl(semid, 0, GETVAL, 0);
    printf("Retval: %d\n", retval);
}

/*
Retval: 1
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>

int main()
{
    int key = (key_t)20;
    int nsem = 1;
    int semid = semget(key, nsem, 0666 | IPC_CREAT);
    int retval = semctl(semid, 0, GETPID, 0);

    printf("PID reutrn by semctl is %d and current pid is %d\n", retval, getpid());

    semctl(semid, 0, SETVAL, 1);
}

/*
PID reutrn by semctl is 123210 and current pid is 125102
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>

int main()
{
    int key = (key_t)21;
    int nsem = 1;
    int semid = semget(key, nsem, 0666 | IPC_CREAT);
    int retval = semctl(semid, 0, GETPID, 0);

    printf("PID reutrn by semctl is %d and current pid is %d\n", retval, getpid());

    semctl(semid, 0, SETVAL, 1);
}

/*
PID reutrn by semctl is 0 and current pid is 125586
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/shm.h>

int main()
{
    int key = ftok("/tmp", 'A');

    __u_short val[5] = {1, 6, 8, 11, 13}, retval[5];

    int semid = semget(key, 5, 0666 | IPC_CREAT);
    if (semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
        struct seminfo *__buf;
    };

    union semun arg;
    arg.array = val;

    semctl(semid, 0, SETALL, arg);
    semctl(semid, 0, GETALL, retval);
    printf("retval[0]=%d retval[1]=%d retval[2]=%d retval[3]=%d retval[4]=%d\n", retval[0], retval[1], retval[2], retval[3], retval[4]);

    return 0;
}

/*
retval[0]=1 retval[1]=6 retval[2]=8 retval[3]=11 retval[4]=13
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/shm.h>

int main()
{
    struct sembuf sop;
    int semid = semget(20, 1, IPC_CREAT | 0666);
    semctl(semid, 0, SETVAL, 2);
    sop.sem_num = 0;
    sop.sem_op = 3;
    sop.sem_flg = 0;
    semop(semid, &sop, 1);
    printf("Will I be able to reach here? :)\n");
    printf("Semaphore value: %d\n", semctl(semid, 0, GETVAL, 0));
    exit(EXIT_SUCCESS);
}

/*
Will I be able to reach here? :)
Semaphore value: 5
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/shm.h>

int main()
{
    struct sembuf sop;
    int semid = semget(20, 1, IPC_CREAT | 0666);
    semctl(semid, 0, SETVAL, 5);
    sop.sem_num = 0;
    sop.sem_op = -3;
    sop.sem_flg = 0;
    semop(semid, &sop, 1);
    printf("Will I be able to reach here? :)\n");
    printf("Semaphore value: %d\n", semctl(semid, 0, GETVAL, 0));
    exit(EXIT_SUCCESS);
}

/*
Will I be able to reach here? :)
Semaphore value: 2
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/shm.h>

int main()
{
    struct sembuf sop;
    int semid = semget(20, 1, IPC_CREAT | 0666);
    semctl(semid, 0, SETVAL, 5);
    sop.sem_num = 0;
    sop.sem_op = -6;
    sop.sem_flg = 0;
    semop(semid, &sop, 1);
    printf("Will I be able to reach here? :)\n");
    printf("Semaphore value: %d\n", semctl(semid, 0, GETVAL, 0));
    exit(EXIT_SUCCESS);
}

/*
^C
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/shm.h>

int main()
{
    struct sembuf sop;
    int semid = semget(20, 1, IPC_CREAT | 0666);
    semctl(semid, 0, SETVAL, 0);
    sop.sem_num = 0;
    sop.sem_op = 0;
    sop.sem_flg = 0;
    semop(semid, &sop, 1);
    printf("Will I be able to reach here? :)\n");
    printf("Semaphore value: %d\n", semctl(semid, 0, GETVAL, 0));
    exit(EXIT_SUCCESS);
}

/*
Will I be able to reach here? :)
Semaphore value: 0
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/shm.h>

int main()
{
    struct sembuf sop;
    int semid = semget(20, 1, IPC_CREAT | 0666);
    semctl(semid, 0, SETVAL, 2);
    sop.sem_num = 0;
    sop.sem_op = 0;
    sop.sem_flg = 0;
    semop(semid, &sop, 1);
    printf("Will I be able to reach here? :)\n");
    printf("Semaphore value: %d\n", semctl(semid, 0, GETVAL, 0));
    exit(EXIT_SUCCESS);
}

/*
^C
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/shm.h>

int main()
{
    struct sembuf sop;
    int semid = semget(20, 1, IPC_CREAT | 0666);
    semctl(semid, 0, SETVAL, 2);
    sop.sem_num = 0;
    sop.sem_op = -3;
    sop.sem_flg = 0;
    if (fork() == 0)
    {
        sleep(2);
        sop.sem_num = 0;
        sop.sem_op = 1;
        sop.sem_flg = 0;
        semop(semid, &sop, 1);
        printf("Child will wake up the parent\n");
        exit(EXIT_SUCCESS);
    }
    semop(semid, &sop, 1);
    printf("Will I be able to reach here? :)\n");
    printf("Semaphore value: %d\n", semctl(semid, 0, GETVAL, 0));
    exit(EXIT_SUCCESS);
}

/*
Child will wake up the parent
Will I be able to reach here? :)
Semaphore value: 0
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/shm.h>

int main()
{
    struct sembuf sop;
    int semid = semget(20, 1, IPC_CREAT | 0666);
    semctl(semid, 0, SETVAL, 2);
    sop.sem_num = 0;
    sop.sem_op = -4;
    sop.sem_flg = 0;
    if (fork() == 0)
    {
        sleep(2);
        sop.sem_num = 0;
        sop.sem_op = 1;
        sop.sem_flg = 0;
        semop(semid, &sop, 1);
        printf("Child will wake up the parent\n");
        exit(EXIT_SUCCESS);
    }
    semop(semid, &sop, 1);
    printf("Will I be able to reach here? :)\n");
    printf("Semaphore value: %d\n", semctl(semid, 0, GETVAL, 0));
    exit(EXIT_SUCCESS);
}

/*
Child will wake up the parent
^C
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/shm.h>

int main()
{
    struct sembuf sop;
    int semid = semget(20, 1, IPC_CREAT | 0666);
    semctl(semid, 0, SETVAL, 2);
    sop.sem_num = 0;
    sop.sem_op = 0;
    sop.sem_flg = 0;
    if (fork() == 0)
    {
        sleep(2);
        sop.sem_num = 0;
        sop.sem_op = 1;
        sop.sem_flg = 0;
        semop(semid, &sop, 1);
        printf("Child will wake up the parent\n");
        exit(EXIT_SUCCESS);
    }
    semop(semid, &sop, 1);
    printf("Will I be able to reach here? :)\n");
    printf("Semaphore value: %d\n", semctl(semid, 0, GETVAL, 0));
    exit(EXIT_SUCCESS);
}

/*
Child will wake up the parent
^C
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/shm.h>

int main()
{
    struct sembuf sop;
    int semid = semget(20, 1, IPC_CREAT | 0666);
    semctl(semid, 0, SETVAL, 2);
    sop.sem_num = 0;
    sop.sem_op = 0;
    sop.sem_flg = 0;
    if (fork() == 0)
    {
        sleep(2);
        sop.sem_num = 0;
        sop.sem_op = -2;
        sop.sem_flg = 0;
        semop(semid, &sop, 1);
        printf("Child will wake up the parent\n");
        exit(EXIT_SUCCESS);
    }
    semop(semid, &sop, 1);
    printf("Will I be able to reach here? :)\n");
    printf("Semaphore value: %d\n", semctl(semid, 0, GETVAL, 0));
    exit(EXIT_SUCCESS);
}

/*
Child will wake up the parent
Will I be able to reach here? :)
Semaphore value: 0
*/