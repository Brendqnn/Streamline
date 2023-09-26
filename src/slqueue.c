#include "slqueue.h"

SLqueue *create_node(const char* data)
{
    SLqueue *new_node = malloc(sizeof(SLqueue));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    new_node->data = malloc(strlen(data) + 1); // +1 for the null terminator
    if (new_node->data == NULL) {
        fprintf(stderr, "Memory allocation failed for data\n");
        exit(1);
    }
    strcpy(new_node->data, data);
    new_node->next = NULL;

    return new_node;
}

void insert_node(SLqueue **head, const char* data)
{
    SLqueue *new_node = create_node(data);
    new_node->next = *head;
    *head = new_node;
}

void remove_node(SLqueue **head)
{
    if (*head != NULL) {
        SLqueue *temp = *head;
        *head = (*head)->next;
        free(temp);
    }
}

void display_list(SLqueue *queue)
{
    SLqueue *current = queue;
    while (current != NULL) {
        printf("%s -> ", current->data);
        current = current->next;
    }
    printf("NULL\n");
}

