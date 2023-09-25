#ifndef SLWINDOW_H
#define SLWINDOW_H

#include <gtk/gtk.h>
#include "slqueue.h"
#include "slio.h"
#include "slcompressor.h"

typedef struct {
    GtkWidget *window;
    GtkWidget *event_box;
    GtkWidget *button;

    SLio *io;
    SLcompressor *compressor;
} SLwindow;

SLwindow *create_window(int height, int width, const char *title, SLio *io);
void show_window(SLwindow *sl_window);
void destroy_window(SLwindow *sl_window);

#endif // SLWINDOW_H
