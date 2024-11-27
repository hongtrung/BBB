#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// Semaphore và Mutex
sem_t semaphore;
pthread_mutex_t mutex;

// Biến chia sẻ
int shared_resource = 0; // Tài nguyên chung
int product_available = 0; // Biến trạng thái sản phẩm

void *producer(void *arg) {
    while (1) {
        // Giả lập quá trình sản xuất
        sleep(1); // Tạm dừng để mô phỏng sản xuất
        pthread_mutex_lock(&mutex);
        shared_resource++; // Tạo sản phẩm
        product_available = 1; // Đánh dấu có sản phẩm
        printf("Producer: Sản xuất sản phẩm %d\n", shared_resource);
        pthread_mutex_unlock(&mutex);

        // Thông báo cho các tiến trình tiêu thụ
        sem_post(&semaphore);
    }
    return NULL;
}

void *consumer(void *arg) {
    int consumer_id = *((int *)arg);
    while (1) {
        // Đợi có sản phẩm
        sem_wait(&semaphore);

        pthread_mutex_lock(&mutex);
        if (product_available) {
            printf("Consumer %d: Tiêu thụ sản phẩm %d\n", consumer_id, shared_resource);
            product_available = 0; // Đánh dấu sản phẩm đã được tiêu thụ
        }
        pthread_mutex_unlock(&mutex);

        // Giả lập quá trình tiêu thụ
        sleep(2); // Tạm dừng để mô phỏng tiêu thụ
    }
    return NULL;
}

int main() {
    pthread_t producer_thread, consumer_thread1, consumer_thread2;
    int consumer_id1 = 1, consumer_id2 = 2;

    // Khởi tạo Semaphore và Mutex
    sem_init(&semaphore, 0, 0); // Semaphore ban đầu là 0
    pthread_mutex_init(&mutex, NULL);

    // Tạo tiến trình
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread1, NULL, consumer, &consumer_id1);
    pthread_create(&consumer_thread2, NULL, consumer, &consumer_id2);

    // Chờ các tiến trình kết thúc (trong thực tế sẽ chạy vô hạn)
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread1, NULL);
    pthread_join(consumer_thread2, NULL);

    // Hủy Semaphore và Mutex
    sem_destroy(&semaphore);
    pthread_mutex_destroy(&mutex);

    return 0;
}
