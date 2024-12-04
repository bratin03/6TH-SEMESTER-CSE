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
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "event.h"

// Defining the maximum number of patients, sales representatives, the starting hour of the session and the file name
#define MAX_PATIENT 25
#define MAX_SALES 3
#define START_HOUR 9
#define FILE_NAME "arrival.txt"

// This variable stores the current time in minutes
int time_store = 0;

// These are the strings for am and pm. Used for printing the time in the required format
char *am = "am";
char *pm = "pm";

// Mutex and condition variables for the doctor
pthread_mutex_t doctor_mutex;
pthread_cond_t doctor_cond;

// Mutex and condition variables for the entry and the assistant
pthread_mutex_t entry_mutex, entry_confirm_mutex, assistant_mutex;
pthread_cond_t entry_child, assistant_cond;
// Condition variables for the patient, reporter and sales representative
pthread_cond_t cond_doctor, cond_patient, cond_reporter, cond_sales;

// Variables to indicate if the visitor or doctor has started waiting
int child_entered = 0;
// Variable to indicate if the visitor has finished the session
int child_done = 0;
// Variable to indicate if the doctor has finished the session
int doctor_done = 0;

/*
Function Name: min
Input: Two integers a and b
Output: The minimum of the two integers
Logic: Returns the minimum of the two integers
*/
int min(int a, int b)
{
    return a < b ? a : b;
}

/*
Structure Name: parameters_thread
Structure Members:
    id: The id of the visitor
    endtime: The time for which the visitor will be in the doctor's chamber
Structure Description: This structure is used to pass the parameters to the thread functions
*/
typedef struct
{
    int id;
    int endtime;
} parameters_thread;

/*
Structure Name: time_struct
Structure Members:
    hour: The hour of the time
    minute: The minute of the time
Structure Description: This structure is used to store the time in hours and minutes
*/
typedef struct
{
    int hour;
    int minute;
} time_struct;

/*
Function Name: get_time
Input: The time in minutes
Output: The time in hours and minutes
Logic: Returns the time in hours and minutes
*/
time_struct get_time(int t)
{
    t += (60 * START_HOUR);
    time_struct time;
    time.hour = t / 60;
    time.minute = t % 60;
    return time;
}

/*
Function Name: get_time_print
Input: The time in hours and minutes and the extension for am or pm
Output: The extension for am or pm and the time in hours and minutes suitable for printing
Logic: Returns the extension for am or pm based on the time and the time in hours and minutes suitable for printing
*/
void get_time_print(time_struct *time, char *ext)
{
    if ((*time).hour > 12)
    {
        (*time).hour -= 12;
        strcpy(ext, pm);
    }
    else
    {
        if ((*time).hour == 12)
        {
            strcpy(ext, pm);
        }
        else
        {
            strcpy(ext, am);
        }
    }
}

// Function declarations for the thread functions
void *reporter_function(void *arg);
void *patient_function(void *arg);
void *sales_function(void *arg);
void *doctor_function(void *arg);

void color_red()
{
    printf("\033[1;31m");
}

void color_blue()
{
    printf("\033[1;34m");
}

void colore_reset()
{
    printf("\033[0m");
}

