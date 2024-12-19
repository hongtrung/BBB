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
#include <errno.h>


#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

void handle_error(const char *msg) 
{
    perror(msg);
    exit(EXIT_FAILURE);
}

typedef struct Node
{
    int ID;
    float TEMP;
    time_t TIME;
    struct Node* next;
}Node;


void add_to_linked_list(Node **head, int ID, float TEMP, time_t TIME)
{
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL)
    {
        handle_error("Malloc failed");
    }
    new_node->ID = ID; 
    new_node->TEMP = TEMP; 
    new_node->TIME = TIME; 
    new_node->next = *head; 
    *head = new_node;
}

void write_linked_list_to_file(Node* head, const char* filename) 
{
    FILE *file = fopen(filename, "a"); 
    if (file == NULL) 
    { 
        handle_error("fopen failed");
    } 
    fseek(file, 0, SEEK_END);
    if(ftell(file) == 0)
    {
        fprintf(file, "ID\tTEMPERATURE\tTIME\n");
    }
    Node *current = head; 
    while (current != NULL) 
    { 
        fprintf(file, "%d\t%.2f\t%ld\n", current->ID, current->TEMP, current->TIME);
        current = current->next;  
    }
    fclose(file);
}

void clear_linked_list(Node **head) 
{ 
    Node *current = *head; 
    while (current != NULL) 
    { 
        Node *temp = current; 
        current = current->next; 
        free(temp); 
    } 
    *head = NULL;
}

void free_linked_list(Node *head) 
{ 
    Node *current = head; 
    while (current != NULL) 
    { 
        Node *temp = current; 
        current = current->next; 
        free(temp); 
    } 
}

void log_unusual_data(char* ID, const char* status, float TEMP, char* time)
{
    FILE *file = fopen("Unusual.txt", "a");
    if (file == NULL) 
    {
        handle_error("fopen failed");
    }

    fprintf(file, "%s - %s – %.2f – %s\n", ID, status, TEMP, time);
    fclose(file);
}

void parsePhrase(const char *phrase) 
{
    char id[10];
    float temperature;
    char time[15];
    int result = sscanf(phrase, "%[^/]/%f/%s", id, &temperature, time);
    if (result != 3) 
    { 
        handle_error("phrase failed");
    }
    if (temperature >= 20 && temperature < 23) 
    {
        printf("TOO COLD\n");
        log_unusual_data(id, "TOO COLD", temperature, time);
    } 
    else if (temperature >= 23 && temperature <= 26) 
    {
        printf("NORMAL\n");
    } 
    else if (temperature > 26 && temperature <= 30) 
    {
        printf("TOO HOT\n");
        log_unusual_data(id, "TOO HOT", temperature, time);
    } 
    else 
    {
        printf("OUT OF RANGE\n");
        log_unusual_data(id, "OUT OF RANGE", temperature, time);
    }
}

void supervisor(const char* client_ip, const char* event, time_t timestamp) 
{
    FILE *file = fopen("Supervisor.txt", "a");
    if (file == NULL) 
    {
        handle_error("fopen failed");
    }

    fprintf(file, "Client – %s - %s – %ld\n", client_ip, event, timestamp);
    fclose(file);
}

void handle_new_connection(int server_fd, int client_socket[], struct sockaddr_in *address) 
{
    int new_socket;
    socklen_t addrlen = sizeof(*address);

    if ((new_socket = accept(server_fd, (struct sockaddr *)address, &addrlen)) < 0) 
    {
        handle_error("Accept failed");
    }

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(address->sin_addr), client_ip, INET_ADDRSTRLEN); 
    time_t timestamp = time(NULL);
    supervisor(client_ip, "Connect", timestamp);

    printf("client connected: IP: %s, PORT: %d, %ld\n", inet_ntoa(address->sin_addr), ntohs(address->sin_port), timestamp);

    for (int i = 0; i < MAX_CLIENTS; i++) 
    {
        if (client_socket[i] == 0) 
        {
            client_socket[i] = new_socket;
            break;
        }
    }
}

