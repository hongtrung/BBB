#ifndef SERVER_H
#define SERVER_H

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

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
volatile sig_atomic_t running = 1;
// ham xu ly loi 
void handle_error(const char *msg);

// Hàm khởi tạo và chạy server
void socket_server(void *args);

// ham xu ly ket noi client
void handle_new_connection(int server_fd, int client_socket[], struct sockaddr_in *address)

#endif // SERVER_H
