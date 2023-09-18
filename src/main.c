#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

#include "../include/util.h"
#include "slio.h"
#include "slcodec.h"
#include "slcompressor.h"
#include "slqueue.h"

#include <gtk/gtk.h>

static void on_drag_data_received(GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *data, guint info, guint time, gpointer user_data) {
    GList *uris_list = gtk_selection_data_get_uris(data);

    if (uris_list != NULL) {
        for (GList *iter = uris_list; iter != NULL; iter = iter->next) {
            gchar *uri = (gchar *)iter->data;
            gchar *filename = g_filename_from_uri(uri, NULL, NULL);

            if (filename != NULL) {
                g_print("Dropped file: %s\n", filename);

                // use SLqueue here to insert
                
                g_free(filename);
            }
        }

        g_list_free(uris_list);
    }
}

void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Streamline-v1.0");
    gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_HEIGHT, WINDOW_WIDTH);

    GtkFixed *fixed_container = GTK_FIXED(gtk_fixed_new());
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(fixed_container));

    g_signal_connect(G_OBJECT(fixed_container), "drag-data-received", G_CALLBACK(on_drag_data_received), NULL);

    const GtkTargetEntry target_entries[] = {
        {"text/uri-list", 0, 0},
    };

    gtk_drag_dest_set(fixed_container, GTK_DEST_DEFAULT_ALL, target_entries, G_N_ELEMENTS(target_entries), GDK_ACTION_COPY);

    gtk_widget_set_events(GTK_WIDGET(fixed_container), GDK_ALL_EVENTS_MASK);

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




