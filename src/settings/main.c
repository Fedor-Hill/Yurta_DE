#include "settings/main.h"
#include "settings/yurta-wallpaper-page.h"

static void on_row_clicked(AdwActionRow *row, AdwNavigationView *nav_view) {
  adw_navigation_view_push_by_tag(nav_view, "wallpaper_settings");
}

static void activate(GtkApplication *app) {
  g_type_ensure(YURTA_TYPE_WALLPAPER_PAGE);
  GtkBuilder *builder =
      gtk_builder_new_from_resource("/org/yurta/de/ui/settings_de.ui");

  GtkWindow *window = GTK_WINDOW(gtk_builder_get_object(builder, "window"));

  AdwNavigationView *nav_view =
      ADW_NAVIGATION_VIEW(gtk_builder_get_object(builder, "nav_view"));
  AdwActionRow *appearance_row =
      ADW_ACTION_ROW(gtk_builder_get_object(builder, "wallpaper_row"));

  g_signal_connect (appearance_row, "activated", G_CALLBACK (on_row_clicked), nav_view);

  gtk_window_set_application(window, GTK_APPLICATION(app));
  g_object_unref(builder);
  gtk_window_present(window);
}

int main(int argc, char *argv[]) {
  g_autoptr(AdwApplication) app =
      adw_application_new("org.yurta.settings", G_APPLICATION_DEFAULT_FLAGS);

  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

  return g_application_run(G_APPLICATION(app), argc, argv);
}