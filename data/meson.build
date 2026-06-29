install_data(
    'org.yurta.settings.gschema.xml',
    install_dir: get_option('datadir') / 'glib-2.0/schemas',
)

gnome = import('gnome')
gnome.compile_schemas(depend_files: files('org.yurta.settings.gschema.xml'))


install_data(
    'yurta-settings.desktop',
    install_dir: get_option('datadir') / 'applications'
)

meson.add_install_script(
    'sh', '-c',
    'update-desktop-database -q ' + (get_option('prefix') / get_option('datadir') / 'applications') + ' || true'
)