// Config common to both cpu0 and cpu1

#define GB_CONFIG_NET_WIFI		1
#define GB_CONFIG_NET_STATIC		1
#define GB_CONFIG_NET_FALLBACK		0
#define GB_CONFIG_NET_PORTAL		0
#define GB_CONFIG_NET_SNTP		1
#define GB_CONFIG_NET_HA		0
#define GB_CONFIG_NET_OTA		1
#define GB_CONFIG_NET_WEB_SERVER	1

// cpu0-specific config

#if _CPU == 0

#define GB_CONFIG_32_KHZ_XTAL		0	// testing with it off
#define GB_CONFIG_APP0_LED		1
#define GB_CONFIG_CPU1_DEFAULTS_ON	0
#define GB_CONFIG_TFT_PWR_DEFAULTS_ON 	0
#define GB_CONFIG_TFT_BACKLIGHT_DEFAULTS_ON 1
#define GB_CONFIG_BUZZER 		0
#define GB_CONFIG_BUZZER_MONOTONE	0

#endif // _CPU == 0
 
// cpu1-specific config

#if _CPU == 1

#endif // _CPU == 1

