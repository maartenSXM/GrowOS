#ifndef __INCLUDE_CONFIG_H__
#define __INCLUDE_CONFIG_H__

#ifdef _PROJTAG_0
#define _CPU 0
#define GOS_CPU0
#else
#define _CPU 1
#define GOS_CPU1
#endif

#define GOS_FLASH_SIZE_4MB	4
// Config specific to current USER

#if _USER_maarten
#define _SERIALNO 100
#else // _USER_erik
#define _SERIALNO 101
#endif

// Config common to both cpu0 and cpu1

#define GOS_FLASH_SIZE			GOS_FLASH_SIZE_4MB
#define GOS_CONFIG_TINYUSB		0
#define GOS_CONFIG_RUN_FAST		1	// run at 240Mhz, not 160Mhz
#define GOS_CONFIG_NET_WIFI		1	// WiFi?
#define GOS_CONFIG_NET_IMPROV_WIFI	1	// Improv WiFi provisioning
#define GOS_CONFIG_NET_STATIC		1	// ... using static config?
#define GOS_CONFIG_NET_DHCP		0	// ... using dhcp?
#define GOS_CONFIG_NET_FALLBACK		0	// ... Fallback HotSpot?
#define GOS_CONFIG_NET_PORTAL		0	// ... Provisioning SSID?
#define GOS_CONFIG_NET_MDNS		1	// ... mDNS service lookup?
#define GOS_CONFIG_NET_SNTP		1	// ... Network Time Protocol?
#define GOS_CONFIG_NET_HA		0	// ... Home Assistant?
#define GOS_CONFIG_NET_OTA		1	// ... WiFi update allowed?
#define GOS_CONFIG_NET_WEB_SERVER	1	// ... Built-in web server?
#define GOS_CONFIG_NET_TELNET_SERVER	1	// ... Built-in telnet server?
						//
#define GOS_CONFIG_INTERNAL_TEMP		1	// internal temp sensor ...
// ... causes reboots: https://github.com/espressif/esp-idf/issues/8088
// ... fixed as of esp-idf v5.1.2
//
#define GOS_CONFIG_DEBUG			1	// expose some debug info

// cpu0-specific config

#if _CPU == 0

#undef GOS_CONFIG_TINYUSB
#define GOS_CONFIG_TINYUSB		0
#define GOS_CONFIG_CONSOLE		0	// configure console?
#define GOS_CONFIG_32_KHZ_XTAL		1	// Use external 32 KHz XTAL?
#define GOS_CONFIG_APP0_LED		1	// status LED on _PIN_tempData?
#define GOS_CONFIG_CPU1_DEFAULTS_ON	0	// CPU1 on by default?
#define GOS_CONFIG_LVGL			1	// use lvgl ?
#define GOS_CONFIG_DISPLAY		1	// touchscreen display?
#define GOS_CONFIG_DISPLAY_3_BIT	0	// ... 8 color (else 64k)?
#define GOS_CONFIG_TFT_PWR_DEFAULTS_ON 	1	// ... power it on by default?
#define GOS_CONFIG_TFT_BACKLIGHT_DEFAULTS_ON 1	// ... backlight on by default?
#define GOS_CONFIG_BUZZER 		0	// buzzer on board?
#define GOS_CONFIG_BUZZER_MONOTONE	0	// ... monotone? (else melody)
#define GOS_CONFIG_CAMERA		0	// esphome camera lib?
#define GOS_CONFIG_ESP32_CAMERA		0	// esp-idf cam lib?
#define GOS_CONFIG_CAMERA_DEFAULTS_ON	0	// ... on by default?
#define GOS_CONFIG_SERIAL_SERVER		0	// serve uart on port 8888?
#define GOS_CONFIG_USB_ACM		0	// enable tiny usb acm

#if GOS_CONFIG_LVGL
#define GOS_CONFIG_LVGL_BOOT_LOGO	1	// spinning boot logo?
#define GOS_CONFIG_LVGL_IDLE		1	// turn off display when idle?
#define GOS_CONFIG_LVGL_ANTIBURN		1	// exercise pixels when idle?
#endif // GOS_CONFIG_LVGL

#endif // _CPU == 0
 
// cpu1-specific config

#if _CPU == 1
#define GOS_CONFIG_LVGL			0	// use lvgl?
#define GOS_CONFIG_DISPLAY_3_BIT	0	// ... 8 color (else 64k)?
#define GOS_CONFIG_CONSOLE		0	// esp-idf console?
#define GOS_CONFIG_SERIAL_SERVER		0	// serve uart on port 8888?
#define GOS_CONFIG_ESP32_CAMERA		0	// esp-idf cam lib?
#undef GOS_CONFIG_NET_FALLBACK
#define GOS_CONFIG_NET_FALLBACK		0	// ... Fallback HotSpot?
#define GOS_CONFIG_USB_ACM		0	// enable tiny usb acm

#endif // _CPU == 1

// some C preprocess string concatenation macros that are useful
// for passing esphome and platformio some generated build_flags.
// See env.yaml for exmaples of how they can be used.
//
#define _ID(x) x
#define _STR(x) #x
#define STR(x) _STR(x)
#define DEF_CAT4(w,x,y,z) STR(_ID(w)_ID(x)_ID(y)_ID(z))
#define DEF_CAT3(w,x,y)   STR(_ID(w)_ID(x)_ID(y))
#define DEF_CAT2(w,x)     STR(_ID(w)_ID(x))
#define DEF_CAT(w,x)      STR(_ID(w)_ID(x))
#define DEF_NAME(w,x,y,z) _ID(w)_ID(x)_ID(y)_ID(z)

#define GOS_NAME  DEF_NAME(gb,_SERIALNO,cpu,_CPU)
#define GOS_NAME_STR STR(GOS_NAME)

#endif // __INCLUDE_CONFIG_H__
