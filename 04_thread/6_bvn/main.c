#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <stdbool.h>

volatile bool paused = false; // paused = 0;
volatile bool stop = false;   // stop = 0;

pthread_t worker_thread;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void sig_handler1(int num)
{
    pthread_mutex_lock(&mutex);
    if (num == SIGUSR1)
    {
        paused = true;
    }
    else if (num == SIGUSR2)
    {
        paused = false;
        pthread_cond_signal(&cond);
    }
    else if (num == SIGINT)
    {
        stop = true;
        paused = false;
        pthread_cond_signal(&cond);
    }
    pthread_mutex_unlock(&mutex);
}

void* worker_function (void* arg)
{
    int counter = 1;
    while (!stop)
    {
        pthread_mutex_lock(&mutex);
        while (!stop && paused)
        {
            pthread_cond_wait(&cond, & mutex);
        }
        pthread_mutex_unlock(&mutex);

        if (stop)
        {
            break;
        }
        printf("Cond: %d\n",counter++);
        sleep(1);     
    }
    printf("Worker thread exiting......\n");
    pthread_exit(NULL);
}

int main()
{
    //Signal register
    printf("process ID: %d\n", getpid());
    signal(SIGINT, sig_handler1);
    signal(SIGUSR1, sig_handler1);
    signal(SIGUSR2, sig_handler1);

    // create thread
    if(pthread_create(&worker_thread, NULL, worker_function, NULL) != 0)
    {
        perror("create thread error %d \n");
		exit(1);
    }

    // sleep(5);
    // printf("Main thread: Send signal SIGUSR1 to worker_thread\n");
    // pthread_kill(worker_thread, SIGUSR1);
    
    // sleep(5);
    // printf("Main thread: Send signal SIGUSR2 to worker_thread\n");
    // pthread_kill(worker_thread, SIGUSR2);

    // sleep(5);
    // printf("Main thread: Send signal SIGINT to worker_thread\n");
    // pthread_kill(worker_thread, SIGINT);

    pthread_join(worker_thread, NULL);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    printf("Terminate program ........\n");
    return 0;
}

