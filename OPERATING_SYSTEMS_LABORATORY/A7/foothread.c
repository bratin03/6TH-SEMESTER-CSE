#define _GNU_SOURCE
#include "foothread.h"
#include <sys/types.h>
#include <sys/sem.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <unistd.h>

static struct sembuf pop, vop;
#define Down(s) semop(s, &pop, 1)
#define Up(s) semop(s, &vop, 1)

typedef struct foothread_table_entry
{
    pid_t tid;
    int join_type;
    int semaphore_id;
    int is_exit;
} foothread_table_entry;

typedef struct foothread_mutex_entry
{
    int sem;
    int is_locked;
    pid_t owner;
} foothread_mutex_entry;

typedef struct foothread_barrier_entry
{
    int sem_mutex;
    int sem_1;
    int sem_2;
    int count;
    int current_count;
} foothread_barrier_entry;

static foothread_table_entry foothread_table[FOOTHREAD_THREADS_MAX];
static foothread_mutex_entry foothread_mutex_table[FOOTHREAD_THREADS_MAX];
static foothread_barrier_entry foothread_barrier_table[FOOTHREAD_THREADS_MAX];
static int count = 0;
static int mutex_count = 0;
static int barrier_count = 0;
static int thread_table_sem, mutex_table_sem, barrier_table_sem;

void foothread_create(foothread_t *thread, foothread_attr_t *attr, int (*start_routine)(void *), void *arg)
{
    if (count >= FOOTHREAD_THREADS_MAX)
    {
        errno = ENOMEM;
        return;
    }
    int stack_size;
    int join_type;
    if (attr == NULL)
    {
        stack_size = FOOTHREAD_DEFAULT_STACK_SIZE;
        join_type = FOOTHREAD_DETACHED;
    }
    else
    {
        stack_size = attr->stack_size;
        join_type = attr->join_type;
    }
    if (count == 0)
    {
        thread_table_sem = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
        if (thread_table_sem == -1)
        {
            return;
        }
        pop.sem_num = 0;
        pop.sem_op = -1;
        pop.sem_flg = 0;
        vop.sem_num = 0;
        vop.sem_op = 1;
        vop.sem_flg = 0;
        semctl(thread_table_sem, 0, SETVAL, 1);
    }
    Down(thread_table_sem);
    char *stack = (char *)malloc(stack_size);
    pid_t tid = clone(start_routine, stack + stack_size, CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD | CLONE_SYSVSEM | CLONE_DETACHED, arg);
    if (tid == -1)
    {
        Up(thread_table_sem);
        return;
    }
    foothread_table[count].tid = tid;
    foothread_table[count].join_type = join_type;
    if (join_type == FOOTHREAD_JOINABLE)
    {
        foothread_table[count].semaphore_id = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
        if (foothread_table[count].semaphore_id == -1)
        {
            Up(thread_table_sem);
            return;
        }
        semctl(foothread_table[count].semaphore_id, 0, SETVAL, 0);
    }
    foothread_table[count].is_exit = 0;
    if (thread != NULL)
    {
        thread->tid = tid;
    }
    count++;
    Up(thread_table_sem);
    return;
}

void foothread_exit()
{
    pid_t tid = gettid();
    pid_t pid = getpid();
    if (pid == tid)
    {
        for (int i = 0; i < count; i++)
        {
            if (foothread_table[i].join_type == FOOTHREAD_JOINABLE)
            {
                Down(foothread_table[i].semaphore_id);
            }
            semctl(foothread_table[i].semaphore_id, 0, IPC_RMID);
        }
        semctl(thread_table_sem, 0, IPC_RMID);
    }
    else
    {
        for (int i = 0; i < count; i++)
        {
            if (foothread_table[i].tid == tid)
            {
                foothread_table[i].is_exit = 1;
                if (foothread_table[i].join_type == FOOTHREAD_JOINABLE)
                {
                    Up(foothread_table[i].semaphore_id);
                }
                break;
            }
        }
    }
}

void foothread_attr_setjointype(foothread_attr_t *attr, int join_type)
{
    attr->join_type = join_type;
}

void foothread_attr_setstacksize(foothread_attr_t *attr, int stack_size)
{
    attr->stack_size = stack_size;
}

void foothread_mutex_init(foothread_mutex_t *mutex)
{
    if (mutex_count == 0)
    {
        mutex_table_sem = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
        if (mutex_table_sem == -1)
        {
            return;
        }
        pop.sem_num = 0;
        pop.sem_op = -1;
        pop.sem_flg = 0;
        vop.sem_num = 0;
        vop.sem_op = 1;
        vop.sem_flg = 0;
        semctl(mutex_table_sem, 0, SETVAL, 1);
    }
    Down(mutex_table_sem);
    mutex->sem = mutex_count;
    foothread_mutex_table[mutex_count].sem = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    if (foothread_mutex_table[mutex_count].sem == -1)
    {
        Up(mutex_table_sem);
        return;
    }
    semctl(foothread_mutex_table[mutex_count].sem, 0, SETVAL, 1);
    foothread_mutex_table[mutex_count].is_locked = 0;
    foothread_mutex_table[mutex_count].owner = 0;
    mutex_count++;
    Up(mutex_table_sem);
}

