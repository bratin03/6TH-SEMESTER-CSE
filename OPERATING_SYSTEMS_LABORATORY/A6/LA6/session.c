#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "event.h"

#define deb(x) printf(#x "=%d\n", x)

#define MAX_PATIENT 25
#define MAX_SALES 3

#define START_HOUR 9
#define VERBOSE
int time_store = 0;
char *am = "am";
char *pm = "pm";

int min(int a, int b)
{
    return a < b ? a : b;
}

int child_entered = 0;

int patient_count = 0;
int patient_service = 0;

int report_count = 0;
int report_service = 0;

int sales_count = 0;
int sales_service = 0;

int doctor_available = 0;
int doctor_done = 0;

pthread_mutex_t doctor_mutex;
pthread_cond_t doctor_cond;

pthread_mutex_t mutex1, mutex2, mutex3;
pthread_cond_t cond1, cond2, cond3, condd, condp, condr, conds;

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

void *reporter_function(void *arg)
{
    parameters_thread *p = (parameters_thread *)arg;
    int id = p->id;
    int endtime = p->endtime;
    pthread_mutex_lock(&mutex1);
    child_entered = 1;
    pthread_mutex_lock(&mutex2);
    pthread_cond_signal(&cond1);
    pthread_mutex_unlock(&mutex1);

    pthread_cond_wait(&condr, &mutex2);
    pthread_mutex_unlock(&mutex2);
    int current_time = time_store;
    int end_time = current_time + endtime;
    time_struct start_time = get_time(current_time);
    time_struct end_time_struct = get_time(end_time);

    char start_ext[3];
    char end_ext[3];
    if (start_time.hour > 12)
    {
        start_time.hour -= 12;
        strcpy(start_ext, pm);
    }
    else
    {
        strcpy(start_ext, am);
    }

    if (end_time_struct.hour > 12)
    {
        end_time_struct.hour -= 12;
        strcpy(end_ext, pm);
    }
    else
    {
        strcpy(end_ext, am);
    }
    printf("[%02d:%02d%s - %02d:%02d%s] Reporter %d is in doctor's chamber.\n", start_time.hour, start_time.minute, start_ext, end_time_struct.hour, end_time_struct.minute, end_ext, id);
    pthread_cond_signal(&cond3);
    pthread_exit(0);
}

void *patient_function(void *arg)
{
    parameters_thread *p = (parameters_thread *)arg;
    int id = p->id;
    int endtime = p->endtime;
    pthread_mutex_lock(&mutex1);
    child_entered = 1;
    pthread_mutex_lock(&mutex2);
    pthread_cond_signal(&cond1);
    pthread_mutex_unlock(&mutex1);

    pthread_cond_wait(&condp, &mutex2);
    pthread_mutex_unlock(&mutex2);
    int current_time = time_store;
    int end_time = current_time + endtime;
    time_struct start_time = get_time(current_time);
    time_struct end_time_struct = get_time(end_time);

    char start_ext[3];
    char end_ext[3];
    if (start_time.hour > 12)
    {
        start_time.hour -= 12;
        strcpy(start_ext, pm);
    }
    else
    {
        strcpy(start_ext, am);
    }

    if (end_time_struct.hour > 12)
    {
        end_time_struct.hour -= 12;
        strcpy(end_ext, pm);
    }
    else
    {
        strcpy(end_ext, am);
    }

    printf("[%02d:%02d%s - %02d:%02d%s] Patient %d is in doctor's chamber.\n", start_time.hour, start_time.minute, start_ext, end_time_struct.hour, end_time_struct.minute, end_ext, id);
    pthread_cond_signal(&cond3);
    pthread_exit(0);
}

