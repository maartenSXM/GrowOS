// Config common to both cpu0 and cpu1

#define GB_CONFIG_NET_WIFI		1	// WiFi?
#define GB_CONFIG_NET_STATIC		1	// ... using static config?
#define GB_CONFIG_NET_FALLBACK		0	// ... Fallback config?
#define GB_CONFIG_NET_PORTAL		0	// ... Provisioning SSID?
#define GB_CONFIG_NET_SNTP		1	// ... Network Time Protocol?
#define GB_CONFIG_NET_HA		0	// ... Home Assistant?
#define GB_CONFIG_NET_OTA		1	// ... WiFi update allowed?
#define GB_CONFIG_NET_WEB_SERVER	1	// ... Built-in web server?

// cpu0-specific config

#if _CPU == 0

#define GB_CONFIG_32_KHZ_XTAL		1	// Use external 32 KHz XTAL?
#define GB_CONFIG_APP0_LED		1	// status LED on _PIN_tempData?
#define GB_CONFIG_CPU1_DEFAULTS_ON	0	// CPU1 on by default?
#define GB_CONFIG_DISPLAY		1	// touchscreen display?
#define GB_CONFIG_TFT_PWR_DEFAULTS_ON 	1	// ... power it on by default?
#define GB_CONFIG_TFT_BACKLIGHT_DEFAULTS_ON 1	// ... backlight on by default?
#define GB_CONFIG_BUZZER 		0	// buzzer on board?
#define GB_CONFIG_BUZZER_MONOTONE	0	// ... monotone? (else melody)
#define GB_CONFIG_CAMERA		0	// camera connected?
#define GB_CONFIG_CAMERA_DEFAULTS_ON	0	// ... on by default?

#endif // _CPU == 0
 
// cpu1-specific config

#if _CPU == 1

#endif // _CPU == 1

