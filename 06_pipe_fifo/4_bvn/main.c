#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<pthread.h>
#include <sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>

#define FIFO_FILE "./myFifo"

#define message "hello child, how are you?"
int fd;
pthread_t child_read_thread, parent_write_thread;
pid_t child_pid;
// ham xu ly process parent
void parent_sig_handle1(int num)
{
    if(num == SIGUSR1)
    {
        printf("Parent received SIGINT from child\n");
    }
}

// ham xu ly process child
// void child_sig_handle1(int num)
// {
//     if(num == SIGUSR1)
//     {
//         printf("Child received SIGINT from parent. Starting to read from FIFO\n");
//     }
// }

// parent write in fifo 
void* worker_parent(void* arg)
{
    fd = open(FIFO_FILE, O_WRONLY);
    if (fd == -1)
    {
        perror("Failed to open FIFO for writing\n");
        pthread_exit(NULL);
    }

    write(fd, message, strlen(message) +1);
    printf("Parent wrote message to FIFO\n");
    sleep(1);
    close(fd);
    kill(child_pid, SIGUSR1);
    pthread_exit(NULL);
}

// child read in fifo
void* worker_child(void* arg)
{
    
    fd = open(FIFO_FILE, O_RDONLY);
    if (fd == -1)
    {
        perror("Failed to open FIFO for reading\n");
        pthread_exit(NULL);
    }
    char buff[100];
   
    read(fd, buff, sizeof(buff));
    printf("Child received SIGINT from parent. Starting to read from FIFO\n");
    printf("Child read message from FIFO: %s\n", buff);
    sleep(1);
    close(fd);
    kill(getppid(), SIGUSR1);
    pthread_exit(NULL);
}

int main()
{
    int ret;
    
    child_pid = fork();
    if (child_pid < 0)
    {
        perror("Fork() failed\n");
        exit(EXIT_FAILURE);
    }
    else if (child_pid == 0)     // child
    {
        
        //signal(SIGUSR1, child_sig_handle1); 
        //create child read thread
        ret = pthread_create(&child_read_thread,NULL, worker_child, NULL);
        if(ret != 0)
        {
            perror("create thread error\n");
            exit(EXIT_FAILURE);
        }
        sleep(1);
        // pthread_kill(child_read_thread, SIGUSR1);
        pthread_join(child_read_thread, NULL);
        exit(EXIT_SUCCESS);
        
    }
    else                   // parent
    {
        signal(SIGUSR1, parent_sig_handle1);
        // process parent create fifo
        mkfifo(FIFO_FILE, 0666);
        if ( mkfifo < 0)
        {
            perror("Mkfifo failed\n");
            exit(EXIT_FAILURE);
        }
        printf("Parent created FIFO\n");
        sleep(1);
        ret = pthread_create(&parent_write_thread, NULL, worker_parent, NULL);
        if (ret != 0)
        {
            perror("create thread error\n");
            exit(EXIT_FAILURE);
        }

        pthread_join(parent_write_thread, NULL);  
        
    }
    return 0;
}
