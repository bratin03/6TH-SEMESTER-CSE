#include "foothread.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CHILD_NUM 20
foothread_mutex_t print_mutex;
foothread_mutex_t barrier_mutex[CHILD_NUM - 1];
foothread_barrier_t barrier;

int child(void *arg)
{
    long p = (long)arg;
    foothread_mutex_lock(&print_mutex);
    // printf("Child %d start %d\n", p, gettid());
    foothread_mutex_unlock(&print_mutex);
    int mysleep_time = rand() % 10;
    foothread_mutex_lock(&print_mutex);
    printf("Child %ld sleep %d seconds\n", p, mysleep_time);
    foothread_mutex_unlock(&print_mutex);
    sleep(mysleep_time);
    foothread_barrier_wait(&barrier);
    foothread_mutex_lock(&print_mutex);
    printf("Child %ld exit from barrier\n", p);
    foothread_mutex_unlock(&print_mutex);
    mysleep_time = rand() % 10;
    foothread_mutex_lock(&print_mutex);
    printf("Child %ld sleep %d seconds\n", p, mysleep_time);
    foothread_mutex_unlock(&print_mutex);
    sleep(mysleep_time);
    foothread_barrier_wait(&barrier);
    foothread_mutex_lock(&print_mutex);
    printf("Child %ld exit from barrier\n", p);
    foothread_mutex_unlock(&print_mutex);
    foothread_exit();
    return 0;
}

int main()
{
    foothread_t thread;
    foothread_attr_t attr;

    foothread_mutex_init(&print_mutex);
    for (int i = 0; i < CHILD_NUM - 1; i++)
    {
        foothread_mutex_init(&barrier_mutex[i]);
    }
    foothread_barrier_init(&barrier, CHILD_NUM);
    foothread_attr_setjointype(&attr, FOOTHREAD_JOINABLE);
    foothread_attr_setstacksize(&attr, FOOTHREAD_DEFAULT_STACK_SIZE);
    long i;
    for (i = 0; i < CHILD_NUM; i++)
    {
        foothread_create(&thread, &attr, child, (void *)i);
    }
    foothread_exit();
    foothread_mutex_lock(&print_mutex);
    printf("Parent Exit\n");
    foothread_mutex_unlock(&print_mutex);
    return 0;
}