/*
Function Name: main
Input: None
Output: 0, to indicate the successful execution of the program
Logic: The main function of the program and the assistant thread . It reads the input from the file and simulates the session.
*/
int main()
{
    // Patient, reporter and sales representative count and service count to indicate the number of visitors and the number of visitors who have been serviced
    int patient_count = 0;
    int patient_service = 0;
    int report_count = 0;
    int report_service = 0;
    int sales_count = 0;
    int sales_service = 0;

    // Variable to indicate if the doctor is available
    int doctor_available = 0;

    // Initializing the mutex and condition variables
    pthread_mutex_init(&entry_mutex, NULL);
    pthread_mutex_init(&entry_confirm_mutex, NULL);
    pthread_mutex_init(&assistant_mutex, NULL);
    pthread_cond_init(&entry_child, NULL);
    pthread_cond_init(&assistant_cond, NULL);
    pthread_cond_init(&cond_doctor, NULL);
    pthread_cond_init(&cond_patient, NULL);
    pthread_cond_init(&cond_reporter, NULL);
    pthread_cond_init(&cond_sales, NULL);
    pthread_t child_thread; // Thread for the visitor

    // Initializing the event queue and the event queue for the patient, reporter and sales representative
    eventQ eq = initEQ(FILE_NAME);
    eventQ pQ, rQ, sQ;
    pQ.n = 0;
    pQ.Q = (event *)malloc(128 * sizeof(event));
    rQ.n = 0;
    rQ.Q = (event *)malloc(128 * sizeof(event));
    sQ.n = 0;
    sQ.Q = (event *)malloc(128 * sizeof(event));

    // Adding the doctor event to the event queue
    event doctorevent;
    doctorevent.type = 'D';
    doctorevent.time = 0;
    doctorevent.duration = 0;
    eq = addevent(eq, doctorevent);

    // Create the doctor thread and enter the doctor has started waiting
    pthread_t doctor;
    pthread_mutex_lock(&entry_mutex);
    child_entered = 0;
    pthread_create(&doctor, NULL, (void *)doctor_function, NULL);
    pthread_mutex_unlock(&entry_mutex);
    while (child_entered == 0)
    {
        pthread_mutex_lock(&entry_mutex);
        pthread_cond_wait(&entry_child, &entry_mutex);
        pthread_mutex_unlock(&entry_mutex);
    }
    pthread_mutex_lock(&entry_confirm_mutex);
    pthread_mutex_unlock(&entry_confirm_mutex);

    // Initialize the doctor_available variable
    doctor_available = 0;

    // Simulate the session
    while (!emptyQ(eq))
    {
        // Get the next event from the event queue
        event e = nextevent(eq);
        eq = delevent(eq);
        if (e.type == 'D')
        {
            // If the event is a doctor event, then check if any visitor is waiting
            time_store = e.time;
            int patient_wait = min(patient_count - patient_service, MAX_PATIENT - patient_service);
            int report_wait = report_count - report_service;
            int sales_wait = min(sales_count - sales_service, MAX_SALES - sales_service);

            // Check if the doctor's quota is full and no reporter is waiting
            if (patient_service == MAX_PATIENT && report_service == report_count && sales_service == MAX_SALES)
            {
                // Signal the doctor that the session is over
                doctor_done = 1;
                child_done = 0;
                pthread_cond_signal(&cond_doctor);
                pthread_mutex_lock(&assistant_mutex);
                while (child_done == 0)
                {
                    pthread_cond_wait(&assistant_cond, &assistant_mutex);
                }
                pthread_mutex_unlock(&assistant_mutex);
                continue;
            }
            // If visitor is waiting, then signal the doctor and the visitor based on the assigned visitor priority and add new doctor event to the event queue indicating the end of the visitor's session
            if (report_wait > 0)
            {
                child_entered = 0;
                child_done = 0;
                pthread_cond_signal(&cond_doctor);
                pthread_mutex_lock(&assistant_mutex);
                while (child_done == 0)
                {
                    pthread_cond_wait(&assistant_cond, &assistant_mutex);
                }
                pthread_mutex_unlock(&assistant_mutex);
                pthread_mutex_lock(&entry_mutex);
                while (child_entered == 0)
                {
                    pthread_cond_wait(&entry_child, &entry_mutex);
                }
                pthread_mutex_unlock(&entry_mutex);
                pthread_mutex_lock(&entry_confirm_mutex);
                pthread_mutex_unlock(&entry_confirm_mutex);

                child_done = 0;
                pthread_cond_signal(&cond_reporter);
                pthread_mutex_lock(&assistant_mutex);
                while (child_done == 0)
                {
                    pthread_cond_wait(&assistant_cond, &assistant_mutex);
                }
                pthread_mutex_unlock(&assistant_mutex);

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
                child_done = 0;
                pthread_cond_signal(&cond_doctor);
                pthread_mutex_lock(&assistant_mutex);
                while (child_done == 0)
                {
                    pthread_cond_wait(&assistant_cond, &assistant_mutex);
                }
                pthread_mutex_unlock(&assistant_mutex);
                pthread_mutex_lock(&entry_mutex);
                while (child_entered == 0)
                {
                    pthread_cond_wait(&entry_child, &entry_mutex);
                }
                pthread_mutex_unlock(&entry_mutex);
                pthread_mutex_lock(&entry_confirm_mutex);
                pthread_mutex_unlock(&entry_confirm_mutex);

                child_done = 0;
                pthread_cond_signal(&cond_patient);
                pthread_mutex_lock(&assistant_mutex);
                while (child_done == 0)
                {
                    pthread_cond_wait(&assistant_cond, &assistant_mutex);
                }
                pthread_mutex_unlock(&assistant_mutex);

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
                child_done = 0;
                pthread_cond_signal(&cond_doctor);
                pthread_mutex_lock(&assistant_mutex);
                while (child_done == 0)
                {
                    pthread_cond_wait(&assistant_cond, &assistant_mutex);
                }
                pthread_mutex_unlock(&assistant_mutex);
                pthread_mutex_lock(&entry_mutex);
                while (child_entered == 0)
                {
                    pthread_cond_wait(&entry_child, &entry_mutex);
                }
                pthread_mutex_unlock(&entry_mutex);
                pthread_mutex_lock(&entry_confirm_mutex);
                pthread_mutex_unlock(&entry_confirm_mutex);

                child_done = 0;
                pthread_cond_signal(&cond_sales);
                pthread_mutex_lock(&assistant_mutex);
                while (child_done == 0)
                {
                    pthread_cond_wait(&assistant_cond, &assistant_mutex);
                }
                pthread_mutex_unlock(&assistant_mutex);

                sales_service++;
                event next_e = nextevent(sQ);
                sQ = delevent(sQ);
                event doc_event;
                doc_event.type = 'D';
                doc_event.time = time_store + next_e.duration;
                doc_event.duration = 0;
                eq = addevent(eq, doc_event);
            }
            // If no visitor is waiting, then mark the doctor as available
            else
            {
                doctor_available = 1;
            }
        }
        else
        {
            // If the event is a visitor event and the quota is not full and the session is not over, then add the visitor event to the respective visitor queue and create a new thread for the visitor
            time_store = e.time;
            char arrival_ext[3];
            time_struct arrival_time = get_time(e.time);
            get_time_print(&arrival_time, arrival_ext);
            if (e.type == 'P')
            {
                // Print the arrival of the patient
                printf("\t  [%02d:%02d%s] Patient %d arrives\n", arrival_time.hour, arrival_time.minute, arrival_ext, patient_count + 1);
                if (doctor_done == 1)
                {
                    // If the session is over, then print the patient leaving
                    printf("\t  [%02d:%02d%s] Patient %d leaves (session over)\n", arrival_time.hour, arrival_time.minute, arrival_ext, patient_count + 1);
                    patient_count++;
                    continue;
                }
                if (patient_count >= MAX_PATIENT)
                {
                    // If the quota is full, then print the patient leaving
                    printf("\t  [%02d:%02d%s] Patient %d leaves (quota full)\n", arrival_time.hour, arrival_time.minute, arrival_ext, patient_count + 1);
                    patient_count++;
                    continue;
                }
                pQ = addevent(pQ, e);
                pthread_mutex_lock(&entry_mutex);
                child_entered = 0;
                parameters_thread *p = (parameters_thread *)malloc(sizeof(parameters_thread));
                p->id = patient_count + 1;
                p->endtime = e.duration;
                // Create a new thread for the patient and ensure that the patient has started waiting
                pthread_create(&child_thread, NULL, (void *)patient_function, (void *)p);
                pthread_mutex_unlock(&entry_mutex);
                pthread_mutex_lock(&entry_mutex);
                while (child_entered == 0)
                {
                    pthread_cond_wait(&entry_child, &entry_mutex);
                }
                pthread_mutex_unlock(&entry_mutex);
                pthread_mutex_lock(&entry_confirm_mutex);
                pthread_mutex_unlock(&entry_confirm_mutex);
                patient_count++;
            }
            else if (e.type == 'R')
            {
                // If the event is a reporter event and the session is not over, then add the reporter event to the reporter queue and create a new thread for the reporter
                printf("\t  [%02d:%02d%s] Reporter %d arrives\n", arrival_time.hour, arrival_time.minute, arrival_ext, report_count + 1);
                if (doctor_done == 1)
                {
                    // If the session is over, then print the reporter leaving
                    printf("\t  [%02d:%02d%s] Reporter %d leaves (session over)\n", arrival_time.hour, arrival_time.minute, arrival_ext, report_count + 1);
                    report_count++;
                    continue;
                }
                rQ = addevent(rQ, e);
                pthread_mutex_lock(&entry_mutex);
                child_entered = 0;
                parameters_thread *p = (parameters_thread *)malloc(sizeof(parameters_thread));
                p->id = report_count + 1;
                p->endtime = e.duration;
                // Create a new thread for the reporter and ensure that the reporter has started waiting
                pthread_create(&child_thread, NULL, (void *)reporter_function, (void *)p);

                pthread_mutex_unlock(&entry_mutex);
                pthread_mutex_lock(&entry_mutex);
                while (child_entered == 0)
                {
                    pthread_cond_wait(&entry_child, &entry_mutex);
                }
                pthread_mutex_unlock(&entry_mutex);
                pthread_mutex_lock(&entry_confirm_mutex);
                pthread_mutex_unlock(&entry_confirm_mutex);
                report_count++;
            }
            else if (e.type == 'S')
            {
                // If the event is a sales representative event and the quota is not full and the session is not over, then add the sales representative event to the sales representative queue and create a new thread for the sales representative
                printf("\t  [%02d:%02d%s] Sales representative %d arrives\n", arrival_time.hour, arrival_time.minute, arrival_ext, sales_count + 1);
                if (doctor_done == 1)
                { // If the session is over, then print the sales representative leaving
                    printf("\t  [%02d:%02d%s] Sales representative %d leaves (session over)\n", arrival_time.hour, arrival_time.minute, arrival_ext, sales_count + 1);
                    sales_count++;
                    continue;
                }
                if (sales_count >= MAX_SALES)
                { // If the quota is full, then print the sales representative leaving
                    printf("\t  [%02d:%02d%s] Sales representative %d leaves (quota full)\n", arrival_time.hour, arrival_time.minute, arrival_ext, sales_count + 1);
                    sales_count++;
                    continue;
                }
                sQ = addevent(sQ, e);
                pthread_mutex_lock(&entry_mutex);
                child_entered = 0;
                parameters_thread *p = (parameters_thread *)malloc(sizeof(parameters_thread));
                p->id = sales_count + 1;
                p->endtime = e.duration;
                // Create a new thread for the sales representative and ensure that the sales representative has started waiting
                pthread_create(&child_thread, NULL, (void *)sales_function, (void *)p);

                pthread_mutex_unlock(&entry_mutex);
                pthread_mutex_lock(&entry_mutex);
                while (child_entered == 0)
                {
                    pthread_cond_wait(&entry_child, &entry_mutex);
                }
                pthread_mutex_unlock(&entry_mutex);
                pthread_mutex_lock(&entry_confirm_mutex);
                pthread_mutex_unlock(&entry_confirm_mutex);
                sales_count++;
            }
            if (doctor_available == 1)
            {
                // If the doctor is available, then signal the doctor and the visitor based on the assigned visitor priority and add new doctor event to the event queue indicating the end of the visitor's session
                doctor_available = 0;
                child_entered = 0;
                child_done = 0;
                pthread_cond_signal(&cond_doctor);
                pthread_mutex_lock(&assistant_mutex);
                while (child_done == 0)
                {
                    pthread_cond_wait(&assistant_cond, &assistant_mutex);
                }
                pthread_mutex_unlock(&assistant_mutex);
                pthread_mutex_lock(&entry_mutex);
                while (child_entered == 0)
                {
                    pthread_cond_wait(&entry_child, &entry_mutex);
                }
                pthread_mutex_unlock(&entry_mutex);
                pthread_mutex_lock(&entry_confirm_mutex);
                pthread_mutex_unlock(&entry_confirm_mutex);

                if (e.type == 'R')
                {
                    child_done = 0;
                    pthread_cond_signal(&cond_reporter);
                    pthread_mutex_lock(&assistant_mutex);
                    while (child_done == 0)
                    {
                        pthread_cond_wait(&assistant_cond, &assistant_mutex);
                    }
                    pthread_mutex_unlock(&assistant_mutex);

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
                    child_done = 0;
                    pthread_cond_signal(&cond_patient);
                    pthread_mutex_lock(&assistant_mutex);
                    while (child_done == 0)
                    {
                        pthread_cond_wait(&assistant_cond, &assistant_mutex);
                    }
                    pthread_mutex_unlock(&assistant_mutex);

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
                    child_done = 0;
                    pthread_cond_signal(&cond_sales);
                    pthread_mutex_lock(&assistant_mutex);
                    while (child_done == 0)
                    {
                        pthread_cond_wait(&assistant_cond, &assistant_mutex);
                    }
                    pthread_mutex_unlock(&assistant_mutex);

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
    // Wait for the doctor to finish the session
    pthread_join(doctor, NULL);
    // Destroy the mutex and condition variables
    pthread_mutex_destroy(&entry_mutex);
    pthread_mutex_destroy(&entry_confirm_mutex);
    pthread_mutex_destroy(&assistant_mutex);
    pthread_cond_destroy(&entry_child);
    pthread_cond_destroy(&assistant_cond);
    pthread_cond_destroy(&cond_doctor);
    pthread_cond_destroy(&cond_patient);
    pthread_cond_destroy(&cond_reporter);
    pthread_cond_destroy(&cond_sales);
    pthread_exit(0);
}

// Thread functions for the visitor, reporter, sales representative and the doctor  are defined below

void *reporter_function(void *arg)
{
    parameters_thread *p = (parameters_thread *)arg;
    int id = p->id;
    int endtime = p->endtime;
    pthread_mutex_lock(&entry_mutex);
    child_entered = 1;
    pthread_mutex_lock(&entry_confirm_mutex);
    pthread_cond_signal(&entry_child);
    pthread_mutex_unlock(&entry_mutex);
    // Wait for the assistant to signal
    pthread_cond_wait(&cond_reporter, &entry_confirm_mutex);
    pthread_mutex_unlock(&entry_confirm_mutex);
    int current_time = time_store;
    int end_time = current_time + endtime;
    time_struct start_time = get_time(current_time);
    time_struct end_time_struct = get_time(end_time);

    char start_ext[3];
    char end_ext[3];
    get_time_print(&start_time, start_ext);
    get_time_print(&end_time_struct, end_ext);
    // Print the reporter entering the doctor's chamber
    color_blue();
    printf("[%02d:%02d%s - %02d:%02d%s] Reporter %d is in doctor's chamber\n", start_time.hour, start_time.minute, start_ext, end_time_struct.hour, end_time_struct.minute, end_ext, id);
    colore_reset();
    // Signal the assistant
    pthread_mutex_lock(&assistant_mutex);
    child_done = 1;
    pthread_cond_signal(&assistant_cond);
    pthread_mutex_unlock(&assistant_mutex);
    pthread_exit(0);
}

void *patient_function(void *arg)
{
    parameters_thread *p = (parameters_thread *)arg;
    int id = p->id;
    int endtime = p->endtime;
    pthread_mutex_lock(&entry_mutex);
    child_entered = 1;
    pthread_mutex_lock(&entry_confirm_mutex);
    pthread_cond_signal(&entry_child);
    pthread_mutex_unlock(&entry_mutex);
    // Wait for the assistant to signal
    pthread_cond_wait(&cond_patient, &entry_confirm_mutex);
    pthread_mutex_unlock(&entry_confirm_mutex);
    int current_time = time_store;
    int end_time = current_time + endtime;
    time_struct start_time = get_time(current_time);
    time_struct end_time_struct = get_time(end_time);

    char start_ext[3];
    char end_ext[3];
    get_time_print(&start_time, start_ext);
    get_time_print(&end_time_struct, end_ext);
    // Print the patient entering the doctor's chamber
    color_blue();
    printf("[%02d:%02d%s - %02d:%02d%s] Patient %d is in doctor's chamber\n", start_time.hour, start_time.minute, start_ext, end_time_struct.hour, end_time_struct.minute, end_ext, id);
    colore_reset();
    // Signal the assistant
    pthread_mutex_lock(&assistant_mutex);
    child_done = 1;
    pthread_cond_signal(&assistant_cond);
    pthread_mutex_unlock(&assistant_mutex);
    pthread_exit(0);
}

void *sales_function(void *arg)
{
    parameters_thread *p = (parameters_thread *)arg;
    int id = p->id;
    int endtime = p->endtime;
    pthread_mutex_lock(&entry_mutex);
    child_entered = 1;
    pthread_mutex_lock(&entry_confirm_mutex);
    pthread_cond_signal(&entry_child);
    pthread_mutex_unlock(&entry_mutex);
    pthread_cond_wait(&cond_sales, &entry_confirm_mutex);
    pthread_mutex_unlock(&entry_confirm_mutex);
    // Wait for the assistant to signal
    int current_time = time_store;
    int end_time = current_time + endtime;
    time_struct start_time = get_time(current_time);
    time_struct end_time_struct = get_time(end_time);
    char start_ext[3];
    char end_ext[3];
    get_time_print(&start_time, start_ext);
    get_time_print(&end_time_struct, end_ext);
    // Print the sales representative entering the doctor's chamber
    color_blue();
    printf("[%02d:%02d%s - %02d:%02d%s] Sales representative %d is in doctor's chamber\n", start_time.hour, start_time.minute, start_ext, end_time_struct.hour, end_time_struct.minute, end_ext, id);
    colore_reset();
    // Signal the assistant
    pthread_mutex_lock(&assistant_mutex);
    child_done = 1;
    pthread_cond_signal(&assistant_cond);
    pthread_mutex_unlock(&assistant_mutex);
    pthread_exit(0);
}

void *doctor_function(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&entry_mutex);
        child_entered = 1;
        pthread_mutex_lock(&entry_confirm_mutex);
        pthread_cond_signal(&entry_child);
        pthread_mutex_unlock(&entry_mutex);
        // Wait for the assistant to signal
        pthread_cond_wait(&cond_doctor, &entry_confirm_mutex);
        if (doctor_done == 1)
        { // If the session is over, then signal the assistant and exit
            pthread_mutex_unlock(&entry_confirm_mutex);
            int current_time = time_store;
            time_struct start_time = get_time(current_time);
            char start_ext[3];
            get_time_print(&start_time, start_ext);
            // Print the doctor leaving
            color_red();
            printf("[%02d:%02d%s] Doctor leaves\n", start_time.hour, start_time.minute, start_ext);
            colore_reset();

            pthread_mutex_lock(&assistant_mutex);
            child_done = 1;
            pthread_cond_signal(&assistant_cond);
            pthread_mutex_unlock(&assistant_mutex);
            pthread_exit(0);
        }
        pthread_mutex_unlock(&entry_confirm_mutex);
        int current_time = time_store;
        time_struct start_time = get_time(current_time);
        char start_ext[3];
        get_time_print(&start_time, start_ext);
        // Print the doctor starting the session
        color_red();
        printf("[%02d:%02d%s] Doctor has next visitor\n", start_time.hour, start_time.minute, start_ext);
        colore_reset();
        // Signal the assistant
        pthread_mutex_lock(&assistant_mutex);
        child_done = 1;
        pthread_cond_signal(&assistant_cond);
        pthread_mutex_unlock(&assistant_mutex);
    }
}