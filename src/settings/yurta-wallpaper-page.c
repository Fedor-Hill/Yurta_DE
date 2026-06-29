#include "settings/yurta-wallpaper-page.h"
#include "adwaita.h"
#include "gdk/gdk.h"
#include "gio/gio.h"
#include "glib-object.h"
#include "glib.h"
#include "gtk/gtk.h"
#include "gtk/gtkshortcut.h"

struct _YurtaWallpaperPage {
  AdwNavigationPage parent_instance;
  GtkFlowBox *wallpaper_flowbox;
  GSettings *settings;
};

typedef struct {
  YurtaWallpaperPage *page;
  gchar *file_path;
  GdkTexture *texture;
} WallpaperPreviewLoadData;

G_DEFINE_TYPE(YurtaWallpaperPage, yurta_wallpaper_page,
              ADW_TYPE_NAVIGATION_PAGE)

// Need for optimization
static GdkTexture *make_thumbnail(GFile *file, int target_width,
                                  int target_height) {
  GError *error = NULL;
  GFileInputStream *stream = g_file_read(file, NULL, &error);

  if (!stream) {
    g_warning("Failed to open wallpaper stream: %s", error->message);
    g_clear_error(&error);
    return NULL;
  }

  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_stream_at_scale(
      G_INPUT_STREAM(stream), target_width, target_height, TRUE, NULL, &error);
  g_object_unref(stream);

  if (!pixbuf) {
    g_warning("Failed to scale image stream: %s", error->message);
    g_clear_error(&error);
    return NULL;
  }

  GBytes *bytes = g_bytes_new_static(gdk_pixbuf_read_pixels(pixbuf),
                                     gdk_pixbuf_get_byte_length(pixbuf));

  GdkTexture *thumbnail_texture = GDK_TEXTURE(gdk_memory_texture_new(
      gdk_pixbuf_get_width(pixbuf), gdk_pixbuf_get_height(pixbuf),
      gdk_pixbuf_get_has_alpha(pixbuf) ? GDK_MEMORY_R8G8B8A8
                                       : GDK_MEMORY_R8G8B8,
      bytes, gdk_pixbuf_get_rowstride(pixbuf)));

  g_bytes_unref(bytes);

  g_object_set_data_full(G_OBJECT(thumbnail_texture), "associated-pixbuf-guard",
                         pixbuf, g_object_unref);

  return thumbnail_texture;
}

static void on_wallpaper_selected(GtkButton *btn, gpointer user_data) {
  YurtaWallpaperPage *self = YURTA_WALLPAPER_PAGE(user_data);

  const gchar *wallpaper_path =
      g_object_get_data(G_OBJECT(btn), "path-string-guard");
  // const gchar *wallpaper_path = (const gchar *)user_data;
  if (!wallpaper_path || wallpaper_path[0] == '\0') {
    g_warning("Cannot switch background: Provided wallpaper path is empty or "
              "invalid.");
    return;
  }

  g_print("Initiating background update via swaybg to target asset: %s\n",
          wallpaper_path);

  g_settings_set_string(self->settings, "current-wallpaper-path",
                        wallpaper_path);

  // Kill other swaybg proccesses
  g_spawn_command_line_async("pkill swaybg", NULL);

  // start own swaybg
  g_autofree gchar *cmd =
      g_strdup_printf("swaybg -m fill -i \"%s\"", wallpaper_path);

  GError *error = NULL;
  if (!g_spawn_command_line_async(cmd, &error)) {
    g_warning("Critical error encountered while spawning swaybg framework: %s",
              error->message);
    g_clear_error(&error);
  }

  GtkWidget *dialog = gtk_widget_get_ancestor(GTK_WIDGET(btn), ADW_TYPE_DIALOG);
  if (dialog) {
    adw_dialog_close(ADW_DIALOG(dialog));
  }
}

