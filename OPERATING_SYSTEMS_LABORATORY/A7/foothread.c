#define _GNU_SOURCE
#include "foothread.h"

typedef struct foothread_table_entry
{
    pid_t pid;
    pid_t ppid;
    pid_t tid;
    int join_type;
    sem_t semaphore_id;
} foothread_table_entry;

foothread_table_entry foothread_table[FOOTHREAD_THREADS_MAX];
int count = 0;
sem_t sem;

void foothread_create(foothread_t *thread, foothread_attr_t *attr, int (*start_routine)(void *), void *arg)
{
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
        sem_init(&sem, 0, 1);
    }
    sem_wait(&sem);
    if (count >= FOOTHREAD_THREADS_MAX)
    {
        sem_post(&sem);
        return;
    }
    int stack = malloc(stack_size);
    foothread_table[count].pid = clone(start_routine, stack + stack_size, CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD, arg);
    foothread_table[count].ppid = getppid();
    foothread_table[count].tid = foothread_table[count].pid;
    foothread_table[count].join_type = join_type;
    sem_t semaphore;
    sem_init(&semaphore, 0, 0);
    foothread_table[count].semaphore_id = semaphore;
    thread->join_type = join_type;
    thread->stack_size = stack_size;
}

void foothread_exit()
{
    pid_t tid = gettid();
    pid_t Ipid = getpid();
}