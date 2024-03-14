#ifndef __FOOTHREAD_H__
#define __FOOTHREAD_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sched.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <sched.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#define FOOTHREAD_THREADS_MAX 100
#define FOOTHREAD_DEFAULT_STACK_SIZE 2097152

#define FOOTHREAD_JOINABLE 1
#define FOOTHREAD_DETACHED 2

#define FOOTHREAD_ATTR_INITIALIZER                       \
    {                                                    \
        FOOTHREAD_DEFAULT_STACK_SIZE, FOOTHREAD_DETACHED \
    }

typedef struct foothread_t
{
    pid_t tid;
    int join_type;
    int stack_size;
} foothread_t;

typedef struct foothread_attr_t
{
    int stack_size;
    int join_type;
} foothread_attr_t;

void foothread_create(foothread_t *thread, foothread_attr_t *attr, int (*start_routine)(void *), void *arg);
void foothread_exit();

#endif