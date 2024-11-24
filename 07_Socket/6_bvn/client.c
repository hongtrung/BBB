#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"

int main() {
    int sock = 0;
    struct sockaddr_in server_addr;
    char message = "Hello, Server!";

    // Tạo socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket failed");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Chuyển đổi địa chỉ IP sang dạng binary
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        return -1;
    }

    // Kết nối đến server
    if (connect(sock, (struct sockaddr)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    printf("Connected to server\n");

    // Gửi thông điệp đến server
    send(sock, message, strlen(message), 0);
    printf("Message sent: %s\n", message);

    // Ngắt kết nối
    close(sock);
    printf("Disconnected from server\n");

    return 0;
}