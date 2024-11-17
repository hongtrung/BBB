/* Bt nâng cao: Multithreaded TCP Server
Mục tiêu: Xây dựng server có thể xử lý nhiều kết nối đồng thời bằng các sử dụng các thread*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080

void* handle_client(void* socket_desc)
{
    int sock = *(int*)socket_desc;
    // int sock;
    char buffer[1024] = {0};

    // nhận dữ liệu từ client
    read(sock, buffer, 1024);
    printf("nhan tu client: %s\n", buffer);
    // phản hồi lại client
    send(sock,"Chao tu server!",strlen("Chao tu server!\n"), 0 );
    // đóng kết nối
    close(sock);
    free(socket_desc);
    return NULL;
}

int main() 
{
    // khởi tạo các value
    int server_fd, new_socket, *new_sock;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    // tạo socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // tạo địa chỉ cho socket
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    // gắn địa chỉ cho socket
    if (bind(server_fd, (struct sockaddr*)&address, addrlen) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    // lắng nghe kết nối từ client
    if (listen(server_fd, 3) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Server dang lang nghe...\n");

    // chấp nhận các kết nối
    while ((new_socket = accept(server_fd,(struct sockaddr*)&address, &addrlen)) > 0)
    {
        printf("ket noi moi tu client");

        // tạo thread để xử lý mỗi kết nối
        new_sock = malloc(1);
        *new_sock = new_socket;
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client,(void*)new_sock) < 0)
        {
            perror("create thread failed");
            exit(EXIT_FAILURE);
        }
    }
    
    if (new_socket < 0)
    {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    
    close(server_fd);
    return 0;
}