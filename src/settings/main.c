#include "settings/main.h"

static void activate(GtkApplication *app) {
  GtkWidget *window = adw_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Yurta Settings");
  gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

  GtkWidget *header = adw_header_bar_new();

  GtkWidget *page = adw_preferences_page_new();
  adw_preferences_page_set_title(ADW_PREFERENCES_PAGE(page), "General");

  GtkWidget *group = adw_preferences_group_new();
  adw_preferences_group_set_title(ADW_PREFERENCES_GROUP(group), "Appearance");

  GtkWidget *switch_row = adw_switch_row_new();
  adw_preferences_row_set_title(ADW_PREFERENCES_ROW(switch_row), "Dark Mode");
  adw_preferences_group_add(ADW_PREFERENCES_GROUP(group), switch_row);

  adw_preferences_page_add(ADW_PREFERENCES_PAGE(page),
                           ADW_PREFERENCES_GROUP(group));

  GtkWidget *content = adw_toolbar_view_new();
  adw_toolbar_view_add_top_bar(ADW_TOOLBAR_VIEW(content), header);
  adw_toolbar_view_set_content(ADW_TOOLBAR_VIEW(content), page);

  adw_application_window_set_content(ADW_APPLICATION_WINDOW(window), content);

  gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[]) {
  g_autoptr(AdwApplication) app =
      adw_application_new("org.yurta.settings", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  return g_application_run(G_APPLICATION(app), argc, argv);
}