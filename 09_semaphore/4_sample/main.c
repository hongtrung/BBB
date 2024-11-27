#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_t producer_thread, consumer_pthread1, consumer_pthread2;
pthread_mutex_t mutex;
sem_t semaphore; 

int buffer = 0;

void* producer(void* argv)
{
    while (buffer < 2)
    {   
        sleep(1);
        pthread_mutex_lock(&mutex);
        buffer++;
        printf("Produce: da tang Buffer len %d\n", buffer);
        if (buffer == 2)
        {
            printf("Buffer =%d , ket thuc chuong trinh\n", buffer);
            pthread_exit(NULL);
        }
        pthread_mutex_unlock(&mutex);
        sem_post(&semaphore);

    }
    return 0;
}

void* consumer_1(void* argv)
{   
    
    while(1)
    {   
        if (buffer == 2)
        {
            printf("Buffer =%d , ket thuc chuong trinh\n", buffer);
            pthread_exit(NULL);
        }
        sem_wait(&semaphore);
        pthread_mutex_lock(&mutex);
        buffer--;
        printf("\tConsumer_1: da giam Buffer di 1\n");
        pthread_mutex_unlock(&mutex);
        sleep(2);
    }
    
    return 0;
}

void* consumer_2(void* argv)
{   
    while(1)
    {   
        if (buffer == 2)
        {
            printf("Buffer =%d , ket thuc chuong trinh\n", buffer);
            pthread_exit(NULL);
        }
        sem_wait(&semaphore);    
        pthread_mutex_lock(&mutex);
        buffer--;
        printf("\tConsumer_2: da giam Buffer di 1\n");
        pthread_mutex_unlock(&mutex);
        sleep(3);
    }
    
    return 0;

}

int main ()
{
    // Create semaphore
    sem_init(&semaphore, 0, 0);
    // Create mutex
    pthread_mutex_init(&mutex, NULL);
    // Create pthread
    if(pthread_create(&producer_thread, NULL, producer, NULL) != 0)
    {
        perror("create Producer failed\n");
		exit(1);
    }

    if(pthread_create(&consumer_pthread1, NULL, consumer_1, NULL) != 0)
    {
        perror("create Consumer_1 failed\n");
        exit(1);
    }
    if(pthread_create(&consumer_pthread2, NULL, consumer_2, NULL) != 0)
    {
        perror("create Consumer_2 failed\n");
        exit(1);
    }

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_pthread1, NULL);
    pthread_join(consumer_pthread2, NULL);

    sem_destroy(&semaphore);
    pthread_mutex_destroy(&mutex);
    return 0;
}