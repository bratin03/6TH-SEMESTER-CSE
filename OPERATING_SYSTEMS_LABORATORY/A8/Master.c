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

#define DEBUG
#define VERBOSE

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

typedef struct Free_Frame_List
{
    int size;
    int *frames;
} FFL;

void init_free_frame_list(FFL *ffl, int size)
{
    ffl->size = size;
    ffl->frames = (int *)malloc(size * sizeof(int));
    for (int i = 0; i < size; i++)
    {
        ffl->frames[i] = i;
    }
}

int get_free_frame(FFL *ffl)
{
    if (ffl->size == 0)
    {
        return -1;
    }
    int frame = ffl->frames[0];
    for (int i = 0; i < ffl->size - 1; i++)
    {
        ffl->frames[i] = ffl->frames[i + 1];
    }
    ffl->size--;
    return frame;
}

int add_free_frame(FFL *ffl, int frame)
{
    ffl->frames[ffl->size] = frame;
    ffl->size++;
    return 0;
}

typedef struct Page_Number_Mapping
{
    int size;
    int *frames;
} PNM;

void init_page_number_mapping(PNM *pnm, int size)
{
    pnm->size = size;
    pnm->frames = (int *)malloc(size * sizeof(int));
    for (int i = 0; i < size; i++)
    {
        pnm->frames[i] = -1;
    }
}

void add_page_number_mapping(PNM *pnm, int page_no, int frame_no)
{
    pnm->frames[page_no] = frame_no;
}

int get_frame_number(PNM *pnm, int page_no)
{
    return pnm->frames[page_no];
}

int main()
{
    int k, m, f;
    printf("Enter Total Number of Processes: ");
    scanf("%d", &k);
    printf("Enter Maximum Number of Pages required by each Process: ");
    scanf("%d", &m);
    printf("Enter Total Number of Frames: ");
    scanf("%d", &f);
}