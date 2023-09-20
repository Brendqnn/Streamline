#ifndef SLQUEUE_H
#define SLQUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct SLqueue {
    const char *data;
    struct SLqueue *next;
} SLqueue;

SLqueue *create_node(const char *data);
void insert_node(SLqueue **head, const char *data);
void remove_node(SLqueue **head, const char *data);
void display_list(SLqueue *queue);

#endif // SLQUEUE_H



