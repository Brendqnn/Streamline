#include "slqueue.h"

void insert_node(SLqueue **head, const char *data)
{
    SLqueue *node = malloc(sizeof(SLqueue));

    if (node == NULL) {
        fprintf(stderr, "Memory allocation for SLqueue has failed.\n");
        exit(EXIT_FAILURE);
    }
    node->data = strdup(data);
    node->next = NULL;
    
    if (*head == NULL) {
        *head = node;
    } else {
        SLqueue *temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = node;
    }
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




