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
// Include the necessary header files
#include <sys/types.h>
// Define the necessary constants
#define FOOTHREAD_THREADS_MAX 10000
#define FOOTHREAD_DEFAULT_STACK_SIZE 2097152
#define FOOTHREAD_JOINABLE 1
#define FOOTHREAD_DETACHED 2
#define FOOTHREAD_ATTR_INITIALIZER                       \
    {                                                    \
        FOOTHREAD_DEFAULT_STACK_SIZE, FOOTHREAD_DETACHED \
    }
/*
Struct Name: foothread_t
Struct Members: 
    1. pid_t tid: The thread id of the thread.
    2. int join_type: The join type of the thread.
    3. int stack_size: The stack size of the thread.
Struct Description: This struct is used to store the thread id, join type and stack size of a thread.
*/
typedef struct foothread_t
{
    pid_t tid;
    int join_type;
    int stack_size;
} foothread_t;
/*
Struct Name: foothread_attr_t
Struct Members: 
    1. int stack_size: The stack size of the thread.
    2. int join_type: The join type of the thread.
Struct Description: This struct is used to store the stack size and join type of a thread.
*/
typedef struct foothread_attr_t
{
    int stack_size;
    int join_type;
} foothread_attr_t;
/*
Struct Name: foothread_mutex_t
Struct Members: 
    1. int sem: The semaphore id of the mutex.
    2. int is_locked: The lock status of the mutex.
    3. pid_t owner: The owner of the mutex.
Struct Description: This struct is used to store the semaphore id, lock status and owner of a mutex.
*/
typedef struct foothread_mutex_t
{
    int sem;
    int is_locked;
    pid_t owner;
} foothread_mutex_t;
/*
Struct Name: foothread_barrier_t
Struct Members: 
    1. int sem_mutex: The semaphore id of the mutex.
    2. int sem_1: The semaphore id of the first barrier.
    3. int sem_2: The semaphore id of the second barrier.
    4. int count: The count of the barrier.
    5. int current_count: The current count of the barrier.
Struct Description: This struct is used to store the semaphore id, count and current count of a barrier.
*/
typedef struct foothread_barrier_t
{
    int sem_mutex;
    int sem_1;
    int sem_2;
    int count;
    int current_count;
} foothread_barrier_t;

// Define the necessary functions
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