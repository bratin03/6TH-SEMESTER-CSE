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
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>

typedef struct Page_Table_Entry
{
    int frame_no;
    int valid;
} PTE;

typedef struct Page_Table
{
    int size;
    PTE *entries;
} PT;

void init_page_table(PT *pt, int size)
{
    pt->size = size;
    pt->entries = (PTE *)malloc(size * sizeof(PTE));
    for (int i = 0; i < size; i++)
    {
        pt->entries[i].frame_no = -1;
        pt->entries[i].valid = 0;
    }
}

PT **init_page_tables(int n, int size)
{
    PT **page_tables = (PT **)malloc(n * sizeof(PT *));
    for (int i = 0; i < n; i++)
    {
        page_tables[i] = (PT *)malloc(sizeof(PT));
        init_page_table(page_tables[i], size);
    }
    return page_tables;
}

typedef 