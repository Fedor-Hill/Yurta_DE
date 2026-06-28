settings_sources = files(
  'main.c', 
  'yurta-wallpaper-page.c'
)

resources = gnome.compile_resources(
  'yurta-settings-resources',
  '../../resources/resources.gresource.xml', 
  source_dir: '../../resources',             
)

executable('yurta-settings',
  settings_sources,
  resources,
  include_directories : inc_dirs,
  dependencies : [common_deps], 
  install : true
)