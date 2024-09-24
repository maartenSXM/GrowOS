#ifdef GOS_MAKE_native
#  define GOS_CONFIG_LVGL           0     // use native gui
#endif // GOS_MAKE_native

#ifdef GOS_MAKE_debug
  #define GOS_CONFIG_LVGL           1     // use lvgl gui
  #define GOS_CONFIG_LVGL_BOOT_LOGO 1     // spinning boot logo
  #define GOS_CONFIG_LVGL_IDLE      1     // turn off display when idle
  #define GOS_CONFIG_LVGL_ANTIBURN  1     // exercise pixels when idle
#endif // GOS_MAKE_debug
