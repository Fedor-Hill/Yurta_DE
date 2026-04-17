#include "gtk4-layer-shell.h"
#include <dbus/dbus.h>

#include "bar/bar.h"
#include "bar/battery.h"
#include "bar/clock.h"

GtkBuilder *builder;

//
// AAAAAACTIVATE
//
void activate(GtkApplication *app, void *_data) {
  (void)_data;

  builder = gtk_builder_new();
  gtk_builder_add_from_file(
      builder, "/home/fedor/Developments/yurta-de/ui/bar.ui", NULL);

  GtkWindow *bar = GTK_WINDOW(gtk_builder_get_object(builder, "bar"));
  gtk_window_set_application(bar, app);

  GtkLabel *clock_label =
      GTK_LABEL(gtk_builder_get_object(builder, "clock_Label"));

  GtkImage *batt_icon = GTK_IMAGE(gtk_builder_get_object(builder, "batt"));
  gtk_image_set_pixel_size(batt_icon, 24);

  //
  // Bar
  //
  gtk_layer_init_for_window(bar);
  gtk_layer_set_layer(bar, GTK_LAYER_SHELL_LAYER_TOP);
  gtk_layer_auto_exclusive_zone_enable(bar);

  gtk_layer_set_margin(bar, GTK_LAYER_SHELL_EDGE_LEFT, 0);
  gtk_layer_set_margin(bar, GTK_LAYER_SHELL_EDGE_RIGHT, 0);
  gtk_layer_set_margin(bar, GTK_LAYER_SHELL_EDGE_TOP, 0);
  gtk_layer_set_margin(bar, GTK_LAYER_SHELL_EDGE_BOTTOM, 0);

  gtk_layer_set_anchor(bar, GTK_LAYER_SHELL_EDGE_RIGHT, TRUE);
  gtk_layer_set_anchor(bar, GTK_LAYER_SHELL_EDGE_TOP, TRUE);
  gtk_layer_set_anchor(bar, GTK_LAYER_SHELL_EDGE_BOTTOM, TRUE);

  //
  // Activating Func
  //

  activateClock(clock_label);
  activateBattery(batt_icon);

  gtk_window_present(GTK_WINDOW(bar));
}