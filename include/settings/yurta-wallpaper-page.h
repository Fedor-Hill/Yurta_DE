#pragma once

#include <adwaita.h>

G_BEGIN_DECLS

#define YURTA_TYPE_WALLPAPER_PAGE (yurta_wallpaper_page_get_type())

G_DECLARE_FINAL_TYPE (YurtaWallpaperPage, yurta_wallpaper_page, YURTA, WALLPAPER_PAGE, AdwNavigationPage)

YurtaWallpaperPage *yurta_wallpaper_page_new (void);

G_END_DECLS