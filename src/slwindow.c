#include "slwindow.h"

void on_drag_data_received(GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *data, guint info, guint time, gpointer user_data, SLqueue *queue) {
    GList *uris_list = gtk_selection_data_get_uris(data);

    if (uris_list != NULL && queue != NULL) {
        for (GList *iter = uris_list; iter != NULL; iter = iter->next) {
            const char *uri = iter->data;
            const char *filepath = g_filename_from_uri(uri, NULL, NULL);

            if (filepath != NULL) {
                printf("Dropped file: %s\n", filepath);
                insert_node(&queue, filepath);
            }
        }
        g_list_free(uris_list);
    }
}

SLwindow *create_window(int width, int height, const char *title, SLqueue *queue) {
    SLwindow *sl_window = malloc(sizeof(SLwindow));
    if (sl_window == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    sl_window->queue = queue;

    gtk_init(NULL, NULL);

    sl_window->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(sl_window->window), title);
    gtk_window_set_default_size(GTK_WINDOW(sl_window->window), width, height);
    g_signal_connect(G_OBJECT(sl_window->window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    sl_window->event_box = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(sl_window->window), sl_window->event_box);
    g_signal_connect(sl_window->window, "drag-data-received", G_CALLBACK(on_drag_data_received), queue);
    
    const GtkTargetEntry target_entries[] = {
        {"text/uri-list", 0, 0},
    };

    gtk_drag_dest_set(sl_window->window, GTK_DEST_DEFAULT_ALL, target_entries, G_N_ELEMENTS(target_entries), GDK_ACTION_COPY);
    gtk_widget_set_events(GTK_WIDGET(sl_window->window), GDK_ALL_EVENTS_MASK);
    gtk_widget_show_all(sl_window->window);
    
    return sl_window;
}

void show_window(SLwindow *sl_window) {
    gtk_widget_show_all(sl_window->window);
    gtk_main();
}

void destroy_window(SLwindow *sl_window) {
    gtk_widget_destroy(sl_window->window);
    free(sl_window);
}



