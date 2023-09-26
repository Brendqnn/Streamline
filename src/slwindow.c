#include "slwindow.h"

void on_button_clicked(GtkWidget *button, gpointer user_data)
{
    SLwindow *sl_window = (SLwindow*)user_data;
    SLcodec *codec = init_codec(sl_window->io);
    SLcompressor *compressor = init_compressor(sl_window->io, codec);
    
    compressor_setup(compressor, codec, sl_window->io);
    compress(compressor, codec, sl_window->io);
    free_compressor(compressor, codec, sl_window->io);
}

void on_drag_data_received(GtkWidget *widget, GdkDragContext *context, gint x, gint y,
                           GtkSelectionData *data, guint info, guint time, gpointer user_data)
{
    SLwindow *sl_window = (SLwindow*)user_data; // Cast user_data to SLwindow pointer
    GList *uris_list = (GList*)gtk_selection_data_get_uris(data);
    
    if (uris_list != NULL) {
        for (GList *iter = uris_list; iter != NULL; iter = iter->next) {
            const char *uri = iter->data;
            const char *filepath = g_filename_from_uri(uri, NULL, NULL);

            if (filepath != NULL) {
                printf("Dropped file: %s\n", filepath);
                insert_node(&sl_window->io->queue, filepath);
            }
        }
        g_list_free(uris_list);
    }
}

SLwindow *create_window(int width, int height, const char *title, SLio *io) {
    SLwindow *sl_window = malloc(sizeof(SLwindow));
    if (sl_window == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
     }
    
    sl_window->io = io;
    
    sl_window->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(sl_window->window), title);
    gtk_window_set_default_size(GTK_WINDOW(sl_window->window), width, height);
    g_signal_connect(G_OBJECT(sl_window->window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    sl_window->event_box = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(sl_window->window), sl_window->event_box);
    g_signal_connect(sl_window->window, "drag-data-received", G_CALLBACK(on_drag_data_received), sl_window);
    
    const GtkTargetEntry target_entries[] = {
        {"text/uri-list", 0, 0},
    };

    sl_window->button = gtk_button_new_with_label("Start");
    g_signal_connect(sl_window->button, "clicked", G_CALLBACK(on_button_clicked), sl_window);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(box), sl_window->button, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(sl_window->event_box), box);
    
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



