#include "bar/battery.h"

// This function pulls the latest data and updates the UI
void refresh_battery_ui(BatteryModule *mod) {
  // Force a fresh read of the properties from the proxy's cache
  GVariant *v_icon = g_dbus_proxy_get_cached_property(mod->proxy, "IconName");
  GVariant *v_pct = g_dbus_proxy_get_cached_property(mod->proxy, "Percentage");

  if (v_icon) {
    const char *name = g_variant_get_string(v_icon, NULL);
    gtk_image_set_from_icon_name(mod->icon_widget, name);
    g_variant_unref(v_icon);
  }

  if (v_pct) {
    double pct = g_variant_get_double(v_pct);
    char buf[16];
    snprintf(buf, sizeof(buf), "%.0f%%", pct);
    gtk_widget_set_tooltip_text(GTK_WIDGET(mod->icon_widget), buf);
    g_variant_unref(v_pct);
  }
}

// Callback triggered by GDBus when ANY property changes
static void on_properties_changed(GDBusProxy *proxy,
                                  GVariant *changed_properties,
                                  GStrv invalidated_properties,
                                  gpointer user_data) {
  (void)proxy;
  (void)changed_properties;
  (void)invalidated_properties;
  BatteryModule *mod = (BatteryModule *)user_data;

  // When ANY property changes (even if it's not IconName), refresh everything
  refresh_battery_ui(mod);
}

BatteryModule *activateBattery(GtkImage *icon) {
  GError *error = NULL;
  BatteryModule *mod = g_malloc0(sizeof(BatteryModule));
  mod->icon_widget = icon;

  // IMPORTANT: Check 'upower -e' in your terminal.
  // If it says 'battery_BAT1', change 'BAT0' to 'BAT1' below.
  mod->proxy = g_dbus_proxy_new_for_bus_sync(
      G_BUS_TYPE_SYSTEM, G_DBUS_PROXY_FLAGS_NONE, NULL,
      "org.freedesktop.UPower", "/org/freedesktop/UPower/devices/battery_BAT0",
      "org.freedesktop.UPower.Device", NULL, &error);

  if (error) {
    g_warning("GDBus Error: %s", error->message);
    g_error_free(error);
    return mod;
  }

  // Connect the signal
  g_signal_connect(mod->proxy, "g-properties-changed",
                   G_CALLBACK(on_properties_changed), mod);

  // Initial update
  refresh_battery_ui(mod);

  return mod;
}