static void on_clicked_bin(GtkGestureClick *gesture, int n_press, double x,
                           double y, gpointer user_data) {
  GtkWidget *card =
      gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));
  YurtaWallpaperPage *self = YURTA_WALLPAPER_PAGE(user_data);

  const gchar *card_file_path =
      g_object_get_data(G_OBJECT(card), "wallpaper-path");
  if (!card_file_path)
    return;

  // Creating Dialog
  AdwDialog *dick = adw_dialog_new();
  adw_dialog_set_content_width(dick, 1000);
  adw_dialog_set_content_height(dick, 600);

  // Creating ToolBar
  GtkWidget *toolbar_view = adw_toolbar_view_new();

  GtkWidget *header_bar = adw_header_bar_new();
  adw_header_bar_set_title_widget(ADW_HEADER_BAR(header_bar),
                                  adw_window_title_new("Are u Sure ?", NULL));

  adw_toolbar_view_add_top_bar(ADW_TOOLBAR_VIEW(toolbar_view), header_bar);

  // Create a layout
  GtkWidget *box_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
  gtk_widget_set_margin_top(box_box, 16);
  gtk_widget_set_margin_bottom(box_box, 16);
  gtk_widget_set_margin_start(box_box, 16);
  gtk_widget_set_margin_end(box_box, 16);

  // Image Preview
  GFile *preview_file = g_file_new_for_path(card_file_path);
  GtkWidget *preview = gtk_picture_new_for_file(preview_file);
  g_object_unref(preview_file);

  gtk_picture_set_content_fit(GTK_PICTURE(preview), GTK_CONTENT_FIT_CONTAIN);
  gtk_widget_set_vexpand(preview, TRUE);
  gtk_box_append(GTK_BOX(box_box), preview);

  // Create a button
  GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
  gtk_widget_set_halign(button_box, GTK_ALIGN_END);

  GtkWidget *btn_apply = gtk_button_new_with_label("Set as Background");
  gtk_widget_add_css_class(btn_apply, "suggested-action");

  gchar *button_allocated_path = g_strdup(card_file_path);
  g_signal_connect(btn_apply, "clicked", G_CALLBACK(on_wallpaper_selected),
                   self);
  g_object_set_data_full(G_OBJECT(btn_apply), "path-string-guard",
                         button_allocated_path, g_free);

  gtk_box_append(GTK_BOX(button_box), btn_apply);
  gtk_box_append(GTK_BOX(box_box), button_box);

  adw_toolbar_view_set_content(ADW_TOOLBAR_VIEW(toolbar_view), box_box);

  // Show dialog
  adw_dialog_set_child(dick, toolbar_view);
  adw_dialog_present(dick, GTK_WIDGET(self));
}


static gboolean add_preview_card(gpointer user_data) {
  WallpaperPreviewLoadData *data = (WallpaperPreviewLoadData *)user_data;
  YurtaWallpaperPage *self = data->page;

  // Creatig GtkPicture
  GtkWidget *gtk_picture;
  if (data->texture) {
    gtk_picture = gtk_picture_new_for_paintable(GDK_PAINTABLE(data->texture));
  } else {
    GFile *image_file = g_file_new_for_path(data->file_path);
    gtk_picture = gtk_picture_new_for_file(image_file);
    g_object_unref(image_file);
  }

  gtk_picture_set_content_fit(GTK_PICTURE(gtk_picture), GTK_CONTENT_FIT_COVER);
  gtk_widget_set_size_request(gtk_picture, 120, 80);

  // Creating Bin (Card) Widget
  GtkWidget *card = adw_bin_new();
  gtk_widget_add_css_class(card, "card");
  adw_bin_set_child(ADW_BIN(card), gtk_picture);

  g_object_set_data_full(G_OBJECT(card), "wallpaper-path", g_strdup(data->file_path),
                         g_free);

  // Add click action to bin
  GtkGesture *click_gesture = gtk_gesture_click_new();
  g_signal_connect(click_gesture, "pressed", G_CALLBACK(on_clicked_bin), self);
  gtk_widget_add_controller(card, GTK_EVENT_CONTROLLER(click_gesture));

  // Add done picture carc to FlowBox
  gtk_flow_box_append(self->wallpaper_flowbox, card);


  //Clean task data
  g_free(data->file_path);
  g_clear_object(&data->texture);
  g_free(data);

  return G_SOURCE_REMOVE;
}

