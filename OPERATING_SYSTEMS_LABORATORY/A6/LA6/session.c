#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "event.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int P_count = 0, R_count = 0, S_count = 0;
int P_count_total = 0, R_count_total = 0, S_count_total = 0;
int P_count_current = 0, R_count_current = 0, S_count_current = 0;
int isdoctor = 0;
pthread_mutex_t doctor_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t doctor_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t P_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t P_mutex_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t P_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t R_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t R_mutex_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t R_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t S_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t S_mutex_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t S_cond = PTHREAD_COND_INITIALIZER;
eventQ E;
typedef struct
{
    char type;
    int time;
    int duration;
    int id;
} event_with_id;

event_with_id initalizer(event e, int x)
{
    event_with_id e1;
    e1.type = e.type;
    e1.time = e.time;
    e1.duration = e.duration;
    e1.id = x;
    return e1;
}

typedef struct
{
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    struct QueueNode *front;
    struct QueueNode *rear;
} CountingSemaphore;

// Node for queue
typedef struct QueueNode
{
    pthread_t thread_id;
    struct QueueNode *next;
} QueueNode;

void counting_semaphore_init(CountingSemaphore *semaphore, int initial_count)
{
    semaphore->count = initial_count;
    pthread_mutex_init(&semaphore->mutex, NULL);
    pthread_cond_init(&semaphore->condition, NULL);
    semaphore->front = NULL;
    semaphore->rear = NULL;
}

void counting_semaphore_wait(CountingSemaphore *semaphore)
{
    pthread_mutex_lock(&semaphore->mutex);
    while (semaphore->count <= 0)
    {
        // Create a new node for the waiting thread
        QueueNode *new_node = (QueueNode *)malloc(sizeof(QueueNode));
        new_node->thread_id = pthread_self();
        new_node->next = NULL;

        // Enqueue the thread
        if (semaphore->front == NULL)
            semaphore->front = new_node;
        else
            semaphore->rear->next = new_node;
        semaphore->rear = new_node;

        pthread_cond_wait(&semaphore->condition, &semaphore->mutex);
    }
    semaphore->count--;
    pthread_mutex_unlock(&semaphore->mutex);
}

void counting_semaphore_post(CountingSemaphore *semaphore)
{
    pthread_mutex_lock(&semaphore->mutex);
    semaphore->count++;

    // Dequeue a waiting thread if the count allows
    if (semaphore->front != NULL)
    {
        QueueNode *temp = semaphore->front;
        semaphore->front = semaphore->front->next;
        if (semaphore->front == NULL)
            semaphore->rear = NULL;
        pthread_cond_signal(&semaphore->condition);
        free(temp);
    }

    pthread_mutex_unlock(&semaphore->mutex);
}

void counting_semaphore_destroy(CountingSemaphore *semaphore)
{
    pthread_mutex_destroy(&semaphore->mutex);
    pthread_cond_destroy(&semaphore->condition);
}

CountingSemaphore P_sem, R_sem, S_sem;
CountingSemaphore Procees_Exectuion_Semaphore;

int t = 0;

void *P_thread(void *arg)
{
    event_with_id e = *(event_with_id *)arg;
    // pthread_mutex_lock(&P_mutex);
    // pthread_mutex_lock(&P_mutex_2);
    // pthread_cond_wait(&P_cond, &P_mutex_2);
    // printf("ID: %d\n", e.id);
    counting_semaphore_wait(&P_sem);
    int initial_time = 540;
    int current_time = t;
    int actual_time = current_time + initial_time;
    int hour = actual_time / 60;
    int minute = actual_time % 60;
    int end_time = actual_time + e.duration;
    int end_hour = end_time / 60;
    int end_minute = end_time % 60;
    printf("\tPatient P%d enters at %d:%d, leaves at %d:%d\n", e.id, hour, minute, end_hour, end_minute);
    event new_event;
    new_event.type = 'D';
    new_event.time = end_time;
    new_event.duration = 0;
    E = addevent(E, new_event);
    counting_semaphore_post(&Procees_Exectuion_Semaphore);
    pthread_exit(0);
}

void *R_thread(void *arg)
{
    event_with_id e = *(event_with_id *)arg;
    pthread_mutex_lock(&R_mutex);
    pthread_cond_wait(&R_cond, &R_mutex);
    int initial_time = 540;
    int current_time = t;
    int actual_time = current_time + initial_time;
    int hour = actual_time / 60;
    int minute = actual_time % 60;
    int end_time = actual_time + e.duration;
    int end_hour = end_time / 60;
    int end_minute = end_time % 60;
    printf("\tPatient R%d enters at %d:%d, leaves at %d:%d\n", e.id, hour, minute, end_hour, end_minute);
    event new_event;
    pthread_exit(0);
}

void *S_thread(void *arg)
{
    event_with_id e = *(event_with_id *)arg;
    pthread_mutex_lock(&S_mutex);
    pthread_cond_wait(&S_cond, &S_mutex);
    int initial_time = 540;
    int current_time = t;
    int actual_time = current_time + initial_time;
    int hour = actual_time / 60;
    int minute = actual_time % 60;
    int end_time = actual_time + e.duration;
    int end_hour = end_time / 60;
    int end_minute = end_time % 60;
    printf("\tPatient S%d enters at %d:%d, leaves at %d:%d\n", e.id, hour, minute, end_hour, end_minute);
    pthread_exit(0);
}

