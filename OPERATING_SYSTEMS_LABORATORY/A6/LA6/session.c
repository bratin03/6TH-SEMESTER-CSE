#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "event.h"

#define START_HOUR 9
#define VERBOSE
int time_store = 0;

pthread_mutex_t p_wait_mutex;
pthread_cond_t p_wait_cond;
pthread_mutex_t mutex;

pthread_mutex_t assistant_mutex;
pthread_cond_t assistant_cond;

int patient_count = 0;
int patient_service = 0;

int doctor_available = 0;
int doctor_done = 0;

pthread_mutex_t doctor_mutex;
pthread_cond_t doctor_cond;

typedef struct
{
    int id;
    int endtime;
} parameters_thread;

typedef struct
{
    int hour;
    int minute;
} time_struct;

time_struct get_time(int t)
{
    t += (60 * START_HOUR);
    time_struct time;
    time.hour = t / 60;
    time.minute = t % 60;
    return time;
}

void *patient_function(void *arg)
{
    printf("Patient");
    parameters_thread *p = (parameters_thread *)arg;
    int id = p->id;
    int endtime = p->endtime;
    while (1)
    {
        pthread_mutex_lock(&p_wait_mutex);
#ifdef VERBOSE
        printf("Patient %d is waiting.\n", id);
#endif
        pthread_cond_wait(&p_wait_cond, &p_wait_mutex);
        pthread_mutex_lock(&mutex);
        if (patient_service == id)
        {
#ifdef VERBOSE
            printf("Patient %d is being released.\n", id);
#endif
            pthread_mutex_unlock(&mutex);
            pthread_mutex_unlock(&p_wait_mutex);
            break;
        }
        else
        {
            printf("Patient Service: %d\n", patient_service);
        }
        pthread_mutex_unlock(&mutex);
        pthread_mutex_unlock(&p_wait_mutex);
    }
    time_struct start_time = get_time(time_store);
    time_struct end_time = get_time(time_store + endtime);
    printf("Patient %d is being served from %02d:%02d to %02d:%02d.\n", id, start_time.hour, start_time.minute, end_time.hour, end_time.minute);
    pthread_cond_signal(&assistant_cond);
    pthread_exit(0);
}

void *doctor_function(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&doctor_mutex);
        pthread_cond_wait(&doctor_cond, &doctor_mutex);
        if (doctor_done)
        {
            pthread_mutex_unlock(&doctor_mutex);
            break;
        }
        pthread_mutex_unlock(&doctor_mutex);
        time_struct start_time = get_time(time_store);
        printf("Doctor has next visitor at %02d:%02d.\n", start_time.hour, start_time.minute);
        pthread_cond_signal(&assistant_cond);
    }
    pthread_exit(0);
}

int main()
{

    pthread_mutex_init(&p_wait_mutex, NULL);
    pthread_cond_init(&p_wait_cond, NULL);
    pthread_mutex_init(&assistant_mutex, NULL);
    pthread_cond_init(&assistant_cond, NULL);
    pthread_mutex_init(&doctor_mutex, NULL);
    pthread_cond_init(&doctor_cond, NULL);

    char filename[] = "ARRIVAL.txt";
    eventQ EQ = initEQ(filename);
    eventQ PQ;
    PQ.n = 0;
    PQ.Q = (event *)malloc(128 * sizeof(event));
    event e;
    e.duration = 0;
    e.time = 0;
    e.type = 'D';
    EQ = addevent(EQ, e);
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_t doctor;
    pthread_create(&doctor, &attr, doctor_function, NULL);
    while (!emptyQ(EQ))
    {
        e = nextevent(EQ);
        EQ = delevent(EQ);
        time_store = e.time;
        printf("Time: %d\n", time_store);
        printf("Event: %c\n", e.type);
        if (e.type == 'D')
        {
            int patient_waiting = patient_count - patient_service;
            if (patient_waiting > 0)
            {
                pthread_cond_signal(&doctor_cond);
                pthread_mutex_lock(&assistant_mutex);
                pthread_cond_wait(&assistant_cond, &assistant_mutex);
                pthread_mutex_unlock(&assistant_mutex);
                patient_service++;
                pthread_cond_broadcast(&p_wait_cond);
                pthread_mutex_lock(&assistant_mutex);
                pthread_cond_wait(&assistant_cond, &assistant_mutex);
                pthread_mutex_unlock(&assistant_mutex);
                event e = nextevent(PQ);
                PQ = delevent(PQ);
                int endtime = e.duration + time_store;
                event event_doctor;
                event_doctor.time = endtime;
                event_doctor.duration = 0;
                event_doctor.type = 'D';
                EQ = addevent(EQ, event_doctor);
            }
            if (patient_count == 25 && patient_service == 25)
            {
                doctor_done = 1;
                pthread_cond_signal(&doctor_cond);
                break;
            }
            else
            {
                doctor_available = 1;
            }
        }
        else
        {
            // printf("Patient %d arrived.\n", patient_count);
            PQ = addevent(PQ, e);
            if (patient_count == 25)
            {
                printf("No more patients are allowed.\n");
                continue;
            }
            patient_count++;
            parameters_thread *p = (parameters_thread *)malloc(sizeof(parameters_thread));
            p->id = patient_count;
            p->endtime = e.duration;
            pthread_t patient;
            printf("Patient %d arrived.\n", patient_count);
            pthread_create(&patient, &attr, patient_function, p);
            printf("Patient %d is waiting.\n", patient_count);
            if (doctor_available)
            {
                doctor_available = 0;
                pthread_cond_signal(&doctor_cond);
                pthread_mutex_lock(&assistant_mutex);
                pthread_cond_wait(&assistant_cond, &assistant_mutex);
                pthread_mutex_unlock(&assistant_mutex);
                pthread_mutex_lock(&mutex);

                patient_service++;
                pthread_mutex_unlock(&mutex);
                pthread_cond_broadcast(&p_wait_cond);
                pthread_mutex_lock(&assistant_mutex);
                pthread_cond_wait(&assistant_cond, &assistant_mutex);
                pthread_mutex_unlock(&assistant_mutex);
                event e = nextevent(PQ);
                PQ = delevent(PQ);
                int endtime = e.duration + time_store;
                event event_doctor;
                event_doctor.time = endtime;
                event_doctor.duration = 0;
                event_doctor.type = 'D';
                EQ = addevent(EQ, event_doctor);
            }
        }
    }
}