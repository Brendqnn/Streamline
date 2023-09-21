#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "../include/util.h"
#include "slwindow.h"
#include "slqueue.h"



int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    SLqueue *queue = NULL;
    SLwindow *window = create_window(WINDOW_HEIGHT, WINDOW_WIDTH, "Streamline-v1.0", queue);
    show_window(window);

    return 0;
}







