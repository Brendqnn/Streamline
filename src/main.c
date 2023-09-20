#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include "../include/util.h"
#include "slio.h"
#include "slcodec.h"
#include "slcompressor.h"
#include "slqueue.h"
#include <gtk/gtk.h>


static void button_clicked(GtkWidget *widget, gpointer user_data)
{
    printf("Button clicked!\n");
}

static void on_drag_data_received(GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *data,
                                  guint info, guint time, gpointer user_data)
{
    GList *uris_list = gtk_selection_data_get_uris(data);

    if (uris_list != NULL) {
        for (GList *iter = uris_list; iter != NULL; iter = iter->next) {
            const char *uri = iter->data;
            const char *filepath = g_filename_from_uri(uri, NULL, NULL);

            if (filepath != NULL) {
                printf("Dropped file: %s\n", filepath);
                
            }
        }
        g_list_free(uris_list);
    }
}

static void activate(GtkApplication *app, gpointer user_data)
{
    SLqueue *queue = NULL;

    GtkWidget *window;
    GtkWidget *main_box;
    GtkWidget *button;
    GtkWidget *list_box;

    const char *a_file = "whats gravy.mp4";

    insert_node(&queue, "hello");
    display_list(queue);
    
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Streamline-v1.0");
    gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_HEIGHT, WINDOW_WIDTH);

    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    button = gtk_button_new_with_label("Start");
    g_signal_connect(button, "clicked", G_CALLBACK(button_clicked), NULL);
    gtk_widget_set_size_request(button, 100, 40);
    gtk_box_pack_start(GTK_BOX(main_box), button, FALSE, FALSE, 0);

    list_box = gtk_list_box_new();
    gtk_box_pack_start(GTK_BOX(main_box), list_box, TRUE, TRUE, 0);

    g_signal_connect(G_OBJECT(window), "drag-data-received", G_CALLBACK(on_drag_data_received), NULL);

    const GtkTargetEntry target_entries[] = {
        {"text/uri-list", 0, 0},
    };

    gtk_drag_dest_set(window, GTK_DEST_DEFAULT_ALL, target_entries, G_N_ELEMENTS(target_entries), GDK_ACTION_COPY);

    gtk_widget_set_events(GTK_WIDGET(window), GDK_ALL_EVENTS_MASK);

    gtk_widget_show_all(window);
}

int main(int argc, char *argv[]) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.example.myapp", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}







