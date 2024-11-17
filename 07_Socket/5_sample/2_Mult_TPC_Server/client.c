#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in server_address;
    char *message = "Xin chào từ client!";
    char buffer[1024] = {0};

    // Tạo socket 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket tạo thất bại");
        return -1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // Chuyển đổi địa chỉ IP sang dạng binary
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("Địa chỉ không hợp lệ");
        return -1;
    }

    // Kết nối đến server
    if(connect(sock,(struct sockaddr*)&server_address, sizeof(server_address)) < 0 )
    {
        perror(" ket noi that bai");
        return -1;
    }

    // Gửi dữ liệu đến server
    send(sock, message, strlen(message), 0);
    printf("Dữ liệu gửi đến server: %s\n", message);

    // Nhận phản hồi từ server
    read(sock, buffer, 1024);
    printf("Nhận từ server: %s\n", buffer);

    // Đóng kết nối
    close(sock);
    return 0;
}