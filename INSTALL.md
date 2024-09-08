To install esphome, do these steps:

sudo apt install git make python3.10-venv gcc yq
sudo snap install yq
    (on MacOS brew install git make python3.10-venv gcc yq)
git clone https://github.com/esphome/esphome.git
cd esphome
vi requirements_dev.txt and comment out clang-tidy (takes too long and not needed)
./script/setup
Add this next line to your ~/.bashrc (tweak paths for the git directories on your system):
alias gos-setup='source ~/git/esphome/venv/bin/activate; cd ~/git/GrowOS; source ./Bashrc'

and then issue these two commands:
source ~/.bashrc 
gos-setup

Now you are in the GrowOS home directory.  You can build
the default project (for lilygot4s3) from $GOS_HOME using 'make' or
from anywhere, using 'gos-make'.  See $GOS_HOME/Bashrc for more
convenience aliases that are defined at the end of that file.

To change GrowOS projects, you can issue, for example, this:
  make PRJ=projects/growBoard0

GrowOS remembers the last project in $GOS_HOME/.goslast so you don't have
to use PRJ= unless you are changing from one project to another.

It is also possible to issue esphome commands directly on the generated
espmake.yaml file from the build directory.  To change to the project's
build directory, you can use the gos-build alias. Then esphome
commands such as these can be issued:

esphome compile espmake.yaml
esphome upload espmake.yaml
esphome logs espmake.yaml

See esphome -h for more details.

Intermediate C-preprocessed yaml files are stored in the build
subdirectory "dehashed".  

To burn GrowOS to a board, 
By default, it is possible to connect to GrowOS's IP address
from a browser to see some published information and flip
some switches.

Notes:
On first upload without existing esphome firmware, will have to program over 
/dev/* will be the path of the serial or USB port, which can be found by
looking at the output of dmesg after plugging in the growboard.  If running
in a VM, you may have to connect that port to the VM from the host.

To put the ESP32S2 into jtag upload mode, just ground GPIO0 and issue alias
jtag0 or jtag1, depending on which CPU you are connected to.

To erase the flash over JTAG when a CPU is in upload mode, you
can use the esptool.py script from Espressif like this:

esptool.py --chip esp32s2 erase_flash

Also, from the .esphome/build/gb100cpu0, you can inspect the esp-idf confif
like this:

   pio run -t menuconfig

Saving the minimal configuration from the menuconfig will show you what
settings have been tweaked in esp-idf by esphome and platformio.

If using the GB_CONFIG_CONSOLE options, the UART is accessible over
TCP port 1234.  To connect to the UART over TCP, run socat in one
terminal and minicom in another. Or run socat in the background like this:

  socat pty,link=$HOME/ttyV0,waitslave tcp:gb100cpu0.local:1234 &
  minicom --device ~/ttyV0

To compile and run an esp-idf project directly on one of the CPUs of the
Growboard, it is possible to do that using the esp-idf frameworks stored
in ~/.platformio:

   cd ~/.platformio/plackages/framework-espidf
   sh ./install.sh
   . ./export.sh
   cd examples/system/console/advanced
   idf.py build
   esptool.py --chip esp32s2 -b 460800 --before default_reset --after hard_reset write_flash --flash_mode dio --flash_size 4MB --flash_freq 80m 0x1000 build/bootloader/bootloader.bin 0x8000 build/partition_table/partition-table.bin 0x10000 build/console.bin
   echo voila!

To validate a partition table part.csv, find gen_esp32part.py path to generate
the .bin and display it using:
 python <path>/gen_esp32part.py part.csv part.bin
 python <path>/gen_esp32part.py part.bin

INSTALL_NOTES
1. If build results in "No module named pip", then run this on thei python
   command with the absolute path shown. For example:

     /home/maarten/.platformio/penv/.espidf-5.2.1/bin/python -m ensurepip

2. To prune old platformio toolchains etc, you can see what will be
   pruned using:

     pio system prune --dry-run

   and you can remove them with:

     pio system prune