void *sales_function(void *arg)
{
    parameters_thread *p = (parameters_thread *)arg;
    int id = p->id;
    int endtime = p->endtime;
    pthread_mutex_lock(&mutex1);
    child_entered = 1;
    pthread_mutex_lock(&mutex2);
    pthread_cond_signal(&cond1);
    pthread_mutex_unlock(&mutex1);

    pthread_cond_wait(&conds, &mutex2);
    pthread_mutex_unlock(&mutex2);
    int current_time = time_store;
    int end_time = current_time + endtime;
    time_struct start_time = get_time(current_time);
    time_struct end_time_struct = get_time(end_time);

    char start_ext[3];
    char end_ext[3];
    if (start_time.hour > 12)
    {
        start_time.hour -= 12;
        strcpy(start_ext, pm);
    }
    else
    {
        strcpy(start_ext, am);
    }

    if (end_time_struct.hour > 12)
    {
        end_time_struct.hour -= 12;
        strcpy(end_ext, pm);
    }
    else
    {
        strcpy(end_ext, am);
    }

    printf("[%02d:%02d%s - %02d:%02d%s] Sales representative %d is in doctor's chamber.\n", start_time.hour, start_time.minute, start_ext, end_time_struct.hour, end_time_struct.minute, end_ext, id);
    pthread_cond_signal(&cond3);
    pthread_exit(0);
}

void *doctor_function(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&mutex1);
        child_entered = 1;
        pthread_mutex_lock(&mutex2);
        pthread_cond_signal(&cond1);
        pthread_mutex_unlock(&mutex1);
        pthread_cond_wait(&condd, &mutex2);
        if (doctor_done == 1)
        {
            pthread_mutex_unlock(&mutex2);
            int current_time = time_store;
            time_struct start_time = get_time(current_time);
            char start_ext[3];
            if (start_time.hour > 12)
            {
                start_time.hour -= 12;
                strcpy(start_ext, pm);
            }
            else
            {
                strcpy(start_ext, am);
            }
            printf("[%02d:%02d%s] Doctor leaves\n", start_time.hour, start_time.minute, start_ext);
            pthread_cond_signal(&cond3);
            pthread_exit(0);
        }
        pthread_mutex_unlock(&mutex2);
        int current_time = time_store;
        time_struct start_time = get_time(current_time);
        char start_ext[3];
        if (start_time.hour > 12)
        {
            start_time.hour -= 12;
            strcpy(start_ext, pm);
        }
        else
        {
            strcpy(start_ext, am);
        }
        printf("[%02d:%02d%s] Doctor has next visitor.\n", start_time.hour, start_time.minute, start_ext);
        pthread_cond_signal(&cond3);
    }
}

