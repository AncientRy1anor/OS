#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "task.h"
#include "list.h"
#include "cpu.h"

struct node *head = NULL;

void add(char *name, int priority, int burst) {
    Task *t = malloc(sizeof(Task));
    t->name = strdup(name);
    t->priority = priority;
    t->burst = burst;
    t->tid = 0;
    insert(&head, t);
}

Task* pickNextTask() {
    if (head == NULL) return NULL;
    struct node *temp = head;
    Task *highest = temp->task;
    while (temp != NULL) {
        if (temp->task->priority > highest->priority) {
            highest = temp->task;
        }
        temp = temp->next;
    }
    return highest;
}

void schedule() {
    printf("--- Priority Scheduling ---\n");
    Task *t;
    int current_time = 0;
    double total_turnaround = 0;
    double total_waiting = 0;
    double total_response = 0;
    int task_count = 0;

    while ((t = pickNextTask()) != NULL) {
        int response_time = current_time;
        int wait_time = current_time;
        
        run(t, t->burst);
        
        current_time += t->burst;
        int turnaround_time = current_time;

        total_response += response_time;
        total_waiting += wait_time;
        total_turnaround += turnaround_time;
        task_count++;
        
        delete_node(&head, t);
        free(t->name);
        free(t);
    }
    
    if (task_count > 0) {
        printf("Average Turnaround Time: %.2f\n", total_turnaround / task_count);
        printf("Average Waiting Time: %.2f\n", total_waiting / task_count);
        printf("Average Response Time: %.2f\n", total_response / task_count);
    }
}
