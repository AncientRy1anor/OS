#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"
#include "task.h"

void insert(struct node **head, Task *newTask) {
    struct node *newNode = malloc(sizeof(struct node));
    newNode->task = newTask;
    newNode->next = NULL;
    
    if (*head == NULL) {
        *head = newNode;
        return;
    }
    struct node *temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newNode;
}

void delete_node(struct node **head, Task *task) {
    struct node *temp;
    struct node *prev;

    temp = *head;
    if (temp != NULL && strcmp(task->name, temp->task->name) == 0) {
        *head = temp->next;
        free(temp);
        return;
    }
    prev = *head;
    if (temp != NULL) temp = temp->next;
    while (temp != NULL && strcmp(task->name, temp->task->name) != 0) {
        prev = temp;
        temp = temp->next;
    }
    if (temp != NULL) {
        prev->next = temp->next;
        free(temp);
    }
}

void traverse(struct node *head) {
    struct node *temp;
    temp = head;

    while (temp != NULL) {
        printf("[%s] [%d] [%d]\n",temp->task->name, temp->task->priority, temp->task->burst);
        temp = temp->next;
    }
}
