/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 5

#define TRUE 1

// this represents work that has to be 
// completed by a thread in the pool
typedef struct 
{
    void (*function)(void *p);
    void *data;
}
task;

// the work queue
task queue[QUEUE_SIZE];
int count = 0;
int head = 0;
int tail = 0;

// synchronization primitives
pthread_mutex_t mutex;
sem_t sem;

// the worker bees
pthread_t bees[NUMBER_OF_THREADS];

// insert a task into the queue
// returns 0 if successful or 1 otherwise
int enqueue(task t) 
{
    pthread_mutex_lock(&mutex);
    if (count == QUEUE_SIZE) {
        pthread_mutex_unlock(&mutex);
        return 1; // queue full
    }
    
    queue[tail] = t;
    tail = (tail + 1) % QUEUE_SIZE;
    count++;
    
    pthread_mutex_unlock(&mutex);
    sem_post(&sem); // notify worker
    
    return 0;
}

// remove a task from the queue
task dequeue() 
{
    task worktodo = queue[head];
    head = (head + 1) % QUEUE_SIZE;
    count--;
    return worktodo;
}

// the worker thread in the thread pool
void *worker(void *param)
{
    while (TRUE) {
        // Wait for work
        sem_wait(&sem);
        
        pthread_mutex_lock(&mutex);
        task worktodo = dequeue();
        pthread_mutex_unlock(&mutex);
        
        // execute the task
        execute(worktodo.function, worktodo.data);
    }

    pthread_exit(0);
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{
    task t;
    t.function = somefunction;
    t.data = p;

    return enqueue(t);
}

// initialize the thread pool
void pool_init(void)
{
    pthread_mutex_init(&mutex, NULL);
    sem_init(&sem, 0, 0); // 0 means shared between threads, initial value 0
    
    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_create(&bees[i], NULL, worker, NULL);
    }
}

// shutdown the thread pool
void pool_shutdown(void)
{
    // cancel all threads
    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_cancel(bees[i]);
    }
    
    // wait for all threads to terminate
    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_join(bees[i], NULL);
    }
    
    pthread_mutex_destroy(&mutex);
    sem_destroy(&sem);
}
