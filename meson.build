project(
  'yurta-de',
  'c',
  version : '0.1.2',
  meson_version : '>= 1.3.0',
  default_options : ['c_std=gnu17', 'warning_level=2']
)

# Global dependencies
gtk_dep = dependency('gtk4')
libadwaita_dep = dependency('libadwaita-1')
layer_shell_dep = dependency('gtk4-layer-shell-0')
dbus_dep = dependency('dbus-1')
pipewire_dep = dependency('libpipewire-0.3')
wireplumber_dep = dependency('wireplumber-0.5')
glib_dep    = dependency('glib-2.0')
gobject_dep = dependency('gobject-2.0')
lubpulse_dep = dependency('libpulse')
libpulse_mainloop_dep = dependency('libpulse-mainloop-glib')

# Make them available to subdirectories
common_deps = [gtk_dep, libadwaita_dep, layer_shell_dep, dbus_dep, pipewire_dep, wireplumber_dep, glib_dep, gobject_dep, lubpulse_dep, libpulse_mainloop_dep]
inc_dirs = include_directories('include')

# Go into subdirectories
subdir('src/bar')
subdir('src/settings')
