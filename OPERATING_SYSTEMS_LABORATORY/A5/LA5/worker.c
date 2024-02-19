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

#define P(s) semop(s, &pop, 1)
#define V(s) semop(s, &vop, 1)

/*
Function Name: main
Input: void
Output: int
Logic: The main function
*/

int main(int argc, char *argv[])
{
    // The main function
    if (argc != 3)
    {
        printf("Error: Invalid number of arguments\n");
        exit(EXIT_FAILURE);
    }

    // Command line arguments
    int n = atoi(argv[1]);
    int node_num = atoi(argv[2]);

    // Shared memory and semaphores
    key_t key = ftok(".", 1000);
    int shmid = shmget(key, n * n * sizeof(int), IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    int *adjacencyMatrix = (int *)shmat(shmid, NULL, 0);

    key_t key2 = ftok(".", 1001);
    int shmid2 = shmget(key2, n * sizeof(int), IPC_CREAT | 0666);
    if (shmid2 < 0)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    int *topologicalOrder = (int *)shmat(shmid2, NULL, 0);

    key_t key3 = ftok(".", 1002);
    int shmid3 = shmget(key3, sizeof(int), IPC_CREAT | 0666);
    if (shmid3 < 0)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    int *idx = (int *)shmat(shmid3, NULL, 0);

    struct sembuf pop, vop;
    pop.sem_num = vop.sem_num = 0;
    pop.sem_flg = vop.sem_flg = 0;
    pop.sem_op = -1;
    vop.sem_op = 1;

    key_t key_mtx = ftok(".", 1003);
    int mtx = semget(key_mtx, 1, IPC_CREAT | 0777);
    key_t key_ntfy = ftok(".", 1004);
    int ntfy = semget(key_ntfy, 1, IPC_CREAT | 0777);

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
    }

    // For incoming edges to the node wait for the signal
    int j = 0;
    for (j = 0; j < n; j++)
    {
        if (adjacencyMatrix[j * n + node_num] == 1)
        {
            P(sem_nodes[node_num]);
        }
    }

    // After all incoming edges are processed, acquire the mutex and update the topological order
    P(mtx);
    topologicalOrder[idx[0]] = node_num;
    idx[0]++;
    V(mtx);

    // Signal the nodes which are dependent on the current node
    for (j = 0; j < n; j++)
    {
        if (adjacencyMatrix[node_num * n + j] == 1)
        {
            V(sem_nodes[j]);
        }
    }
    // Signal the boss that the current node has been processed
    V(ntfy);
    // Detach the shared memory
    shmdt(adjacencyMatrix);
    shmdt(idx);
    shmdt(topologicalOrder);

    exit(EXIT_SUCCESS);
}