#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_MACHINES 2
#define BUFFER_SIZE_MACHINE 2
#define BUFFER_SIZE_OUT 2

int buffer_m[2];
int buffer_out = 0;

sem_t s, mutex, full;
sem_t m[NUM_MACHINES];

void *machine(void *arg)
{
    int id = *(int *)arg;
    while(1)
    {
        sleep(rand() % 3 + 1);
        buffer_m[id]+=1;
        sem_post(&s);
        sem_wait(&m[id]);
    }
}

void *robot(void *arg)
{
    while(1)
    {
        sleep(rand() % 3 + 1);
        sem_wait(&s);

        for(int i = 0; i<NUM_MACHINES; i++)
        {
            if (buffer_m[i])
            {
                buffer_m[i]-=1;
                sem_post(&m[i]);
                break;
            }
        }
        sem_wait(&full);
        sem_wait(&mutex);
        buffer_out+=1;
        sem_post(&mutex);
    }
}

void *conveyor(void *arg)
{
    while(1)
    {
        sleep(rand() % 3 + 1);
        sem_wait(&mutex);
        if (buffer_out>0)
            buffer_out-=1;
            sem_post(&full);
        sem_post(&mutex);
    }
}

int main()
{
    sem_init(&s, 0, NUM_MACHINES-1);
    sem_init(&mutex, 0, 0);
    sem_init(&full, 0, BUFFER_SIZE_OUT-1);

    for (int i = 0; i < NUM_MACHINES; i++){
        sem_init(&m[i], 0, BUFFER_SIZE_MACHINE);
    }
}