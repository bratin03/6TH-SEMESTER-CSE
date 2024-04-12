// Bratin Mondal
// 21CS10016

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define PORT 30000
#define BUF 1024
#define MAX_ENT 15
#define MAX_LEN 220
#define MAX_CON 10

// #define DEBUG

int readint(int sockfd)
{
    int n;
    recv(sockfd, &n, sizeof(n), 0);
    int num = ntohl(n);
    return num;
}

void writeint(int sockfd, int n)
{
    int num = htonl(n);
    send(sockfd, &num, sizeof(num), 0);
}

typedef struct
{
    char name[MAX_LEN];
    int votes;
    int is_valid;

} table_entry;

table_entry Vote_Info_Table[MAX_ENT];

void display_option()
{
    printf("\nEnter 0 to print the list of all the entities, 1 to add new entity, 2 to delete an existing entity:\n");
}

int max(int a, int b)
{
    if (a > b)
        return a;
    return b;
}

int main()
{
    for (int i = 0; i < MAX_ENT; i++)
    {
        Vote_Info_Table[i].is_valid = 0;
        Vote_Info_Table[i].votes = 0;
    }
    int vote_req = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv, cli;
    int cli_len;
    memset(&cli, 0, sizeof(cli));
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    serv.sin_addr.s_addr = INADDR_ANY;
    bind(vote_req, (const struct sockaddr *)&serv, sizeof(serv));

    listen(vote_req, 5);

    int arr_sock[MAX_CON];
    for (int i = 0; i < MAX_CON; i++)
    {
        arr_sock[i] = -1;
    }

    display_option();
    while (1)
    {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(0, &readfds);
        FD_SET(vote_req, &readfds);
        int maxi = max(vote_req, 0);
        for (int i = 0; i < MAX_CON; i++)
        {
            if (arr_sock[i] != -1)
            {
                FD_SET(arr_sock[i], &readfds);
                maxi = max(maxi, arr_sock[i]);
            }
        }

        int ret = select(maxi + 1, &readfds, 0, 0, 0);
        if (ret < 0)
        {
            perror("select");
            exit(1);
        }
        if (FD_ISSET(0, &readfds))
        {
            int option;
            scanf("%d", &option);
            getchar();
            if (option == 0)
            {
                printf("\nVote_Info_Table:\n");
                for (int i = 0; i < MAX_ENT; i++)
                {
                    if (Vote_Info_Table[i].is_valid == 1)
                    {
                        printf("Entity Name: %s\t Votes Received: %d\n", Vote_Info_Table[i].name, Vote_Info_Table[i].votes);
                    }
                }
                printf("\n");
            }
            else if (option == 1)
            {
                char buf[MAX_LEN];
                printf("Enter Entity Name to add: ");
                fgets(buf, MAX_LEN, stdin);
                int is_add = 0;
                if (buf[strlen(buf) - 1] == '\n')
                {
                    buf[strlen(buf) - 1] = '\0';
                }
                for (int i = 0; i < MAX_ENT; i++)
                {
                    if (Vote_Info_Table[i].is_valid == 0)
                    {
                        strcpy(Vote_Info_Table[i].name, buf);
                        Vote_Info_Table[i].is_valid = 1;
                        Vote_Info_Table[i].votes = 0;
                        is_add = 1;
                        break;
                    }
                }
                if (is_add)
                {
                    printf("Entity %s added succesfully\n", buf);
                }
                else
                {
                    printf("No Space in Table\n");
                }
            }
            else if (option == 2)
            {
                char buf[MAX_LEN];
                printf("Enter Entity Name to delete: ");
                fgets(buf, MAX_LEN, stdin);
                if (buf[strlen(buf) - 1] == '\n')
                {
                    buf[strlen(buf) - 1] = '\0';
                }
                int is_deleted = 0;
                for (int i = 0; i < MAX_ENT; i++)
                {
                    if (Vote_Info_Table[i].is_valid == 1)
                    {
                        if (strcmp(Vote_Info_Table[i].name, buf) == 0)
                        {
                            Vote_Info_Table[i].is_valid = 0;
                            is_deleted = 1;
                            break;
                        }
                    }
                }
                if (is_deleted)
                {
                    printf("Entity %s deleted succesfully\n", buf);
                }
                else
                {
                    printf("No such entity in table\n");
                }
            }
            display_option();
        }
        if (FD_ISSET(vote_req, &readfds))
        {
            cli_len = sizeof(cli);
            int newfd = accept(vote_req, (struct sockaddr *)&cli, &cli_len);
            int entity_count = 0;
            char name_buffer[3 * BUF];
            int pos = 0;
            for (int i = 0; i < MAX_ENT; i++)
            {
                if (Vote_Info_Table[i].is_valid)
                {
                    entity_count++;
                    int name_len = strlen(Vote_Info_Table[i].name);
                    for (int j = 0; j < name_len; j++)
                    {
                        name_buffer[pos++] = Vote_Info_Table[i].name[j];
                    }
                    name_buffer[pos++] = '\0';
                }
            }
            writeint(newfd, entity_count);
            send(newfd, name_buffer, pos, 0);
            for (int i = 0; i < MAX_CON; i++)
            {
                if (arr_sock[i] == -1)
                {
                    arr_sock[i] = newfd;
                    break;
                }
            }
        }
        for (int i = 0; i < MAX_CON; i++)
        {
            if (arr_sock[i] != -1 && FD_ISSET(arr_sock[i], &readfds))
            {
                char vote_buf[BUF];
                int idx = 0;
                while (1)
                {
                    char c;
                    int ret = recv(arr_sock[i], &c, sizeof(c), 0);
                    if (ret == 0)
                    {
                        close(arr_sock[i]);
                        arr_sock[i] = -1;
                        break;
                    }
#ifdef DEBUG
                    printf("Recv: %c\n", c);
#endif
                    vote_buf[idx++] = c;
                    if (c == '\0')
                    {
                        break;
                    }
                }
                if (idx == 0)
                {
                    continue;
                }
                int is_Found = 0;
                for (int i = 0; i < MAX_ENT; i++)
                {
                    if (Vote_Info_Table[i].is_valid == 1)
                    {
                        if (strcmp(vote_buf, Vote_Info_Table[i].name) == 0)
                        {
                            Vote_Info_Table[i].votes++;
                            is_Found = 1;
                        }
                    }
                }
                char reply[BUF];
                if (is_Found == 1)
                {
                    strcpy(reply, "Vote Registered succesfully");
                }
                else
                {
                    strcpy(reply, "Problem in voting, Try again later");
                }
                send(arr_sock[i], reply, strlen(reply) + 1, 0);
            }
        }
    }
}