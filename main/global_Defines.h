#ifndef GLOBAL_DEFINES_H_
#define GLOBAL_DEFINES_H_

// HARDWARE CONFIG

#define H_GPIO 0
#define CSI_VSYNC 1
#define CSI_D9 2
#define CSI_D8 3
#define CSI_D7 4
#define CSI_D6 5
#define CTP_IRQ 6
#define CTP_RST 7
#define TFT_RST 7
#define TWI_SDA 8
#define TWI_SCK 9
#define CSI_PWR_EN 10
#define CSI_PCLK 11
#define CSI_D4 12
#define H_S_ENABLE 13
#define PWROFF_INT 14
#define TEMP_DATA 17
#define TFT_PWR_EN 18
#define H_USB_MINUS 19
#define H_USB_PLUS 20
#define TFT_BACKLIGHT 21
#define TFT_BL TFT_BACKLIGHT
#define TFT_BACKLIGHT_ON = 0
#define TFT_CS 33
#define CSI_HREF 33
#define TFT_DC 34
#define CSI_XCLK 34
#define TFT_MOSI 35
#define CSI_D2 35
#define TFT_SCLK 36
#define CSI_D3 36
#define TFT_MISO 37
#define CSI_D5 37
#define VBAT_SENSE 38
#define H_GPIO39 39
#define H_GPIO40 40
#define H_GPIO41 41
#define H_GPIO42 42
#define H_U0TXD
#define H_U0RXD
#define VBAT_CE 45

// TFT and Touch CONFIG
#define FT6236_TOUCH_FREQUENCY 400000

#define TFT_RGB_ORDER TFT_RGB
#define TFT_INVERSION_OFF

#define TFT_WIDTH 480
#define TFT_HEIGHT 320

#define ILI9488_DRIVER
#define SPI_FREQUENCY 20000000 // Max Reliable for ILI9488
// Optional reduced SPI frequency for reading TFT
// #define SPI_READ_FREQUENCY 20000000
#define SPI_READ_FREQUENCY 5000000

#define LOAD_GLCD  // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2 // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4 // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6 // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7 // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8 // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
// #define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
#define LOAD_GFXFF // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

// Comment out the #define below to stop the SPIFFS filing system and smooth font code being loaded
// this will save ~20kbytes of FLASH
#define SMOOTH_FONT

#define maxLabelLength 20

#define DEFAULT_SCAN_LIST_SIZE CONFIG_EXAMPLE_SCAN_LIST_SIZE

// COLOUR CONFIG
// 565 colours http://www.barth-dev.de/online/rgb565-color-picker/
#define WHITE 0xFFFF       /* 255, 255, 255 */
#define BLACK 0x0000       /*   0,   0,   0 */
#define OFFBLACK 0x2124    /*  38,  38,  38 */
#define DARKGREY 0x7BEF    /* 128, 128, 128 */
#define GREY 0x9CF3        /* 156, 156, 156 */
#define LIGHTGREY 0xC618   /* 192, 192, 192 */
#define NAVY 0x000F        /*   0,   0, 128 */
#define MAROON 0x7800      /* 128,   0,   0 */
#define PURPLE 0x780F      /* 128,   0, 128 */
#define OLIVE 0x7BE0       /* 128, 128,   0 */
#define RED 0xF800         /* 255,   0,   0 */
#define BLUE 0x001F        /*   0,   0, 255 */
#define GREEN 0x07E0       /*   0, 255,   0 */
#define DARKGREEN 0x03E0   /*   0, 128,   0 */
#define CYAN 0x07FF        /*   0, 255, 255 */
#define DARKCYAN 0x03EF    /*   0, 128, 128 */
#define RED 0xF800         /* 255,   0,   0 */
#define MAGENTA 0xF81F     /* 255,   0, 255 */
#define YELLOW 0xFFE0      /* 255, 255,   0 */
#define ORANGE 0xFDA0      /* 255, 180,   0 */
#define GREENYELLOW 0xB7E0 /* 180, 255,   0 */
#define PINK 0xFC9F
#define OPBOX_GREEN 0x7DE8

#define offOutline GREY
#define offFill BLACK
#define textColour WHITE
#define onOutline GREEN
#define onFill DARKGREEN
#define hiddenTextcolor DARKGREY

#define PORTRAIT 0
#define LANDSCAPE 1

#define maxNumSlots 100

#define maxLabelLength 20
#define numMonths 12
#define numDayBtns 8
#define numWeekBtns 13

#define secondsInDay 86400
#define growOSSizeOfDeviceLabel 20

#define numDevices 7

/***** Global Vars *****/

/***** GENERAL SOFTWARE CONFIG *****/
// #define Serial SerialUSB                              //native uses SerialUSB
// #define DEBUG                                         //for serial debugging and other
// #define DEBUGTOUCH                                    //shows boxes around items that can be touched
// #define EnsureSerial                                  //waits for serial port to be ready
// #define DEVELOPMENT_MODE                              //settings to use when developing
// #define saveToFlash                                   //to enable saving important settings to flash and recovering them after power off.

// Uncomment whatever type you're using!
// #define DHTTYPE DHT22
#define DHTTYPE DHT21 // DHT 21 (AM2301)     //DHTTYPE DHT11 = DHT 11  // DHT 22  = (AM2302), AM2321

#ifdef DEVELOPMENT
#else
#endif

#endif // GLOBAL_DEFINES_H_