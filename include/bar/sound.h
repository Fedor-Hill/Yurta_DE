#ifndef SOUND_H
#define SOUND_H

#include <gtk/gtk.h>
#include <pulse/glib-mainloop.h>
#include <pulse/context.h>
#include <pulse/introspect.h>
#include <pulse/subscribe.h>

// Clean signature targeting your GTK image widget
void activateSound (GtkImage *volume_icon);

#endif