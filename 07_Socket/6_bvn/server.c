#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX_CLIENTS 10

void handle_new_connection(int server_fd, fd_set *readfds, int *client_sockets, int *max_sd) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (new_socket < 0) {
        perror("Accept failed");
        return;
    }

    // Thêm vào danh sách client
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] == 0) {
            client_sockets[i] = new_socket;
            printf("Client %s connected.\n", inet_ntoa(client_addr.sin_addr));
            break;
        }
    }

    // Cập nhật max socket descriptor
    if (new_socket > *max_sd) {
        *max_sd = new_socket;
    }

    FD_SET(new_socket, readfds);
}

void handle_client_data(fd_set *readfds, int *client_sockets) {
    char buffer[1024];
    int valread;
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        int sock = client_sockets[i];
        if (FD_ISSET(sock, readfds)) {
            valread = read(sock, buffer, 1024);
            if (valread == 0) {
                // Client ngắt kết nối
                struct sockaddr_in client_addr;
                socklen_t addr_len = sizeof(client_addr);
                getpeername(sock, (struct sockaddr*)&client_addr, &addr_len);
                printf("Client %s disconnected.\n", inet_ntoa(client_addr.sin_addr));

                // Đóng socket và xóa khỏi danh sách
                close(sock);
                client_sockets[i] = 0;
            } else {
                buffer[valread] = '\0'; // Đảm bảo chuỗi kết thúc
                printf("Received from client: %s\n", buffer);
            }
        }
    }
}

int main() {
    int server_fd, new_socket, client_sockets[MAX_CLIENTS] = {0};
    struct sockaddr_in address;
    fd_set readfds;
    int max_sd;

    // Tạo socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Gắn socket vào địa chỉ và cổng
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Server đang lắng nghe...\n");

    // Lắng nghe kết nối từ client
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    // Đặt server_fd vào readfds ban đầu
    FD_ZERO(&readfds);
    FD_SET(server_fd, &readfds);
    max_sd = server_fd;

    while (1) {
        // Sao chép readfds để truyền cho select
        fd_set tempfds = readfds;

        // Sử dụng select để chờ hoạt động trên các socket
        if (select(max_sd + 1, &tempfds, NULL, NULL, NULL) < 0) {
            perror("Select failed");
            exit(EXIT_FAILURE);
        }

        // Kiểm tra các kết nối mới
        if (FD_ISSET(server_fd, &tempfds)) {
            handle_new_connection(server_fd, &tempfds, client_sockets, &max_sd);
        }

        // Kiểm tra các client đã có kết nối
        handle_client_data(&tempfds, client_sockets);
    }

    return 0;
}