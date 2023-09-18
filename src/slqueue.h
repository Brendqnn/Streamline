#ifndef SLQUEUE_H
#define SLQUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct SLqueue {
    const char *data;
    struct SLqueue *next;
} SLqueue;

SLqueue *create_node(const char* value);
void insert_node(SLqueue **head, const char* value);
void display(SLqueue *head);
void remove_node(SLqueue **head, const char* value);

#endif // SLQUEUE_H