void foothread_mutex_lock(foothread_mutex_t *mutex)
{

    Down(foothread_mutex_table[mutex->sem].sem);
    foothread_mutex_table[mutex->sem].is_locked = 1;
    foothread_mutex_table[mutex->sem].owner = gettid();
}

void foothread_mutex_unlock(foothread_mutex_t *mutex)
{
    if (foothread_mutex_table[mutex->sem].is_locked == 0 || foothread_mutex_table[mutex->sem].owner != gettid())
    {
        errno = EPERM;
        return;
    }
    foothread_mutex_table[mutex->sem].is_locked = 0;
    foothread_mutex_table[mutex->sem].owner = 0;
    Up(foothread_mutex_table[mutex->sem].sem);
}

void foothread_mutex_destroy(foothread_mutex_t *mutex)
{
    semctl(foothread_mutex_table[mutex->sem].sem, 0, IPC_RMID);
    foothread_mutex_table[mutex->sem].is_locked = 0;
    foothread_mutex_table[mutex->sem].owner = 0;
    foothread_mutex_table[mutex->sem].sem = -1;
    mutex->sem = -1;
}

void foothread_barrier_init(foothread_barrier_t *barrier, int count)
{
    if (barrier_count == 0)
    {
        barrier_table_sem = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
        if (barrier_table_sem == -1)
        {
            return;
        }
        pop.sem_num = 0;
        pop.sem_op = -1;
        pop.sem_flg = 0;
        vop.sem_num = 0;
        vop.sem_op = 1;
        vop.sem_flg = 0;
        semctl(barrier_table_sem, 0, SETVAL, 1);
    }
    Down(barrier_table_sem);
    barrier->sem = barrier_count;
    foothread_barrier_table[barrier_count].sem_mutex = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    if (foothread_barrier_table[barrier_count].sem_mutex == -1)
    {
        Up(barrier_table_sem);
        return;
    }
    semctl(foothread_barrier_table[barrier_count].sem_mutex, 0, SETVAL, 1);
    foothread_barrier_table[barrier_count].sem_1 = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    if (foothread_barrier_table[barrier_count].sem_1 == -1)
    {
        Up(barrier_table_sem);
        return;
    }
    semctl(foothread_barrier_table[barrier_count].sem_1, 0, SETVAL, 0);
    foothread_barrier_table[barrier_count].sem_2 = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    if (foothread_barrier_table[barrier_count].sem_2 == -1)
    {
        Up(barrier_table_sem);
        return;
    }
    semctl(foothread_barrier_table[barrier_count].sem_2, 0, SETVAL, 1);
    foothread_barrier_table[barrier_count].count = count;
    foothread_barrier_table[barrier_count].current_count = 0;
    barrier_count++;
    Up(barrier_table_sem);
}

void foothread_barrier_wait(foothread_barrier_t *barrier)
{
    Down(foothread_barrier_table[barrier->sem].sem_mutex);
    foothread_barrier_table[barrier->sem].current_count++;
    if (foothread_barrier_table[barrier->sem].current_count == foothread_barrier_table[barrier->sem].count)
    {
        Down(foothread_barrier_table[barrier->sem].sem_2);
        Up(foothread_barrier_table[barrier->sem].sem_1);
    }
    Up(foothread_barrier_table[barrier->sem].sem_mutex);

    Down(foothread_barrier_table[barrier->sem].sem_1);
    Up(foothread_barrier_table[barrier->sem].sem_1);

    Down(foothread_barrier_table[barrier->sem].sem_mutex);
    foothread_barrier_table[barrier->sem].current_count--;
    if (foothread_barrier_table[barrier->sem].current_count == 0)
    {
        Down(foothread_barrier_table[barrier->sem].sem_1);
        Up(foothread_barrier_table[barrier->sem].sem_2);
    }
    Up(foothread_barrier_table[barrier->sem].sem_mutex);

    Down(foothread_barrier_table[barrier->sem].sem_2);
    Up(foothread_barrier_table[barrier->sem].sem_2);
}

void foothread_barrier_destroy(foothread_barrier_t *barrier)
{
    semctl(foothread_barrier_table[barrier->sem].sem_mutex, 0, IPC_RMID);
    semctl(foothread_barrier_table[barrier->sem].sem_1, 0, IPC_RMID);
    semctl(foothread_barrier_table[barrier->sem].sem_2, 0, IPC_RMID);
    foothread_barrier_table[barrier->sem].sem_mutex = -1;
    foothread_barrier_table[barrier->sem].sem_1 = -1;
    foothread_barrier_table[barrier->sem].sem_2 = -1;
    barrier->sem = -1;    
}