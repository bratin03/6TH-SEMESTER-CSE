/**********************************************************
                    Student Information:
-----------------------------------------------------------
                    Name: Bratin Mondal
                    Student ID: 21CS10016
-----------------------------------------------------------
        Department of Computer Science and Engineering,
        Indian Institute of Technology Kharagpur.
***********************************************************/

#ifndef __FOOTHREAD_H__
#define __FOOTHREAD_H__

#include <sys/types.h>

#define FOOTHREAD_THREADS_MAX 1000
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

typedef struct foothread_mutex_t
{
    int sem;
    int is_locked;
    pid_t owner;
} foothread_mutex_t;

typedef struct foothread_barrier_t
{
    int sem_mutex;
    int sem_1;
    int sem_2;
    int count;
    int current_count;
} foothread_barrier_t;

void foothread_create(foothread_t *thread, foothread_attr_t *attr, int (*start_routine)(void *), void *arg);
void foothread_exit();
void foothread_attr_setjointype(foothread_attr_t *attr, int join_type);
void foothread_attr_setstacksize(foothread_attr_t *attr, int stack_size);
void foothread_mutex_init(foothread_mutex_t *mutex);
void foothread_mutex_lock(foothread_mutex_t *mutex);
void foothread_mutex_unlock(foothread_mutex_t *mutex);
void foothread_mutex_destroy(foothread_mutex_t *mutex);
void foothread_barrier_init(foothread_barrier_t *barrier, int count);
void foothread_barrier_wait(foothread_barrier_t *barrier);
void foothread_barrier_destroy(foothread_barrier_t *barrier);
#endif