void *D_thread(void *arg)
{
    int i = 0;
    while (1 && i < 60)
    {
        pthread_mutex_lock(&doctor_mutex);
        pthread_cond_wait(&doctor_cond, &doctor_mutex);
        i++;
    }
}

int main(int argc, char *argv[])
{
    char *fname = "ARRIVAL.txt";
    pthread_attr_t attr;
    E = initEQ(fname);
    event D_start;
    D_start.type = 'D';
    D_start.time = 0;
    D_start.duration = 0;
    E = addevent(E, D_start);
    pthread_t P, R, S, D;
    pthread_create(&D, NULL, D_thread, NULL);
    int i = 0;

    counting_semaphore_init(&P_sem, 0);
    counting_semaphore_init(&R_sem, 0);
    counting_semaphore_init(&S_sem, 0);
    counting_semaphore_init(&Procees_Exectuion_Semaphore, 0);
    while (!emptyQ(E) && i < 60)
    {
        i++;
        event e = nextevent(E);
        E = delevent(E);
        t = e.time;
        if (e.type == 'D')
        {
            if (P_count != 0 || R_count != 0 || S_count != 0)
            {
                int initial_time = 540;
                int current_time = t;
                int actual_time = current_time + initial_time;
                int hour = actual_time / 60;
                int minute = actual_time % 60;
                printf("Doctor has next visitor at %d:%d\n", hour, minute);
                pthread_cond_signal(&doctor_cond);
                if (R_count != 0)
                {
                    R_count--;
                    pthread_cond_signal(&R_cond);
                    event new_event;
                    E = addevent(E, new_event);
                }
                else if (P_count != 0)
                {
                    counting_semaphore_post(&P_sem);
                    P_count--;
                    counting_semaphore_wait(&Procees_Exectuion_Semaphore);

                    // pthread_cond_signal(&P_cond);
                }
                else if (S_count != 0)
                {
                    S_count--;
                    pthread_cond_signal(&S_cond);
                    event new_event;
                    new_event.type = 'D';
                    new_event.time = e.time + e.duration;
                    new_event.duration = 0;
                    E = addevent(E, new_event);
                }
            }
            else
            {
                isdoctor = 1;
            }
        }
        else
        {
            if (e.type = 'P')
            {
                P_count_total++;
                P_count++;
                int initial_time = 540;
                int current_time = t;
                int actual_time = current_time + initial_time;
                int hour = actual_time / 60;
                int minute = actual_time % 60;
                printf("\tPatient P%d arrives at %d:%d\n", P_count_total, hour, minute);
                pthread_attr_init(&attr);
                printf("ID: %d\n", P_count_total);
                event_with_id e1 = initalizer(e, P_count_total);
                pthread_create(&P, &attr, P_thread, &e1);
            }
            else if (e.type = 'R')
            {
                R_count_total++;
                R_count++;
                int initial_time = 540;
                int current_time = t;
                int actual_time = current_time + initial_time;
                int hour = actual_time / 60;
                int minute = actual_time % 60;
                printf("\tPatient R%d arrives at %d:%d\n", R_count_total, hour, minute);
                pthread_attr_init(&attr);
                event_with_id e1 = initalizer(e, R_count_total);
                pthread_create(&R, &attr, R_thread, &e1);
            }
            else
            {
                S_count_total++;
                S_count++;
                int initial_time = 540;
                int current_time = t;
                int actual_time = current_time + initial_time;
                int hour = actual_time / 60;
                int minute = actual_time % 60;
                printf("\tPatient S%d arrives at %d:%d\n", S_count_total, hour, minute);
                pthread_attr_init(&attr);
                event_with_id e1 = initalizer(e, S_count_total);
                pthread_create(&S, &attr, S_thread, &e1);
            }
            if (isdoctor == 1)
            {
                int initial_time = 540;
                int current_time = t;
                int actual_time = current_time + initial_time;
                int hour = actual_time / 60;
                int minute = actual_time % 60;
                printf("Doctor has next visitor at %d:%d\n", hour, minute);
                pthread_cond_signal(&doctor_cond);
                if (e.type = 'P')
                {
                    // pthread_cond_signal(&P_cond);
                    counting_semaphore_post(&P_sem);
                    P_count--;
                    counting_semaphore_wait(&Procees_Exectuion_Semaphore);
                    event new_event;
                    new_event.type = 'D';
                    new_event.time = e.time + e.duration;
                    new_event.duration = 0;
                    // E = addevent(E, new_event);
                }
                else if (e.type = 'R')
                {
                    pthread_cond_signal(&R_cond);
                    event new_event;
                    new_event.type = 'D';
                    new_event.time = e.time + e.duration;
                    new_event.duration = 0;
                    E = addevent(E, new_event);
                }
                else
                {
                    pthread_cond_signal(&S_cond);
                    event new_event;
                    new_event.type = 'D';
                    new_event.time = e.time + e.duration;
                    new_event.duration = 0;
                    E = addevent(E, new_event);
                }
                isdoctor = 0;
            }
        }
    }
}