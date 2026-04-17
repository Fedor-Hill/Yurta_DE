#ifndef BATTERY_H
#define BATTERY_H

#include <gtk/gtk.h>
#include <gio/gio.h>

typedef struct {
    GDBusProxy *proxy;
    GtkImage *icon_widget;
} BatteryModule;

BatteryModule* activateBattery(GtkImage *icon);

#endif