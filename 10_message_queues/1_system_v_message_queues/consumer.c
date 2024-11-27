#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// structure for message queues
struct message_buffer
{
    long message_type;
    char message_text[100];
}message;

int main()
{
    key_t key;
    int msgid;
    
    //ftok tp generate unique key
    key = ftok("progfile", 65);

    //msgget creates a message queue and returns identifier
    msgid = msgget(key, 0666 | IPC_CREAT );

    // msgrcv to receive message
    msgrcv(msgid, &message, sizeof(message), 1, 0);

    //display the message
    printf("data received is: %s \n", message.message_text);

    //to destroy the message queue
    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}