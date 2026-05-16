#include "task.h"

struct node {
    Task *task;
    struct node *next;
};

// insert and delete operations.
void insert(struct node **head, Task *task);
void delete_node(struct node **head, Task *task);
void traverse(struct node *head);
