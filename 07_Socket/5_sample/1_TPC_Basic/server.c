/* BT cơ bản: TCP Client và Server đơn giản
Mục tiêu: tạo 1 server và 1 client đơn giản sử dụng TCP socket trong C.
        TCP: stream socket   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() 
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    char buff[1024] = {0};

    // tạo socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }
    
    // đăng ký địa chỉ cho socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    //  gắn địa chỉ cho socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1)
    {
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    printf("Server dang lang nghe...\n");
    // lắng nghe kết nối từ client
    if (listen(server_fd,3) == -1)
    {
        perror("listen() failed");
        exit(EXIT_FAILURE);
    }
    
    // chấp nhận kết nối từ client
    socklen_t addrlen = sizeof(address);
    new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    if (new_socket == -1)
    {
        perror("accept() failed");
        exit(EXIT_FAILURE);
    }
    
    // nhận và gửi dữ liệu
    read(new_socket, buff, 1024);
    printf("Client gui: %s\n", buff);

    send(new_socket, "chao tu Server!", strlen("chao tu Server!"), 0);

    // đóng kết nối
    close(new_socket);
    close(server_fd);

    return 0;
}
