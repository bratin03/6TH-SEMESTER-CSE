#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_MSG_SIZE 1024

// Define a structure for the message
struct message
{
    long msg_type;
    char msg_text[MAX_MSG_SIZE];
};

int main()
{
    key_t key;
    int msgid;
    struct message msg;

    // Generate a unique key
    if ((key = ftok("/usr/bin", 1)) == -1)
    {
        perror("ftok");
        exit(1);
    }

    // Create a message queue or get the identifier of an existing one
    if ((msgid = msgget(key, 0644 | IPC_CREAT)) == -1)
    {
        perror("msgget");
        exit(1);
    }

    printf("Message queue created/opened successfully, id = %d\n", msgid);

    // Send a message to the queue
    printf("Enter a message to send: ");
    fgets(msg.msg_text, MAX_MSG_SIZE, stdin);
    msg.msg_type = 1; // Message type can be any positive integer

    if (msgsnd(msgid, &msg, sizeof(msg.msg_text), 0) == -1)
    {
        perror("msgsnd");
        exit(1);
    }
    
    strcpy(msg.msg_text, "Hello from the other side!");
    printf("Message sent successfully.\n");

    // Receive a message from the queue
    if (msgrcv(msgid, &msg, sizeof(msg.msg_text), 0, 0) == -1)
    {
        perror("msgrcv");
        exit(1);
    }

    printf("Received message: %s\n", msg.msg_text);

    // Remove the message queue
    if (msgctl(msgid, IPC_RMID, NULL) == -1)
    {
        perror("msgctl");
        exit(1);
    }

    printf("Message queue removed successfully.\n");

    return 0;
}
