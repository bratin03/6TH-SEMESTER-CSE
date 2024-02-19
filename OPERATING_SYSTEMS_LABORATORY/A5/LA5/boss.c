/**********************************************************
                    Student Information:
-----------------------------------------------------------
                    Name: Bratin Mondal
                    Student ID: 21CS10016
-----------------------------------------------------------
        Department of Computer Science and Engineering,
        Indian Institute of Technology Kharagpur.
***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <wait.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define P(s) semop(s, &pop, 1)
#define V(s) semop(s, &vop, 1)

/*
Function Name: isTopologicalOrderCorrect
Input: adjacencyMatrix (int*), topologicalOrder (int*), n (int)
Output: int
Logic: Checks if the given topological order is correct or not
*/

int isTopologicalOrderCorrect(int *adjacencyMatrix, int *topologicalOrder, int n)
{
    int i;
    int j;
    int flag = 1;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            if (i == j)
            {
                continue;
            }
            if (adjacencyMatrix[i * n + j] == 1)
            {
                int i_pos = -1;
                int j_pos = -1;
                int k;
                for (k = 0; k < n; k++)
                {
                    if (topologicalOrder[k] == i)
                    {
                        i_pos = k;
                    }
                    if (topologicalOrder[k] == j)
                    {
                        j_pos = k;
                    }
                }
                if (i_pos == -1 || j_pos == -1 || i_pos >= j_pos)
                {
                    printf("+++ Boss: Edge (%d, %d) violates topological order\n", i, j);
                    flag = 0;
                }
            }
        }
    }

    return flag;
}

/*
Function Name: main
Input: void
Output: int
Logic: The main function
*/

int main()
{
    FILE *file = fopen("graph.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Read the adjacency matrix and allocate shared memory
    int n;
    fscanf(file, "%d", &n);
    key_t key = ftok(".", 1000);
    int shmid = shmget(key, n * n * sizeof(int), IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    int *adjacencyMatrix = (int *)shmat(shmid, NULL, 0);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            fscanf(file, "%d", &adjacencyMatrix[i * n + j]);
        }
    }
    fclose(file);

    // Allocate shared memory for topological order
    key_t key2 = ftok(".", 1001);
    int shmid2 = shmget(key2, n * sizeof(int), IPC_CREAT | 0666);
    if (shmid2 < 0)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    int *topologicalOrder = (int *)shmat(shmid2, NULL, 0);

    // Allocate shared memory for index
    key_t key3 = ftok(".", 1002);
    int shmid3 = shmget(key3, sizeof(int), IPC_CREAT | 0666);
    if (shmid3 < 0)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    int *idx = (int *)shmat(shmid3, NULL, 0);
    idx[0] = 0;

    // Setup semaphores
    struct sembuf pop, vop;
    pop.sem_num = vop.sem_num = 0;
    pop.sem_flg = vop.sem_flg = 0;
    pop.sem_op = -1;
    vop.sem_op = 1;

    // Setup semaphores for synchronization
    key_t key_mtx = ftok(".", 1003);
    int mtx = semget(key_mtx, 1, IPC_CREAT | 0777);
    semctl(mtx, 0, SETVAL, 1);
    key_t key_ntfy = ftok(".", 1004);
    int ntfy = semget(key_ntfy, 1, IPC_CREAT | 0777);
    semctl(ntfy, 0, SETVAL, 0);
    key_t key_nodes[n];
    int sem_nodes[n];
    int i;
    for (i = 0; i < n; i++)
    {
        key_nodes[i] = ftok(".", i);
    }

    for (i = 0; i < n; i++)
    {
        sem_nodes[i] = semget(key_nodes[i], 1, IPC_CREAT | 0777);
        semctl(sem_nodes[i], 0, SETVAL, 0);
    }

    // Wait for all workers to complete
    printf("+++ Boss: Setup done...\n");
    for (i = 0; i < n; i++)
    {
        P(ntfy);
    }

    // Print the topological order
    P(mtx);
    printf("+++ Topological sorting of the vertices\n");
    for (i = 0; i < n; i++)
    {
        printf("%d ", topologicalOrder[i]);
    }
    printf("\n");

    int is_correct = isTopologicalOrderCorrect(adjacencyMatrix, topologicalOrder, n);
    V(mtx);

    // Print the result
    if (is_correct)
    {
        printf("+++ Boss: Well done, my team...\n");
    }
    else
    {
        printf("+++ Boss: Better luck, next time...\n");
    }

    // Detach and remove shared memory and semaphores
    shmdt(adjacencyMatrix);
    shmdt(idx);
    shmdt(topologicalOrder);
    semctl(mtx, 0, IPC_RMID, 0);
    semctl(ntfy, 0, IPC_RMID, 0);
    for (i = 0; i < n; i++)
    {
        semctl(sem_nodes[i], 0, IPC_RMID, 0);
    }
    shmctl(shmid, IPC_RMID, NULL);
    shmctl(shmid2, IPC_RMID, NULL);
    shmctl(shmid3, IPC_RMID, NULL);

    exit(EXIT_SUCCESS);
}
