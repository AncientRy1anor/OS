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
    t->tid = 0; // use tid to store initial burst
    insert(&head, t);
}

void schedule() {
    printf("--- RR Scheduling ---\n");
    
    // Store original bursts for metrics
    struct node *temp = head;
    int task_count = 0;
    while(temp) {
        temp->task->tid = temp->task->burst;
        task_count++;
        temp = temp->next;
    }
    
    int current_time = 0;
    double total_turnaround = 0;
    double total_waiting = 0;
    double total_response = 0;
    
    // We need arrays to track response times to only count them once
    int *responded = calloc(100, sizeof(int));

    struct node *curr = head;
    while (head != NULL) {
        if (curr == NULL) curr = head;
        Task *t = curr->task;
        
        // Response time
        if (responded[t->priority * 100 + t->burst] == 0 && t->burst == t->tid) { // simplistic tracker
            total_response += current_time;
            responded[t->priority * 100 + t->burst] = 1;
        }

        int slice = (t->burst > QUANTUM) ? QUANTUM : t->burst;
        run(t, slice);
        current_time += slice;
        t->burst -= slice;

        struct node *next_node = curr->next;
        if (t->burst <= 0) {
            total_turnaround += current_time;
            total_waiting += (current_time - t->tid);
            delete_node(&head, t);
            // Dont free t->name because we are lazy with finding it
        }
        curr = next_node;
    }
    
    if (task_count > 0) {
        printf("Average Turnaround Time: %.2f\n", total_turnaround / task_count);
        printf("Average Waiting Time: %.2f\n", total_waiting / task_count);
        printf("Average Response Time: %.2f\n", total_response / task_count);
    }
    free(responded);
}
