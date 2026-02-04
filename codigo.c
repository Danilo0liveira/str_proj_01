#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define NUM_MACHINES 4
#define BUFFER_SIZE_MACHINE 1
#define BUFFER_SIZE_OUT 4

#define MACHINE_MAX_TIME 10
#define ROBOT_MAX_TIME 1
#define CONVEYOR_MAX_TIME 5

int buffer_m[NUM_MACHINES];
int buffer_out = 0;

sem_t s, mutex, full;
sem_t m[NUM_MACHINES];
sem_t log_mutex;

void print_buffers() {
    printf("Buffers: ");
    for (int i = 0; i < NUM_MACHINES; i++)
        printf("M%d=%d ", i, buffer_m[i]);
    printf("OUT=%d\n", buffer_out);
}

void *machine(void *arg)
{
    int id = *(int *)arg;
    
    while(1)
    {
        sleep(rand() % MACHINE_MAX_TIME + 1);

        sem_wait(&m[id]);          
        sem_wait(&mutex);         
        buffer_m[id] += 1;

        sem_wait(&log_mutex);
        printf("[MAQ %d] Gerou produto\n", id);
        printf("Buffers: ");
        for (int i = 0; i < NUM_MACHINES; i++)
            printf("M%d=%d ", i, buffer_m[i]);
        printf("OUT=%d\n", buffer_out);
        sem_post(&log_mutex);

        sem_post(&mutex);
        sem_post(&s);             
    }
}


void *robot(void *arg)
{
    while(1)
    {
        sleep(rand() % ROBOT_MAX_TIME + 1);
        sem_wait(&s);   

        sem_wait(&mutex);
        for(int i = 0; i < NUM_MACHINES; i++)
        {
            if (buffer_m[i] > 0)
            {
                buffer_m[i] -= 1;
                sem_post(&m[i]);

                sem_wait(&log_mutex);
                printf("[ROBO] Pegou produto da MAQ %d\n", i);
                printf("Buffers: ");
                for (int j = 0; j < NUM_MACHINES; j++)
                    printf("M%d=%d ", j, buffer_m[j]);
                printf("OUT=%d\n", buffer_out);
                sem_post(&log_mutex);
                break;
            }
        }
        sem_post(&mutex);

        sem_wait(&full);      
        sem_wait(&mutex);
        buffer_out += 1;

        sem_wait(&log_mutex);
        printf("[ROBO] Depositou no BUFFER\n");
        printf("Buffers: ");
        for (int j = 0; j < NUM_MACHINES; j++)
            printf("M%d=%d ", j, buffer_m[j]);
        printf("OUT=%d\n", buffer_out);
        sem_post(&log_mutex);

        sem_post(&mutex);
    }
}


void *conveyor(void *arg)
{
    while(1)
    {
        sleep(rand() % CONVEYOR_MAX_TIME + 1);

        sem_wait(&mutex);
        if (buffer_out > 0)
        {
            buffer_out -= 1;
            sem_post(&full);

            sem_wait(&log_mutex);
            printf("[CONVEYOR] Removeu do BUFFER\n");
            printf("Buffers: ");
            for (int j = 0; j < NUM_MACHINES; j++)
                printf("M%d=%d ", j, buffer_m[j]);
            printf("OUT=%d\n", buffer_out);
            sem_post(&log_mutex);
        }
        sem_post(&mutex);
    }
}


int main()
{
    srand(time(NULL));

    sem_init(&s, 0, 0);
    sem_init(&mutex, 0, 1);
    sem_init(&full, 0, BUFFER_SIZE_OUT);
    sem_init(&log_mutex, 0, 1);

    for (int i = 0; i < NUM_MACHINES; i++)
        sem_init(&m[i], 0, BUFFER_SIZE_MACHINE);

    pthread_t machines_t[NUM_MACHINES], robot_t, conveyor_t;
    int const_ids[NUM_MACHINES];

    for (int i = 0; i < NUM_MACHINES; i++)
    {
        const_ids[i] = i;
        pthread_create(&machines_t[i], NULL, machine, &const_ids[i]);
    }

    pthread_create(&robot_t, NULL, robot, NULL);
    pthread_create(&conveyor_t, NULL, conveyor, NULL);

    for (int i = 0; i < NUM_MACHINES; i++)
        pthread_join(machines_t[i], NULL);

    pthread_join(robot_t, NULL);
    pthread_join(conveyor_t, NULL);

    sem_destroy(&s);
    sem_destroy(&mutex);
    sem_destroy(&full);
}
