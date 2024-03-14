/**********************************************************
                    Student Information:
-----------------------------------------------------------
                    Name: Bratin Mondal
                    Student ID: 21CS10016
-----------------------------------------------------------
        Department of Computer Science and Engineering,
        Indian Institute of Technology Kharagpur.
***********************************************************/
#include "foothread.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define FILE_NAME "tree.txt"
int *param_arr;
int *parent_arr;
int *child_count;
int *partial_sums;
foothread_mutex_t write_mutex;
foothread_barrier_t barrier;
foothread_barrier_t *thread_barrier;

int nodes(void *arg)
{
    int node = *(int *)arg;
    int isleaf = (child_count[node] == 0);
    if (isleaf)
    {
        int input;
        foothread_mutex_lock(&write_mutex);
        printf("Leaf node %2d :: Enter a positive integer: ", node);
        scanf("%d", &input);
        partial_sums[parent_arr[node]] += input;
        foothread_mutex_unlock(&write_mutex);
        foothread_barrier_wait(&barrier);
        foothread_barrier_wait(&thread_barrier[parent_arr[node]]);
    }
    else
    {
        if (parent_arr[node] != node)
        {
            foothread_barrier_wait(&barrier);
            foothread_barrier_wait(&thread_barrier[node]);
            foothread_mutex_lock(&write_mutex);
            printf("Internal node %2d gets the partial sum %d from its children\n", node, partial_sums[node]);
            partial_sums[parent_arr[node]] += partial_sums[node];
            foothread_mutex_unlock(&write_mutex);
            foothread_barrier_wait(&thread_barrier[parent_arr[node]]);
        }
        else
        {
            foothread_barrier_wait(&barrier);
            foothread_barrier_wait(&thread_barrier[node]);
            foothread_mutex_lock(&write_mutex);
            printf("Sum at root (node %2d) = %d\n", node, partial_sums[node]);
            foothread_mutex_unlock(&write_mutex);
        }
    }
    foothread_exit();
    return 0;
}

int main()
{
    FILE *fp = fopen(FILE_NAME, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error opening file\n");
        exit(EXIT_FAILURE);
    }

    int n;
    fscanf(fp, "%d", &n);
    param_arr = (int *)malloc(n * sizeof(int));
    if (param_arr == NULL)
    {
        fprintf(stderr, "Error allocating memory\n");
        exit(EXIT_FAILURE);
    }
    parent_arr = (int *)malloc(n * sizeof(int));
    if (parent_arr == NULL)
    {
        fprintf(stderr, "Error allocating memory\n");
        exit(EXIT_FAILURE);
    }
    child_count = (int *)malloc(n * sizeof(int));
    if (child_count == NULL)
    {
        fprintf(stderr, "Error allocating memory\n");
        exit(EXIT_FAILURE);
    }
    partial_sums = (int *)malloc(n * sizeof(int));
    if (partial_sums == NULL)
    {
        fprintf(stderr, "Error allocating memory\n");
        exit(EXIT_FAILURE);
    }
    memset(child_count, 0, n * sizeof(int));
    memset(partial_sums, 0, n * sizeof(int));
    for (int i = 0; i < n; i++)
    {

        int x, y;
        fscanf(fp, "%d %d", &x, &y);
        parent_arr[x] = y;
        if (x != y)
            child_count[y]++;
    }
    fclose(fp);
    foothread_mutex_init(&write_mutex);
    foothread_barrier_init(&barrier, n);
    thread_barrier = (foothread_barrier_t *)malloc(n * sizeof(foothread_barrier_t));
    for (int i = 0; i < n; i++)
    {
        if (child_count[i] > 0)
        {
            foothread_barrier_init(&thread_barrier[i], child_count[i] + 1);
        }
    }
    foothread_attr_t attr;
    foothread_attr_setjointype(&attr, FOOTHREAD_JOINABLE);
    foothread_attr_setstacksize(&attr, FOOTHREAD_DEFAULT_STACK_SIZE);
    for (int i = 0; i < n; i++)
    {
        param_arr[i] = i;
        foothread_create(NULL, &attr, nodes, &param_arr[i]);
    }

    foothread_exit();
    free(param_arr);
    free(parent_arr);
    free(child_count);
    free(partial_sums);
    for (int i = 0; i < n; i++)
    {
        if (child_count[i] > 0)
        {
            foothread_barrier_destroy(&thread_barrier[i]);
        }
    }
    free(thread_barrier);
    foothread_mutex_destroy(&write_mutex);
    foothread_barrier_destroy(&barrier);
    exit(EXIT_SUCCESS);
}