int main()
{
    pthread_t patient;
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);
    pthread_mutex_init(&mutex3, NULL);
    pthread_cond_init(&cond1, NULL);
    pthread_cond_init(&cond2, NULL);
    pthread_cond_init(&cond3, NULL);
    pthread_cond_init(&condd, NULL);
    pthread_cond_init(&condp, NULL);
    pthread_cond_init(&condr, NULL);
    pthread_cond_init(&conds, NULL);

    eventQ eq = initEQ("ARRIVAL.txt");
    eventQ pQ, rQ, sQ;
    pQ.n = 0;
    pQ.Q = (event *)malloc(128 * sizeof(event));
    rQ.n = 0;
    rQ.Q = (event *)malloc(128 * sizeof(event));
    sQ.n = 0;
    sQ.Q = (event *)malloc(128 * sizeof(event));

    event doctorevent;
    doctorevent.type = 'D';
    doctorevent.time = 0;
    doctorevent.duration = 0;
    eq = addevent(eq, doctorevent);
    pthread_t doctor;
    pthread_mutex_lock(&mutex1);
    child_entered = 0;
    pthread_create(&doctor, NULL, (void *)doctor_function, NULL);
    pthread_mutex_unlock(&mutex1);
    while (child_entered == 0)
    {
        pthread_mutex_lock(&mutex1);
        pthread_cond_wait(&cond1, &mutex1);
        pthread_mutex_unlock(&mutex1);
    }
    pthread_mutex_lock(&mutex2);
    pthread_mutex_unlock(&mutex2);

    doctor_available = 0;

    while (!emptyQ(eq))
    {
        // printf("Patient count: %d Report count: %d Sales count: %d\n", patient_count, report_count, sales_count);
        event e = nextevent(eq);
        // printf("Event type: %c Time: %d\n", e.type, e.time);
        eq = delevent(eq);
        if (e.type == 'D')
        {
            time_store = e.time;
            int patient_wait = min(patient_count - patient_service, MAX_PATIENT - patient_service);
            int report_wait = report_count - report_service;
            int sales_wait = min(sales_count - sales_service, MAX_SALES - sales_service);

            if (patient_service == MAX_PATIENT && report_service == report_count && sales_service == MAX_SALES)
            {
                doctor_done = 1;
                pthread_cond_signal(&condd);
                pthread_mutex_lock(&mutex3);
                pthread_cond_wait(&cond3, &mutex3);
                pthread_mutex_unlock(&mutex3);
                continue;
            }
            if (report_wait > 0)
            {
                child_entered = 0;
                pthread_cond_signal(&condd);
                pthread_mutex_lock(&mutex3);
                pthread_mutex_lock(&mutex1);
                pthread_cond_wait(&cond3, &mutex3);
                pthread_mutex_unlock(&mutex3);
                pthread_mutex_unlock(&mutex1);
                while (child_entered == 0)
                {
                    pthread_mutex_lock(&mutex1);
                    pthread_cond_wait(&cond1, &mutex1);
                    pthread_mutex_unlock(&mutex1);
                }
                pthread_mutex_lock(&mutex2);
                pthread_mutex_unlock(&mutex2);

                pthread_cond_signal(&condr);
                pthread_mutex_lock(&mutex3);
                pthread_cond_wait(&cond3, &mutex3);
                pthread_mutex_unlock(&mutex3);

                report_service++;
                event next_e = nextevent(rQ);
                rQ = delevent(rQ);
                event doc_event;
                doc_event.type = 'D';
                doc_event.time = time_store + next_e.duration;
                doc_event.duration = 0;
                eq = addevent(eq, doc_event);
            }
            else if (patient_wait > 0)
            {
                child_entered = 0;
                pthread_cond_signal(&condd);
                pthread_mutex_lock(&mutex3);
                pthread_mutex_lock(&mutex1);
                pthread_cond_wait(&cond3, &mutex3);
                pthread_mutex_unlock(&mutex3);
                pthread_mutex_unlock(&mutex1);
                while (child_entered == 0)
                {
                    pthread_mutex_lock(&mutex1);
                    pthread_cond_wait(&cond1, &mutex1);
                    pthread_mutex_unlock(&mutex1);
                }
                pthread_mutex_lock(&mutex2);
                pthread_mutex_unlock(&mutex2);

                pthread_cond_signal(&condp);
                pthread_mutex_lock(&mutex3);
                pthread_cond_wait(&cond3, &mutex3);
                pthread_mutex_unlock(&mutex3);

                patient_service++;
                event next_e = nextevent(pQ);
                pQ = delevent(pQ);
                event doc_event;
                doc_event.type = 'D';
                doc_event.time = time_store + next_e.duration;
                doc_event.duration = 0;
                eq = addevent(eq, doc_event);
            }
            else if (sales_wait > 0)
            {
                child_entered = 0;
                pthread_cond_signal(&condd);
                pthread_mutex_lock(&mutex3);
                pthread_mutex_lock(&mutex1);
                pthread_cond_wait(&cond3, &mutex3);
                pthread_mutex_unlock(&mutex3);
                pthread_mutex_unlock(&mutex1);
                while (child_entered == 0)
                {
                    pthread_mutex_lock(&mutex1);
                    pthread_cond_wait(&cond1, &mutex1);
                    pthread_mutex_unlock(&mutex1);
                }
                pthread_mutex_lock(&mutex2);
                pthread_mutex_unlock(&mutex2);
                printf("Sales service\n");
                pthread_cond_signal(&conds);
                pthread_mutex_lock(&mutex3);
                pthread_cond_wait(&cond3, &mutex3);
                pthread_mutex_unlock(&mutex3);

                sales_service++;
                event next_e = nextevent(sQ);
                sQ = delevent(sQ);
                event doc_event;
                doc_event.type = 'D';
                doc_event.time = time_store + next_e.duration;
                doc_event.duration = 0;
                eq = addevent(eq, doc_event);
            }
            else
            {
                doctor_available = 1;
            }
        }
        else
        {
            time_store = e.time;
            char arrival_ext[3];
            time_struct arrival_time = get_time(e.time);
            if (arrival_time.hour > 12)
            {
                arrival_time.hour -= 12;
                strcpy(arrival_ext, pm);
            }
            else
            {
                strcpy(arrival_ext, am);
            }
            if (e.type == 'P')
            {
                printf("\t\t[%02d:%02d%s] Patient %d arrives.\n", arrival_time.hour, arrival_time.minute, arrival_ext, patient_count + 1);
                if (doctor_done == 1)
                {
                    printf("\t\t[%02d:%02d%s] Patient %d leaves (session over).\n", arrival_time.hour, arrival_time.minute, arrival_ext, patient_count + 1);
                    patient_count++;
                    continue;
                }
                if (patient_count >= MAX_PATIENT)
                {
                    printf("\t\t[%02d:%02d%s] Patient %d leaves (quota full).\n", arrival_time.hour, arrival_time.minute, arrival_ext, patient_count + 1);
                    patient_count++;
                    continue;
                }
                pQ = addevent(pQ, e);
                // printf("Patient added\n");
                pthread_mutex_lock(&mutex1);
                child_entered = 0;
                parameters_thread *p = (parameters_thread *)malloc(sizeof(parameters_thread));
                p->id = patient_count + 1;
                p->endtime = e.duration;

                pthread_create(&patient, NULL, (void *)patient_function, (void *)p);
                pthread_mutex_unlock(&mutex1);
                while (child_entered == 0)
                {
                    pthread_mutex_lock(&mutex1);
                    pthread_cond_wait(&cond1, &mutex1);
                    pthread_mutex_unlock(&mutex1);
                }
                pthread_mutex_lock(&mutex2);
                pthread_mutex_unlock(&mutex2);
                patient_count++;
            }
            else if (e.type == 'R')
            {

                printf("\t\t[%02d:%02d%s] Reporter %d arrives.\n", arrival_time.hour, arrival_time.minute, arrival_ext, report_count + 1);
                if (doctor_done == 1)
                {
                    printf("\t\t[%02d:%02d%s] Reporter %d leaves (session over).\n", arrival_time.hour, arrival_time.minute, arrival_ext, report_count + 1);
                    report_count++;
                    continue;
                }
                rQ = addevent(rQ, e);
                pthread_mutex_lock(&mutex1);
                child_entered = 0;
                parameters_thread *p = (parameters_thread *)malloc(sizeof(parameters_thread));
                p->id = report_count + 1;
                p->endtime = e.duration;

                pthread_create(&patient, NULL, (void *)reporter_function, (void *)p);
                pthread_mutex_unlock(&mutex1);
                while (child_entered == 0)
                {
                    pthread_mutex_lock(&mutex1);
                    pthread_cond_wait(&cond1, &mutex1);
                    pthread_mutex_unlock(&mutex1);
                }
                pthread_mutex_lock(&mutex2);
                pthread_mutex_unlock(&mutex2);
                report_count++;
            }
            else if (e.type == 'S')
            {

                printf("\t\t[%02d:%02d%s] Sales representative %d arrives.\n", arrival_time.hour, arrival_time.minute, arrival_ext, sales_count + 1);
                if (doctor_done == 1)
                {
                    printf("\t\t[%02d:%02d%s] Sales representative %d leaves (session over).\n", arrival_time.hour, arrival_time.minute, arrival_ext, sales_count + 1);
                    sales_count++;
                    continue;
                }
                if (sales_count >= MAX_SALES)
                {
                    printf("\t\t[%02d:%02d%s] Sales representative %d leaves (quota full).\n", arrival_time.hour, arrival_time.minute, arrival_ext, sales_count + 1);
                    sales_count++;
                    continue;
                }
                sQ = addevent(sQ, e);
                pthread_mutex_lock(&mutex1);
                child_entered = 0;
                parameters_thread *p = (parameters_thread *)malloc(sizeof(parameters_thread));
                p->id = sales_count + 1;
                p->endtime = e.duration;

                pthread_create(&patient, NULL, (void *)sales_function, (void *)p);
                pthread_mutex_unlock(&mutex1);
                while (child_entered == 0)
                {
                    pthread_mutex_lock(&mutex1);
                    pthread_cond_wait(&cond1, &mutex1);
                    pthread_mutex_unlock(&mutex1);
                }
                pthread_mutex_lock(&mutex2);
                pthread_mutex_unlock(&mutex2);
                sales_count++;
            }

            if (doctor_available == 1)
            {
                doctor_available = 0;
                child_entered = 0;
                pthread_cond_signal(&condd);
                pthread_mutex_lock(&mutex3);
                pthread_mutex_lock(&mutex1);
                pthread_cond_wait(&cond3, &mutex3);
                pthread_mutex_unlock(&mutex3);
                pthread_mutex_unlock(&mutex1);
                while (child_entered == 0)
                {
                    pthread_mutex_lock(&mutex1);
                    pthread_cond_wait(&cond1, &mutex1);
                    pthread_mutex_unlock(&mutex1);
                }
                pthread_mutex_lock(&mutex2);
                pthread_mutex_unlock(&mutex2);

                if (e.type == 'R')
                {
                    pthread_cond_signal(&condr);
                    pthread_mutex_lock(&mutex3);
                    pthread_cond_wait(&cond3, &mutex3);
                    pthread_mutex_unlock(&mutex3);

                    report_service++;
                    event next_e = nextevent(rQ);
                    rQ = delevent(rQ);
                    event doc_event;
                    doc_event.type = 'D';
                    doc_event.time = time_store + next_e.duration;
                    doc_event.duration = 0;
                    eq = addevent(eq, doc_event);
                }
                else if (e.type == 'P')
                {
                    pthread_cond_signal(&condp);
                    pthread_mutex_lock(&mutex3);
                    pthread_cond_wait(&cond3, &mutex3);
                    pthread_mutex_unlock(&mutex3);

                    patient_service++;
                    event next_e = nextevent(pQ);
                    pQ = delevent(pQ);
                    event doc_event;
                    doc_event.type = 'D';
                    doc_event.time = time_store + next_e.duration;
                    doc_event.duration = 0;
                    eq = addevent(eq, doc_event);
                }
                else if (e.type == 'S')
                {
                    pthread_cond_signal(&conds);
                    pthread_mutex_lock(&mutex3);
                    pthread_cond_wait(&cond3, &mutex3);
                    pthread_mutex_unlock(&mutex3);

                    sales_service++;
                    event next_e = nextevent(sQ);
                    sQ = delevent(sQ);
                    event doc_event;
                    doc_event.type = 'D';
                    doc_event.time = time_store + next_e.duration;
                    doc_event.duration = 0;
                    eq = addevent(eq, doc_event);
                }
            }
        }
    }
    pthread_join(doctor, NULL);
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    pthread_mutex_destroy(&mutex3);
    pthread_cond_destroy(&cond1);
    pthread_cond_destroy(&cond2);
    pthread_cond_destroy(&cond3);
    pthread_cond_destroy(&condd);
    pthread_cond_destroy(&condp);
    pthread_cond_destroy(&condr);
    pthread_cond_destroy(&conds);
    return 0;
}