To install GrowOS on Linux or Darwin, clone it from github
and then cd into GrowOS and run ./install.sh.

GrowOS clone cpphash and run cpphash/install.sh as part of its
install process.  cpphash/install.sh in turn will install its
dependencies and create a virtual python environment containing the
latest esphome release.

Once installed, GrowOS can be activated using 'source Bashrc',
which sets GOS_HOME to the GrowOS home directory and, in turn
sources cpphash/Bashrc specifying $GOS_HOME/build as the build
directory.  To change the build directory, edit $GOS_HOME/Bashrc
and also $GOS_HOME/Makefile so that they are in sync.

The default GrowOS project is lilygot4s3.  It can be built using
'make' with no arguments from $GOS_HOME - or from anywhere
using 'gos_make'. 

gos_help will list the convenience aliases defined by $GOS_HOME/Bashrc
and $GOS_HOME/cpphash/Bashrc.

To select a specific GrowOS projects, for example, do this from GOS_HOME:

  make PRJ=projects/lilygot4s3/debug.mk

GrowOS remembers the last project in $GOS_HOME/.espmake so that you
don't have to specify a PRJ= argument to make unless you are changing
from one project to another.

It is also possible to issue esphome commands directly on the generated
espmake.yaml file from the build directory. To change to the last
built project's build directory, you can use the gos_build alias.

Then commands such as these can be issued:

  gos_build
  esphome compile espmake.yaml
  esphome upload espmake.yaml
  esphome logs espmake.yaml

See esphome -h for more details.

To burn GrowOS to a board, try gos_upload or gos_jtag.

To set serial port names or IP addresses used by convenience aliases,
set these environment variables.  Otherwise these defaults are
set by cpphash/Bashrc:

  _ESPMAKE_IP0=192.168.248.10
  _ESPMAKE_IP1=192.168.248.11
  _ESPMAKE_DEV0=/dev/ttyACM0
  _ESPMAKE_DEV0=/dev/ttyACM1

Once GrowOS is running on your hardware, It is possible to connect
to GrowOS's IP address from a browser to see some published information 
and flip some switches.

Notes on burning firmware for the first time:

On first upload without existing esphome firmware or network connectivity,
you may have to program over jtag/usb. Look in /dev/* for the path of the
serial or USB port, which can be found by looking at the output of dmesg
after plugging in your hardware.  If running in a VM, you may have t
 connect that port to the VM from the host.

To put the an ESP32 device into jtag upload mode and download GrowOS to it,
ground GPIO0 (.e. hold the BOOT button) and issue the convenience alias
gos_jtag.

To erase the flash over JTAG when a CPU is in upload mode, you
can use the esptool.py script from Espressif like this:

esptool.py --chip esp32s2 erase_flash

Also, from the .esphome/build/gb100cpu0, you can inspect the esp-idf confif
like this:

   pio run -t menuconfig

Saving the minimal configuration from the menuconfig will show you what
settings have been tweaked in esp-idf by esphome and platformio.

If using the GOS_CONFIG_CONSOLE options, the UART is accessible over
TCP port 1234.  To connect to the UART over TCP, run socat in one
terminal and minicom in another. Or run socat in the background like this:

  socat pty,link=$HOME/ttyV0,waitslave tcp:gb100cpu0.local:1234 &
  minicom --device ~/ttyV0

Some other miscellaneous notes for developers:

Alternatively to GrowOS, to compile and run an esp-idf project directly,
it is possible to do that using the esp-idf frameworks stored in
~/.platformio with commands similar to:

  cd ~/.platformio/packages/framework-espidf
  sh ./install.sh
  . ./export.sh
  cd examples/system/console/advanced
  idf.py build
  esptool.py --chip esp32s2 -b 460800 --before default_reset --after hard_reset write_flash --flash_mode dio --flash_size 4MB --flash_freq 80m 0x1000 build/bootloader/bootloader.bin 0x8000 build/partition_table/partition-table.bin 0x10000 build/console.bin

Note that the chip type and addresses and sizes and start address may vary
depending on the hardware targetted.

To validate a partition table part.csv, find gen_esp32part.py path to generate
the .bin and display it using:

  python <path>/gen_esp32part.py part.csv part.bin
  python <path>/gen_esp32part.py part.bin

Other Install Notes:

1. If build results in "No module named pip", then run this python
   command with an absolute path of the form:

     ~/.platformio/penv/.espidf-5.2.1/bin/python -m ensurepip

2. To prune old platformio toolchains etc, you can see what will be
   pruned using:

     pio system prune --dry-run

   and you can remove them with:

     pio system prune

