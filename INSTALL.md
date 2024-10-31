To install GrowOS on Linux Debian or MacOS Darwin, clone it from github and then cd into GrowOS and run ./install.sh.

GrowOS clones cpphash and then runs cpphash/install.sh as part of its install process.  cpphash/install.sh in turn will install its dependencies and create a virtual python environment containing the latest esphome release.

Once installed, GrowOS can be activated using 'source Bashrc', which sets GOS_HOME to the GrowOS home directory and, in turn sources cpphash/Bashrc specifying $GOS_HOME/build as the build directory.  To change the build directory, edit $GOS_HOME/Bashrc and also $GOS_HOME/Makefile so that they are in sync.

The default GrowOS project is lilygot4s3.  It can be built using 'make' with no arguments from $GOS_HOME - or from anywhere using 'gos_make'. 

gos_help will list the convenience aliases defined by $GOS_HOME/Bashrc and $GOS_HOME/cpphash/Bashrc.

To select a specific GrowOS projects, for example, do this from GOS_HOME:

  make PRJ=projects/lilygot4s3/debug.mk

GrowOS remembers the last project in $GOS_HOME/.cpphash_prj so that you don't have to specify a PRJ= argument to make unless you are changing from one project to another.
