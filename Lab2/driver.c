#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "schedulers.h"

#define SIZE    100

void add(char *name, int priority, int burst);
void schedule();

int main(int argc, char *argv[])
{
    FILE *in;
    char task[SIZE];
    
    char *name;
    int priority;
    int burst;

    if (argc != 2) {
        printf("Usage: %s <schedule.txt>\n", argv[0]);
        return -1;
    }

    in = fopen(argv[1], "r");
    if (!in) {
        printf("Error opening file\n");
        return -1;
    }
    
    while (fgets(task, SIZE, in) != NULL) {
        name = strtok(task, ",\n");
        if (name) {
            char *p = strtok(NULL, ",\n");
            char *b = strtok(NULL, ",\n");
            if (p && b) {
                priority = atoi(p);
                burst = atoi(b);
                add(name, priority, burst);
            }
        }
    }
    
    fclose(in);
    schedule();

    return 0;
}
