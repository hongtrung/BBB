#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/wait.h>

#define CLIENT_MAX 10
#define BUFFER_SIZE 1024


int main(int argc, char* argv[])
{
    // kiem tra tham so dong lenh
    if (argc != 2) 
    {
        fprintf(stderr, "Usage: %s <port number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

}
