project(
  'yurta DE',
  'c',
  meson_version : '>= 1.3.0',
  version : '0.1'
)

src_files = files(
  'src/main.c', 
  'src/bar/bar.c', 
  'src/bar/battery.c',
  'src/bar/clock.c')
inc_dirs = include_directories('include')

dependencies = [
  dependency('gtk4-layer-shell-0'), dependency('gtk4'), dependency('dbus-1'), dependency('libpipewire-0.3')
]

executable(
  'run_yurta',
  src_files, 
    include_directories : inc_dirs,
  dependencies : dependencies
)
