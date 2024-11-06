#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
int counter = 2; //shared variable/ shared resources/ global variable

typedef struct{
	char name[30];
	char msg[30];
}thread_args_t;

static void* handle_th1(void* args)
{
	thread_args_t* thr = (thread_args_t*)args;
	// sleep(1);
    pthread_mutex_lock(&lock1);
    //critical section
	printf("hello %s !\n", thr->name);
	printf("thread 1 handler, counter: %d\n",++counter);
    sleep(5);
	//dead lock
    //pthread_mutex_lock(&lock2);
    //printf("thread1<<<\n");

    pthread_mutex_unlock(&lock1);

    pthread_exit(NULL);//exit
}
static void* handle_th2(void* args)
{
	// sleep(5);
	// atomic
    pthread_mutex_lock(&lock1);
    //critical section
	printf("thread 1 handler, counter: %d\n",++counter);
	// pthread_mutex_lock(&lock1);
    pthread_mutex_lock(&lock1);
    //printf("thread2 <<<\n");
	//atomic
	pthread_exit(NULL); //exit
}

int main(int argc, char const* argv[])
{
    //code
    int ret;
    thread_args_t thr;
    pthread_t thread_id1, thread_id2;

    memset(&thr, 0x0, sizeof(thread_args_t));
    strncpy(thr.name, "trungkc", sizeof(thr.name));
    if(ret = pthread_create(&thread_id1, NULL, &handle_th1, &thr))
    {
        printf("pthread_create() error number = %d \n",ret);
		return -1;
    }

    if (ret = pthread_create(&thread_id2, NULL, &handle_th2, NULL))
	{
		printf("pthread_create() error number = %d\n", ret);
		return -1;
	}

    // used to block for the end of a thread and release
    pthread_join(thread_id1,NULL);
    pthread_join(thread_id2,NULL);

    return 0;
}
