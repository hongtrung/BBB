#include<semaphore.h>
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<signal.h>
#include<sys/mman.h>

#define BUFFER_SIZE 10
#define Fettuccine_Chicken_Alfredo 11
#define Garlic_Sirloin_Steak 12
#define Pistachio_Pesto_Pasta 21
#define Avocado_Fruit_Salad 22

void write_to_buffer(int* buffer, int buffer_size, int data);
int read_from_buffer(int* buffer, int buffer_size);
void clean_up(void);
void handle_signal(int sig);
void producer_Vegan_Process(void);
void producer_Non_Vegan_Process(void);
void consumer_Vegan_Process(void);
void consumer_Non_Vegan_Process(void);
void consumer_Both_Process(void);
void wait_for_children(int num_children);

volatile sig_atomic_t interrupted = 0;

int* buffer_Vegan;
int* buffer_Non_Vegan;

sem_t* mutex_Vegan;
sem_t* mutex_Non_Vegan;
sem_t* empty_Vegan;
sem_t* empty_Non_Vegan;
sem_t* full_Vegan;
sem_t* full_Non_Vegan;

// fork()
pid_t producer_Vegan;
pid_t producer_Non_Vegan;
pid_t consumer_Vegan;
pid_t consumer_Non_Vegan;
pid_t consumer_Both;

