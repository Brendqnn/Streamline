#include "slqueue.h"


SLqueue *create_node(const char* value)
{
    SLqueue *queue_node = malloc(sizeof(SLqueue));
    if (queue_node == NULL) {
        perror("Error: Failed to allocate memory for SLqueue.\n");
        return NULL;
    }
    
    queue_node->data = value;
    queue_node->next = NULL;
    return queue_node;
}

void insert_node(SLqueue **head, const char* value)
{
    SLqueue *queue_node = create_node(value);
    queue_node->next = *head;
    *head = queue_node;
}

void display(SLqueue *head)
{
    SLqueue *current = head;
    while (current != NULL) {
        printf("%s -> ", current->data);
        current = current->next;
    }
    printf("NULL\n");
}

void remove_node(SLqueue **head, const char* value)
{
    SLqueue *current = *head;
    SLqueue *prev = NULL;

    while (current != NULL && strcmp(current->data, value) != 0) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Node with value '%s' not found in the list.\n", value);
        return;
    }

    if (prev == NULL) {
        // Node to be removed is the head of the list
        *head = current->next;
    } else {
        prev->next = current->next;
    }

    free(current);
}
