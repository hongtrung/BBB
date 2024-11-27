#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define BUFFER_SIZE 100
// structure for message queues
struct message_buffer
{
    long message_type;
    char message_text[BUFFER_SIZE];
}message;

int main()
{
    key_t key;
    int msgid;
    
    //ftok tp generate unique key
    key = ftok("progfile", 65);

    //msgget creates a message queue and returns identifier
    msgid = msgget(key, 0666 | IPC_CREAT);

    printf("Enter type message: ");
    scanf("%ld", &message.message_type);
    stdin = freopen(NULL, "r", stdin);

    printf("Enter message: ");
    fgets(message.message_text, BUFFER_SIZE, stdin);

    //msgsnd to send message
    msgsnd(msgid, &message, sizeof(message), 0);

    return 0;
}
