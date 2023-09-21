#ifndef SLWINDOW_H
#define SLWINDOW_H

#include <gtk/gtk.h>
#include "slqueue.h"

typedef struct {
    SLqueue *queue;
    
    GtkWidget *window;
    GtkWidget *event_box;
} SLwindow;

SLwindow *create_window(int height, int width, const char *title, SLqueue *queue);
void show_window(SLwindow *sl_window);
void destroy_window(SLwindow *sl_window);

#endif // SLWINDOW_H

