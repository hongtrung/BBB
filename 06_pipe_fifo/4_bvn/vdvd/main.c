#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#define FIFO_NAME "./my_fifo"
#define MESSAGE "Hello from parent!"

pid_t child_pid;

// Hàm xử lý tín hiệu trong tiến trình con
void handle_sigusr1(int sig) {
    printf("Child received SIGUSR1 from parent.\n");
}

// Hàm đọc từ FIFO trong tiến trình con
void *child_read_fifo(void *arg) {
    int fifo_fd;
    char buffer[256];

    // Mở FIFO để đọc
    fifo_fd = open(FIFO_NAME, O_RDONLY);
    if (fifo_fd < 0) {
        perror("Child failed to open FIFO");
        pthread_exit(NULL);
    }

    // Đọc thông điệp từ FIFO
    read(fifo_fd, buffer, sizeof(buffer));
    printf("Child read from FIFO: %s\n", buffer);

    // Gửi tín hiệu lại cho tiến trình cha
    kill(getppid(), SIGUSR1);

    close(fifo_fd);
    pthread_exit(NULL);
}

// Hàm xử lý tín hiệu trong tiến trình cha
void handle_parent_sigusr1(int sig) {
    printf("Parent received SIGUSR1 from child.\n");
}

// Hàm ghi vào FIFO trong tiến trình cha
void *parent_write_fifo(void *arg) {
    int fifo_fd;

    // Mở FIFO để ghi
    fifo_fd = open(FIFO_NAME, O_WRONLY);
    if (fifo_fd < 0) {
        perror("Parent failed to open FIFO");
        pthread_exit(NULL);
    }

    // Ghi thông điệp vào FIFO
    write(fifo_fd, MESSAGE, strlen(MESSAGE) + 1);
    close(fifo_fd);
    pthread_exit(NULL);
}

int main() {
    // Tạo FIFO
    if (mkfifo(FIFO_NAME, 0666) == -1 && errno != EEXIST) {
        perror("Failed to create FIFO");
        exit(EXIT_FAILURE);
    }

    // Tạo tiến trình con
    child_pid = fork();
    if (child_pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
        // Tiến trình con

        // Cài đặt handler cho SIGUSR1
        signal(SIGUSR1, handle_sigusr1);

        // Tạo luồng đọc từ FIFO
        pthread_t read_thread;
        pthread_create(&read_thread, NULL, child_read_fifo, NULL);

        // Chờ luồng hoàn thành
        pthread_join(read_thread, NULL);

    } else {
        // Tiến trình cha

        // Cài đặt handler cho SIGUSR1
        signal(SIGUSR1, handle_parent_sigusr1);

        // Tạo luồng ghi vào FIFO
        pthread_t write_thread;
        pthread_create(&write_thread, NULL, parent_write_fifo, NULL);

        // Gửi tín hiệu cho tiến trình con
        sleep(1); // Đợi con cài đặt xong signal handler
        kill(child_pid, SIGUSR1);

        // Chờ luồng hoàn thành
        pthread_join(write_thread, NULL);

        // Đợi tín hiệu từ tiến trình con
        // pause();
    }

    // Xóa FIFO sau khi xong
    unlink(FIFO_NAME);
    return 0;
}
