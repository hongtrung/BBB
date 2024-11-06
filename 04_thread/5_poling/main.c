#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>

#define THREADHOLD 5
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int counter; //shared variable/ shared resources/ global variable

typedef struct{
	char name[30];
	char msg[30];
}thread_args_t;

static void* handle_th1(void* args)
{
    thread_args_t* thr = (thread_args_t*)args;
    pthread_mutex_lock(&lock);

    //critical section
	printf("hello %s !\n", thr->name);
    while (counter < THREADHOLD)
    {
        counter += 1;
        printf("counter: %d\n", counter);
        sleep(1);
    }
    
    printf("thread 1 handler, counter: %d\n",counter);
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);//exit
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

    while (1)
    {
        if (counter == THREADHOLD)
        {
            printf("Global variable counter = %d.\n",counter);
            break;
        }
        
    }
    

    // used to block for the end of a thread and release
    pthread_join(thread_id1,NULL);
    
    return 0;
}