int main()
{
    // dang ky signal
    signal(SIGINT, handle_signal);

    // create memory
    buffer_Vegan = (int*)mmap(NULL, sizeof(int)* BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (buffer_Vegan == MAP_FAILED) 
    {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }
    buffer_Non_Vegan = (int*)mmap(NULL, sizeof(int)* BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (buffer_Non_Vegan == MAP_FAILED) 
    {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    // Create shared semaphore
    // mutex
    mutex_Vegan = (sem_t*)mmap(NULL, sizeof(sem_t*), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (mutex_Vegan == MAP_FAILED)
    {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }
    sem_init(mutex_Vegan, 1, 1);
    mutex_Non_Vegan = (sem_t*)mmap(NULL, sizeof(sem_t*), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (mutex_Non_Vegan == MAP_FAILED)
    {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }
    sem_init(mutex_Non_Vegan, 1, 1);
    // full
    full_Vegan = (sem_t*)mmap(NULL, sizeof(sem_t*), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (full_Vegan == MAP_FAILED)
    {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }
    sem_init(full_Vegan, 1, 0);
    full_Non_Vegan = (sem_t*)mmap(NULL, sizeof(sem_t*), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (full_Non_Vegan == MAP_FAILED)
    {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }
    sem_init(full_Non_Vegan, 1, 0);
    // empty
    empty_Vegan = (sem_t*)mmap(NULL, sizeof(sem_t*), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (empty_Vegan == MAP_FAILED)
    {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }
    sem_init(empty_Vegan, 1, BUFFER_SIZE);
    empty_Non_Vegan = (sem_t*)mmap(NULL, sizeof(sem_t*), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (empty_Non_Vegan == MAP_FAILED)
    {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }
    sem_init(empty_Non_Vegan, 1, BUFFER_SIZE);

    // Child producer vegan process
    producer_Vegan_Process();
    // child producer non vegan process
    producer_Non_Vegan_Process();
    // child consumer vegan process
    consumer_Vegan_Process();
    // child consumer non vegan process
    consumer_Non_Vegan_Process();
    // child consumer both process
    consumer_Both_Process();

    // parent
    while (!interrupted)
    {
        sleep(10);
        int takenSlots_Vegan;
        int takenSlots_Non_Vegan;
        sem_getvalue(full_Vegan, &takenSlots_Vegan);
        sem_getvalue(full_Non_Vegan, &takenSlots_Non_Vegan);
        printf("\nItems in Vegan dish: %d/%d    Items in NON_Vegane dish: %d/%d\n", takenSlots_Vegan, BUFFER_SIZE, takenSlots_Non_Vegan, BUFFER_SIZE);
    }

    // Kết thúc các tiến trình con
    kill(producer_Vegan, SIGTERM);
    kill(producer_Non_Vegan, SIGTERM);
    kill(consumer_Vegan, SIGTERM);
    kill(consumer_Non_Vegan, SIGTERM);
    kill(consumer_Both, SIGTERM);

    // Đợi các tiến trình con kết thúc
    wait_for_children(5);

    clean_up();
    return 0;
}

void handle_signal(int sig)
{
    interrupted = 1;
}

void wait_for_children(int num_children)
{
    for (int i = 0; i < num_children; i++)
    {
        wait(NULL);
    }
    
}

void write_to_buffer(int* buffer, int buffer_size, int data)
{
    for (int i = 0; i < buffer_size; i++)
    {
        if (buffer[i] == 0)
        {
            buffer[i] = data;
            break;
        }
        
    }
    
}

int read_from_buffer(int* buffer, int buffer_size)
{
    for (int i = 0; i < buffer_size; i++)
    {
        if (buffer[i] != 0)
        {
            int value = buffer[i];
            buffer[i] = 0;
            return value;
        }
        
    }
    return 0;
}

void producer_Vegan_Process(void)
{
    if ((producer_Vegan = fork()) ==0)
    {
        srand(time(NULL));
        unsigned int seed = rand();
        while (!interrupted)
        {
            sem_wait(empty_Vegan);
            sem_wait(mutex_Vegan);

            // create meal vegan
            int random_Meal_Vegan = rand_r(&seed);
            if (random_Meal_Vegan % 2 == 0)
            {
                write_to_buffer(buffer_Vegan, BUFFER_SIZE, Fettuccine_Chicken_Alfredo);
                printf("Producer Vegan creating Fettuccine Chicken Alfredo\n");
            }
            else
            {
                write_to_buffer(buffer_Vegan, BUFFER_SIZE, Garlic_Sirloin_Steak);
                printf("Producer creating Garlic_Sirloin_Steak\n");
            }
            sem_post(full_Vegan);
            sem_post(mutex_Vegan);

            // sleep between 1 & 5 seconds    
            sleep(1 + rand_r(&seed) % 5);
        }
        printf("Child process ending...\n");
        exit(0);
    }
}

void producer_Non_Vegan_Process(void)
{
    if ((producer_Non_Vegan = fork()) ==0)
    {
        srand(time(NULL));
        unsigned int seed = rand();
        while (!interrupted)
        {
            sem_wait(empty_Vegan);
            sem_wait(mutex_Vegan);

            // create non meal vegan
            int random_Non_Meal_Vegan = rand_r(&seed);
            if (random_Non_Meal_Vegan % 2 == 0)
            {
                write_to_buffer(buffer_Non_Vegan, BUFFER_SIZE, Pistachio_Pesto_Pasta);
                printf("Producer Non Vegan creating Pistachio_Pesto_Pasta\n");
            }
            else
            {
                write_to_buffer(buffer_Non_Vegan, BUFFER_SIZE, Avocado_Fruit_Salad);
                printf("Producer Non Vegan creating Avocado_Fruit_Salad\n");
            }
            sem_post(full_Vegan);
            sem_post(mutex_Vegan);

            // sleep between 1 & 5 seconds    
            sleep(1 + rand_r(&seed) % 5);
        }
        printf("Child process ending...\n");
        exit(0);
    }
}

void consumer_Non_Vegan_Process(void)
{
    if ((consumer_Non_Vegan = fork()) == 0)
    {
        srand(time(NULL));
        unsigned int seed = rand();
        while (!interrupted)
        {
            sem_wait(mutex_Non_Vegan);
            sem_wait(full_Non_Vegan);

            // take meal Non vegan from buffer non vegan
            int read_Non_Vegan = read_from_buffer(buffer_Non_Vegan, BUFFER_SIZE);
            if (read_Non_Vegan == Pistachio_Pesto_Pasta)
            {
                printf("Consumer_Non_Vegan take Pistachio_Pesto_Pasta\n");
            }
            else if (read_Non_Vegan == Avocado_Fruit_Salad)
            {
                printf("Consumer_Non_Vegan take Avocado_Fruit_Salad\n");
            }
            
            sem_post(empty_Non_Vegan);
            sem_post(mutex_Non_Vegan);

            // Sleep between 10 & 15 seconds
            sleep(10 + rand_r(&seed) % 6);
        }
        printf("Child process ending...\n");
        exit(0);
    }
}

void consumer_Vegan_Process(void)
{
    if ((consumer_Vegan = fork()) == 0)
    {
        srand(time(NULL));
        unsigned int seed = rand();
        while (!interrupted)
        {
            sem_wait(mutex_Vegan);
            sem_wait(full_Vegan);

            // take meal vegan from buffer non vegan
            int read_Vegan = read_from_buffer(buffer_Vegan, BUFFER_SIZE);
            if (read_Vegan == Fettuccine_Chicken_Alfredo)
            {
                printf("Consumer_Vegan take Fettuccine_Chicken_Alfredo\n");
            }
            else if (read_Vegan == Garlic_Sirloin_Steak)
            {
                printf("Consumer_Vegan take Garlic_Sirloin_Steak\n");
            }
            
            sem_post(empty_Vegan);
            sem_post(mutex_Vegan);

            // Sleep between 10 & 15 seconds
            sleep(10 + rand_r(&seed) % 6);
        }
        printf("Child process ending...\n");
        exit(0);
    }
}


void consumer_Both_Process(void)
{
    if ((consumer_Both = fork()) == 0)
    {
        srand(time(NULL));
        unsigned int seed = rand();
        while (!interrupted)
        {
            struct timespec timeout;
            clock_gettime(CLOCK_REALTIME, &timeout);
            timeout.tv_sec += 3;

            int result_Vegan = sem_timedwait(full_Vegan, &timeout);
            if (result_Vegan == 0)
            {
                sem_wait(mutex_Vegan);

                /* Take meal from buffer Vegan */
                int read_Vegan = read_from_buffer(buffer_Vegan, BUFFER_SIZE);
                if (read_Vegan == Fettuccine_Chicken_Alfredo)
                {
                    printf("consumer_Both take Fettuccine_Chicken_Alfredo\n");
                }
                else if (read_Vegan == Garlic_Sirloin_Steak)
                {
                    printf("consumer_Both take Garlic_Sirloin_Steak\n");
                }

                sem_post(mutex_Vegan);
                sem_post(empty_Vegan);
            }
            else
            {
                clock_gettime(CLOCK_REALTIME, &timeout);
                timeout.tv_sec += 3;

                int result_Non_Vegan = sem_timedwait(full_Non_Vegan, &timeout);
                if (result_Non_Vegan == 0)
                {
                    sem_wait(mutex_Non_Vegan);

                    /* Take meal from buffer _NON_Vegan */
                    int read_Non_Vegan = read_from_buffer(buffer_Non_Vegan, BUFFER_SIZE);
                    if (read_Non_Vegan == Pistachio_Pesto_Pasta)
                    {
                        printf("consumer_Both take Pistachio_Pesto_Pasta\n");
                    }
                    else if (read_Non_Vegan == Avocado_Fruit_Salad)
                    {
                        printf("consumer_Both take Avocado_Fruit_Salad\n");
                    }

                    sem_post(mutex_Non_Vegan);
                    sem_post(empty_Non_Vegan);
                }
            }

            /* Sleep between 10 and 15 seconds */
            sleep(10 + rand_r(&seed) % 6);
        }
        printf("Child process ending...\n");
        exit(0);
    }
}

void clean_up(void)
{
    // Giải phóng shared memory
    munmap(buffer_Vegan, sizeof(int) * BUFFER_SIZE);
    munmap(buffer_Non_Vegan, sizeof(int) * BUFFER_SIZE);

    // Giải phóng semaphore cho mutex và empty/full của các bộ đệm Vegan và NON_Vegan
    munmap(mutex_Vegan, sizeof(sem_t *));
    munmap(empty_Vegan, sizeof(sem_t *));
    munmap(full_Vegan, sizeof(sem_t *));

    munmap(mutex_Non_Vegan, sizeof(sem_t *));
    munmap(empty_Non_Vegan, sizeof(sem_t *));
    munmap(full_Non_Vegan, sizeof(sem_t *));
}