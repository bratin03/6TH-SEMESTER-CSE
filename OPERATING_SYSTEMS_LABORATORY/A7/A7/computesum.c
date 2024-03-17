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

// Define the file name
#define FILE_NAME "tree.txt"
// Declare pointers to store the tree information
int *param_arr;
int *parent_arr;
int *child_count;
int *partial_sums;
// Declare the mutex and barrier
foothread_mutex_t write_mutex;
foothread_barrier_t barrier;
foothread_barrier_t *thread_barrier;

// Function to simulate the nodes
int nodes(void *arg)
{
    // Get the node number
    int node = *(int *)arg;
    // Check if the node is a leaf node
    int isleaf = (child_count[node] == 0);
    if (isleaf)
    {
        // Get the input from the user if the node is a leaf node
        int input;
        foothread_mutex_lock(&write_mutex);
        printf("Leaf node %2d :: Enter a positive integer: ", node);
        scanf("%d", &input);
        partial_sums[parent_arr[node]] += input; // Add the input to the parent's partial sum
        foothread_mutex_unlock(&write_mutex);
        foothread_barrier_wait(&barrier);                          // Wait for the barrier for all nodes to finish
        foothread_barrier_wait(&thread_barrier[parent_arr[node]]); // Wait for the barrier for the siblings to finish
    }
    else
    {
        if (parent_arr[node] != node)
        {
            // If internal node
            foothread_barrier_wait(&barrier);              // Wait for the barrier for all nodes to finish
            foothread_barrier_wait(&thread_barrier[node]); // Wait for the barrier for the children to finish
            foothread_mutex_lock(&write_mutex);
            printf("Internal node %2d gets the partial sum %d from its children\n", node, partial_sums[node]); // Print the partial sum of the node
            partial_sums[parent_arr[node]] += partial_sums[node];                                              // Add the partial sum to the parent's partial sum
            foothread_mutex_unlock(&write_mutex);
            foothread_barrier_wait(&thread_barrier[parent_arr[node]]); // Wait for the barrier for the siblings to finish
        }
        else
        {
            // If root node
            foothread_barrier_wait(&barrier);              // Wait for the barrier for all nodes to finish
            foothread_barrier_wait(&thread_barrier[node]); // Wait for the barrier for the children to finish
            foothread_mutex_lock(&write_mutex);
            printf("Internal node %2d gets the partial sum %d from its children\n", node, partial_sums[node]); // Print the partial sum of the node
            printf("Sum at root (node %2d) = %d\n", node, partial_sums[node]);                                 // Print the sum at the root node
            foothread_mutex_unlock(&write_mutex);
        }
    }
    foothread_exit(); // Exit the thread
    return 0;
}

int main()
{
    FILE *fp = fopen(FILE_NAME, "r"); // Open the file
    if (fp == NULL)
    {
        fprintf(stderr, "Error opening file\n");
        exit(EXIT_FAILURE);
    }
    // Read the number of nodes
    int n;
    fscanf(fp, "%d", &n);
    // Allocate memory for the arrays
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
        parent_arr[x] = y; // Store the parent of the node
        if (x != y)
            child_count[y]++; // Increment the child count of the parent
    }
    fclose(fp);
    foothread_mutex_init(&write_mutex);
    foothread_barrier_init(&barrier, n);
    thread_barrier = (foothread_barrier_t *)malloc(n * sizeof(foothread_barrier_t));
    for (int i = 0; i < n; i++)
    {
        if (child_count[i] > 0)
        {
            foothread_barrier_init(&thread_barrier[i], child_count[i] + 1); // Initialize the barrier for the node if not a leaf node
        }
    }
    // Declare and initialize the thread attributes
    foothread_attr_t attr;
    foothread_attr_setjointype(&attr, FOOTHREAD_JOINABLE);
    foothread_attr_setstacksize(&attr, FOOTHREAD_DEFAULT_STACK_SIZE);
    for (int i = 0; i < n; i++)
    {
        // Create the threads
        param_arr[i] = i;
        foothread_create(NULL, &attr, nodes, &param_arr[i]);
    }

    // Wait for the threads to finish
    foothread_exit();
    // Clean up the resources
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