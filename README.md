# GrowOS

Embedded Operating System and User Interface that runs on the ESPS2 Based growBoard by Westlake Horticulture.

### Hardware Required

- A CyberGrow Board with ESP32s2
- A USB cable for Power supply and programming.

## Setting up dev environment

This project uses ESP-IDF (IoT Development Framework) and Arduino Core for ESP32 chips. ESP-IDF provides complete customization and access to professional developement tools to aid with IoT dev, and Arduino brings the largest embedded community and code libraries.

1. find a cozy spot on your machine to clone the ESP-IDF repo. This might also be where your source files are kept
2. Ensure you have python3 installed.
3. Clone Esp-idf v4.4. [GITHUB](https://github.com/espressif/esp-idf) [DOCS](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/index.html)

```
    git clone --recursive -b release/v4.4 https://github.com/espressif/esp-idf.git
```

4. Clone the Arduino core for esp32 [GITHUB](https://github.com/espressif/arduino-esp32) [DOCS](https://docs.espressif.com/projects/arduino-esp32/en/latest/libraries.html)
5.

```
    cd esp-idf/components
    git clone -b idf-release/v4.4 https://github.com/espressif/arduino-esp32.git arduino
    cd arduino
    git submodule update --init --recursive
    cd ../..
```

6. Ensure .install files are executable in the esp-idf directory
7.

```
source ./install.sh
```

Note: Python3 is required, if your machine is defaulting to python2, try making "python" command map to python3 8. After successfull install

```
source ./export.sh
```

Now you can use idf.py to run commands such as "build" and "menuconfig"
You may want to add this alias to your .zprofile "alias get_idf='. $HOME/Documents/Prog/ESP/esp-idf/export.sh'"

## Compiling

Make sure to export esp-idf via get_idf or directly calling '. $HOME/Documents/Prog/ESP/esp-idf/export.sh'

All component libraries live within the project to ensure stable dependency. Each component has a CMakeLists.txt as required by the CMAKE build tool integrated into ESP-IDF.

Be sure to set the correct chip target using `idf.py set-target <chip_name>`. Chip_name = esp32s2

Important
`idf.py set-target` will clear the build directory and re-generate the sdkconfig file from scratch. The old sdkconfig file will be saved as sdkconfig.old.

You may need to do some `idf.py menuconfig` for a bit

Then run `idf.py build` to build the project

If some error about Hz=100, change to Hz=1000

### Configure the project

Open the project configuration menu (`idf.py menuconfig`).
This is usually already configured.

### Build and Flash

Find your port with the command `ls /dev`, on an m1 macbook with only the device connected it typically is `/dev/tty.usbserial-00000000`

Build the project and flash it to the board, then run the monitor tool to view the serial output:

Run `idf.py -p <PORT> flash monitor` to build, flash and monitor the project. `idf.py -p /dev/tty.usbserial-00000000 flash monitor`

NOTE:
The ESP32-S2 chip needs to be in bootloader mode before it can be detected as a DFU device and flash. This can be achieved by pulling GPIO0 down (on a dev board this is typically done by pressing the BOOT button), pulling RESET down for a moment. GPIO0 can then be released at convenience.

(To exit the serial monitor, type `Ctrl-]`.)

See the Getting Started Guide for all the steps to configure and use the ESP-IDF to build projects.

- [ESP-IDF Getting Started Guide on ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)
- [ESP-IDF Getting Started Guide on ESP32-S2](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/index.html)
- [ESP-IDF Getting Started Guide on ESP32-C3](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/get-started/index.html)

### DEBUG

`openocd -f board/esp32s2-kaluga-1.cfg`
Then start debug (`F5`)
