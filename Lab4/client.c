
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#define sleep(x) Sleep(1000 * (x))
#else
#include <unistd.h>
#endif
#include "threadpool.h"

struct data
{
    int a;
    int b;
};

void add(void *param)
{
    struct data *temp;
    temp = (struct data*)param;

    printf("I add two values %d and %d result = %d\n", temp->a, temp->b, temp->a + temp->b);
}

int main(void)
{
    // initialize the thread pool
    pool_init();

    // create some work to do
    struct data work1 = {5, 10};
    struct data work2 = {15, 25};
    struct data work3 = {100, 200};
    struct data work4 = {200, 200};
    struct data work5 = {200, 2000};


    // submit the work to the queue
    pool_submit(&add, &work1);
    pool_submit(&add, &work2);
    pool_submit(&add, &work3);
    pool_submit(&add, &work4);
    pool_submit(&add, &work5);

    // give the worker threads time to finish the tasks before shutdown
    sleep(3);

    pool_shutdown();

    return 0;
}
