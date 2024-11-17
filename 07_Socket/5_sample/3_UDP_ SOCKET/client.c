#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define PORT 8080
int main()
{
    int sockfd;
    struct sockaddr_in server_addr;
    char* message = "Xin chao server";
    char buffer[1024];

    // Tạo socket client udp
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0 )
    {
        perror("tạo socket thất bại");
        return -1;
    }
    
    // tạo địa chỉ cho client socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Chuyển đổi địa chỉ IP sang dạng binary
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0)
    {
        perror("Địa chỉ không phù hợp");
        return -1;
    }
    
    // Gửi dữ liệu đến server
    sendto(sockfd, message, sizeof(message), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("Dữ liệu gửi đến server: %s\n", message);

    // Nhận dữ liệu từ server
    int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
    buffer[n] = '\0';
    printf("Nhận từ server: %s\n", buffer);

    close(sockfd);
    return 0;

}