static void preview_thread_load(GTask *task, gpointer source_object,
                                gpointer task_data, GCancellable *cancellable) {

  WallpaperPreviewLoadData *data = (WallpaperPreviewLoadData *)task_data;
  GFile *image_file = g_file_new_for_path(data->file_path);

  data->texture = make_thumbnail(image_file, 240, 160);
  g_idle_add(add_preview_card, data);

  g_object_unref(image_file);
  g_object_unref(task);
}

static void init_wallpaper(YurtaWallpaperPage *self) {
  // Setup path
  const gchar *home = g_get_home_dir();
  g_autofree gchar *path =
      g_build_filename(home, "Pictures", "wallpapers", NULL);

  // Setup directory
  GFile *dir = g_file_new_for_path(path);
  GFileEnumerator *enumerator = g_file_enumerate_children(
      dir, G_FILE_ATTRIBUTE_STANDARD_NAME, G_FILE_QUERY_INFO_NONE, NULL, NULL);

  if (!enumerator) { // Check if dir is NULL
    g_warning("Can`t load path: %s", path);
    g_object_unref(dir);
    return;
  }

  // Loop trhoug directory
  GFileInfo *info;
  while ((info = g_file_enumerator_next_file(enumerator, NULL, NULL)) != NULL) {
    const gchar *filename = g_file_info_get_name(info);

    // Skip hidden files
    if (filename[0] == '.') {
      g_object_unref(info);
      continue;
    }

    gchar *file_path = g_build_filename(path, filename, NULL);
    // GFile *image_file = g_file_new_for_path(file_path);

    // Creating task
    WallpaperPreviewLoadData *taskData = g_new0(WallpaperPreviewLoadData, 1);
    taskData->page = self;
    taskData->file_path = file_path;

    // Start async tasking
    GTask *task = g_task_new(self, NULL, NULL, NULL);
    g_task_set_task_data(task, taskData, NULL);
    g_task_run_in_thread(task, preview_thread_load);
     
    g_object_unref(info);
  }

  // Clear
  g_file_enumerator_close(enumerator, NULL, NULL);
  g_object_unref(enumerator);
  g_object_unref(dir);
}

static void yurta_wallpaper_page_dispose(GObject *object) {
  YurtaWallpaperPage *self = YURTA_WALLPAPER_PAGE(object);
  g_clear_object(&self->settings);

  gtk_widget_dispose_template(GTK_WIDGET(object), YURTA_TYPE_WALLPAPER_PAGE);
  G_OBJECT_CLASS(yurta_wallpaper_page_parent_class)->dispose(object);
}

static void yurta_wallpaper_page_init(YurtaWallpaperPage *self) {
  gtk_widget_init_template(GTK_WIDGET(self));
  self->settings = g_settings_new("org.yurta.settings");
  g_signal_connect_swapped(self, "map", G_CALLBACK(init_wallpaper), self);
}

static void yurta_wallpaper_page_class_init(YurtaWallpaperPageClass *klass) {
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

  object_class->dispose = yurta_wallpaper_page_dispose;

  gtk_widget_class_set_template_from_resource(
      widget_class, "/org/yurta/de/ui/settings_de_wallpaper_page.ui");

  gtk_widget_class_bind_template_child(widget_class, YurtaWallpaperPage,
                                       wallpaper_flowbox);
}

YurtaWallpaperPage *yurta_wallpaper_page_new(void) {
  return g_object_new(YURTA_TYPE_WALLPAPER_PAGE, NULL);
}