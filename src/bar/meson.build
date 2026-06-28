gnome = import('gnome')

bar_sources = files(
  'main.c',
  'bar.c',
  'battery.c',
  'clock.c',
  'sound.c'
)

resources = gnome.compile_resources(
  'yurta-bar-resources',
  '../../resources/resources.gresource.xml', 
  source_dir: '../../resources'
)

executable('yurta-bar',
  bar_sources,
  resources,
  include_directories : inc_dirs,
  dependencies : common_deps,
  install : true
)