void handle_client_data(int client_socket[], fd_set *readfds, int pipe_fds[]) 
{
    char buffer[BUFFER_SIZE];
    struct sockaddr_in address;
    char client_ip[INET_ADDRSTRLEN];
    socklen_t addrlen = sizeof(address);

    for (int i = 0; i < MAX_CLIENTS; i++) 
    {
        int sd = client_socket[i];

        if (FD_ISSET(sd, readfds)) 
        {
            int valread = read(sd, buffer, BUFFER_SIZE);
            if (valread == 0) 
            {
                getpeername(sd, (struct sockaddr *)&address, &addrlen);
                inet_ntop(AF_INET, &(address.sin_addr), client_ip, INET_ADDRSTRLEN);
                int client_port = ntohs(address.sin_port);
                time_t timestamp = time(NULL);
                supervisor(client_ip, "Disconnect", timestamp);
                printf("Client disconnected: %s: %d: %ld\n", client_ip, client_port, timestamp);
                close(sd);
                client_socket[i] = 0;
            } 
            else 
            {
                buffer[valread] = '\0';
                inet_ntop(AF_INET, &(address.sin_addr), client_ip, INET_ADDRSTRLEN);
                int client_port = ntohs(address.sin_port);
                printf("Received from %s:%d -> %s -> ", client_ip, client_port, buffer);
                parsePhrase(buffer);

                // Ghi dữ liệu vào pipe để chuyển cho tiến trình con
                int ID = i;
                float TEMP;
                time_t TIME;
                char client_id[10], time_str[15];
                sscanf(buffer, "ID%[^/]/%f/%s", client_id, &TEMP, time_str);
                TIME = atoi(time_str);

                char filename[BUFFER_SIZE];
                snprintf(filename, sizeof(filename), "client_%s.txt", client_id);
                write(pipe_fds[1], &ID, sizeof(ID));
                write(pipe_fds[1], &TEMP, sizeof(TEMP));
                write(pipe_fds[1], &TIME, sizeof(TIME));
                write(pipe_fds[1], filename, sizeof(filename)); // Ghi kích thước đúng của filename
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) 
    {
        fprintf(stderr, "Usage: %s <port number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server_fd, max_sd, activity, sd;
    int client_socket[MAX_CLIENTS] = {0};
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    fd_set readfds;
    char buffer[BUFFER_SIZE];

    int pipe_fds[2]; 
    if (pipe(pipe_fds) == -1) 
    { 
        handle_error("Pipe failed"); 
    }

    int server_port = atoi(argv[1]);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        handle_error("Socket failed");
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(server_port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    {
        handle_error("Bind failed");
    }

    if (listen(server_fd, 10) < 0) 
    {
        handle_error("Listen failed");
    }
    printf("Listening on port %d...\n", server_port);

    pid_t pid = fork();
    if (pid < 0) 
    { 
        handle_error("Fork failed"); 
    } 
    else if (pid == 0) 
    { //  process con
        close(pipe_fds[1]); 
        Node *head = NULL; 
        while (1) 
        {
            int ID; 
            float TEMP; 
            time_t TIME; 
            char filename[BUFFER_SIZE]; 
            ssize_t size = read(pipe_fds[0], &ID, sizeof(ID));
            if (size <= 0) 
                break; 
            read(pipe_fds[0], &TEMP, sizeof(TEMP)); 
            read(pipe_fds[0], &TIME, sizeof(TIME)); 
            read(pipe_fds[0], filename, BUFFER_SIZE); 
            add_to_linked_list(&head, ID, TEMP, TIME); 
            write_linked_list_to_file(head, filename); 
            clear_linked_list(&head);
        } 
        free_linked_list(head); 
        close(pipe_fds[0]); 
        exit(EXIT_SUCCESS); 
    } 
    else 
    { // process cha 
        close(pipe_fds[0]);
        while (1) 
        {
            FD_ZERO(&readfds);
            FD_SET(server_fd, &readfds);
            max_sd = server_fd;

            for (int i = 0; i < MAX_CLIENTS; i++) 
            {
                sd = client_socket[i];
                if (sd > 0) 
                {
                    FD_SET(sd, &readfds);
                }
                if (sd > max_sd) 
                {
                    max_sd = sd;
                }
            }

            struct timeval timeout;
            timeout.tv_sec = 3;
            timeout.tv_usec = 0;

            activity = select(max_sd + 1, &readfds, NULL, NULL, &timeout);
            if ((activity < 0) && (errno != EINTR)) {
                handle_error("Select error");
            }

            if (activity == 0) {
                printf("No activity within 3 seconds. Exiting...\n");
                break;
            }

            if (FD_ISSET(server_fd, &readfds)) 
            {
                handle_new_connection(server_fd, client_socket, &address);
            }

            handle_client_data(client_socket, &readfds, pipe_fds);

        }
    }
    close(pipe_fds[1]);
    wait(NULL);
    close(server_fd);
    return 0;
}
