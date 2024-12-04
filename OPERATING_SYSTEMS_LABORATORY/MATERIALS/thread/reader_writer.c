#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define READER_COUNT 10
#define WRITER_COUNT 5

pthread_mutex_t mutex;
pthread_mutex_t wrt;
pthread_mutex_t IO;

int reader_count = 0;

void *reader_thread(void *arg);
void *writer_thread(void *arg);

int main()
{
    srand(time(0));
    printf("Reader Writer Problem\n");
    pthread_t reader[READER_COUNT], writer[WRITER_COUNT];
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&wrt, NULL);
    pthread_mutex_init(&IO, NULL);
    int i, id[READER_COUNT + WRITER_COUNT];
    pthread_mutex_lock(&IO);
    printf("Creating Readers and Writers\n");
    pthread_mutex_unlock(&IO);
    for (i = 0; i < READER_COUNT; i++)
    {
        pthread_mutex_lock(&IO);
        printf("Reader %d\n", i + 1);
        pthread_mutex_unlock(&IO);
        id[i] = i + 1;
        pthread_create(&reader[i], NULL, reader_thread, &id[i]);
    }
    pthread_mutex_lock(&IO);
    printf("Writer Count: %d\n", WRITER_COUNT);
    pthread_mutex_unlock(&IO);
    for (i = 0; i < WRITER_COUNT; i++)
    {
        pthread_mutex_lock(&IO);
        printf("Writer %d\n", i + 1);
        pthread_mutex_unlock(&IO);
        id[i + READER_COUNT] = i + 1;
        pthread_create(&writer[i], NULL, writer_thread, &id[i + READER_COUNT]);
    }

    pthread_mutex_lock(&IO);
    printf("\n\n");
    pthread_mutex_unlock(&IO);

    for (i = 0; i < READER_COUNT; i++)
    {
        pthread_join(reader[i], NULL);
    }
    for (i = 0; i < WRITER_COUNT; i++)
    {
        pthread_join(writer[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&wrt);
    return 0;
}

void *reader_thread(void *arg)
{
    int my_id = *(int *)arg;
    int sleep_time = 1 + rand() % 5;
    sleep(sleep_time);
    pthread_mutex_lock(&IO);
    printf("Reader %d Arrived\n", my_id);
    pthread_mutex_unlock(&IO);
    pthread_mutex_lock(&mutex);
    reader_count++;
    if (reader_count == 1)
    {
        pthread_mutex_lock(&wrt);
    }
    pthread_mutex_unlock(&mutex);
    FILE *file = fopen("shared.txt", "r");
    if (file == NULL)
    {
        printf("Error in opening file\n");
        exit(1);
    }
    pthread_mutex_lock(&IO);
    printf("Reader %d is reading\n", my_id);
    pthread_mutex_unlock(&IO);

    char dest[100];
    sprintf(dest, "Reader_%d.txt", my_id);
    FILE *dest_file = fopen(dest, "w");
    char ch;
    while ((ch = fgetc(file)) != EOF)
    {
        fputc(ch, dest_file);
    }
    fclose(dest_file);
    fclose(file);
    printf("\n");
    pthread_mutex_lock(&mutex);
    reader_count--;
    if (reader_count == 0)
    {
        pthread_mutex_unlock(&wrt);
    }
    pthread_mutex_unlock(&mutex);
    pthread_mutex_lock(&IO);
    printf("Reader %d Left\n", my_id);
    pthread_mutex_unlock(&IO);
    return NULL;
}

void *writer_thread(void *arg)
{
    int my_id = *(int *)arg;
    int sleep_time = rand() % 5;
    sleep(sleep_time);
    pthread_mutex_lock(&IO);
    printf("Writer %d Arrived\n", my_id);
    pthread_mutex_unlock(&IO);
    pthread_mutex_lock(&wrt);
    FILE *file = fopen("shared.txt", "a");
    if (file == NULL)
    {
        printf("Error in opening file\n");
        exit(1);
    }
    printf("Writer %d is writing\n", my_id);
    fprintf(file, "Writer %d is writing\n", my_id);
    char to_write[100];
    sprintf(to_write, "I am writer %d\n", my_id);
    fprintf(file, "%s", to_write);
    fclose(file);
    pthread_mutex_unlock(&wrt);
    pthread_mutex_lock(&IO);
    printf("Writer %d Left\n", my_id);
    pthread_mutex_unlock(&IO);
    return NULL;
}
