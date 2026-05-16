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
    t->tid = burst;
    insert(&head, t);
}

void schedule() {
    printf("--- Priority RR Scheduling ---\n");
    
    int task_count = 0;
    struct node *temp = head;
    while(temp) {
        task_count++;
        temp = temp->next;
    }
    
    int current_time = 0;
    double total_turnaround = 0;
    double total_waiting = 0;
    double total_response = 0;
    int *responded = calloc(100, sizeof(int));
    
    while (head != NULL) {
        // Find highest priority
        int highest_pri = -1;
        struct node *curr = head;
        while (curr != NULL) {
            if (curr->task->priority > highest_pri) {
                highest_pri = curr->task->priority;
            }
            curr = curr->next;
        }

        // Round robin for all tasks with highest priority
        curr = head;
        while (curr != NULL) {
            Task *t = curr->task;
            struct node *next_node = curr->next;
            
            if (t->priority == highest_pri) {
                if (responded[t->priority * 100 + t->tid] == 0 && t->burst == t->tid) {
                    total_response += current_time;
                    responded[t->priority * 100 + t->tid] = 1;
                }
                
                int slice = (t->burst > QUANTUM) ? QUANTUM : t->burst;
                run(t, slice);
                current_time += slice;
                t->burst -= slice;

                if (t->burst <= 0) {
                    total_turnaround += current_time;
                    total_waiting += (current_time - t->tid);
                    delete_node(&head, t);
                }
            }
            curr = next_node;
        }
    }
    
    if (task_count > 0) {
        printf("Average Turnaround Time: %.2f\n", total_turnaround / task_count);
        printf("Average Waiting Time: %.2f\n", total_waiting / task_count);
        printf("Average Response Time: %.2f\n", total_response / task_count);
    }
    free(responded);
}
