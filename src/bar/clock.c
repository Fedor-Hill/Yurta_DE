#include "bar/clock.h"

static gboolean update_clock(gpointer user_data) {
    GtkLabel *label = GTK_LABEL(user_data);
    GDateTime *now = g_date_time_new_now_local();
    
    char *time_str = g_date_time_format(now, "%H\n%M");
    gtk_label_set_text(label, time_str);
    
    g_free(time_str);
    g_date_time_unref(now);
    return G_SOURCE_CONTINUE;
}

void activateClock(GtkLabel *label) {
    update_clock(label);
    g_timeout_add_seconds(1, update_clock, label);
}