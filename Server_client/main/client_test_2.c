#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define BUFFER_SIZE 2048
#define client_id 2

int main(int argc, char* argv[]) 
{
    int client_socket;
    struct sockaddr_in server_address;
    char message[BUFFER_SIZE];

    // kiem tra tham so dong lenh
    if (argc != 3) 
    {
        fprintf(stderr, "Usage: %s <hostname> <port number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);
    // Khởi tạo random number generator
    srand(time(NULL));

    // Tạo socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Cấu hình địa chỉ server
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip, &server_address.sin_addr) <= 0) 
    {
        perror("Invalid address or address not supported");
        exit(EXIT_FAILURE);
    }

    // Kết nối đến server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) 
    {
        perror("Connection failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Kết nối thành công tới server %s:%d\n", server_ip, server_port);

    // Gửi dữ liệu mỗi giây
    for (int i = 1; i < 11; i++)
    {
        // tao nhiet do va timestamp
        float temperature = (rand() % 10 + 20) + (rand() % 10) / 10.0;
        time_t timestamp = time(NULL);

        // create message
        snprintf(message, sizeof(message), "ID%d/%.2f/%ld", client_id, temperature, timestamp);

        // send to server
        send(client_socket, message, strlen(message), 0);
        printf("Message number %d sent to server: %s\n", i, message);
        sleep(2);
    }
    close(client_socket);
    return 0;
}
