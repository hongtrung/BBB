/*Bài tập nâng cao hơn: UDP Socket
Mục tiêu: Xây dựng server-client sử dụng UDP socket.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[1024];

    // tạo socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Tạo socket thất bại");
        return -1;
    }
    
    // tạo địa chỉ cho server_addr
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // gắn địa chỉ cho server_addr
    if (bind(sockfd,(struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        perror(" Gắn địa chỉ thất bại");
        return -1;
    }
    
    printf("Server UDP đang lắng nghe...\n");

    // Nhận dữ liệu và gửi phản hồi
    while (1)
    {
        int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &addr_len);
        if(n < 0)
        {
            perror("Nhận dữ liệu thất bại");
            return -1;
        }

        buffer[n] = '\0';
        printf("Nhận tin nhắn từ client: %s\n", buffer);

        // Gửi phản hồi tới client
        sendto(sockfd, "Chào từ server UDP!", strlen("Chào từ server UDP!"), 0, (struct sockaddr*)&client_addr, addr_len);
    }
    
    close(sockfd);
    return 0;
}