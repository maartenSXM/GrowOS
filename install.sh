#!/usr/bin/env bash

# This script can be re-run as it will only install things if needed.

# Note: cpphash is updated using git pull if it exists.
# To update GrowOS itself, please do it manuallly using git pull.

set -o nounset

declare -r CDIR="./cpphash"
declare -r VENV="./build/venv"

# Check that cpphash has been cloned into $CDIR and is up to date

if [ ! -f "$CDIR/make/cpphash.mk" ]; then
  echo "cd $(dirname $CDIR)"
  cd $(dirname "$CDIR")
  echo "git clone https://github.com/maartenSXM/cpphash.git"
  git clone https://github.com/maartenSXM/cpphash.git
  if [ ! -f "$CDIR/cpphash.mk" ]; then
    echo "$0: Could not git clone cpphash. Aborting."
    exit 1
  fi
  echo cd -
  cd - 
fi

# Check that cpphash is successfully installed

echo "$CDIR/install.sh -y -v $VENV"
"$CDIR"/install.sh -y -v "$VENV"
if [ $? -ne 0 ]; then
  echo "$0: Could not successfully install cpphash. Aborting."
  exit 1
fi

# Check that libmagic is available to esphome on Darwin

if [[ "$(uname)" == "Darwin" ]]; then
  if [[ "$(command -v brew)" == "" ]]; then
    echo "$0: brew not found. Aborting."
  fi
  brew list libmagic 2>/dev/null | grep -q libmagic
  if [ $? -ne 0 ]; then
    echo "brew install libmagic"
    brew install libmagic
    if [ $? -ne 0 ]; then
      echo "$0: Could not brew install libmagic. Aborting."
      exit 1
    fi
  fi
fi

if [[ ! -f "./secrets.h" && ! -f "../secrets.h" ]]; then
  while :; do
    echo "secrets.h not found. Would you like me to create one? [y/n]"
    read -n 1 -r
    echo
    if [[ $REPLY =~ ^[QqNn]$ ]]; then
      break
    fi
    if [[ $REPLY =~ ^[Yy]$ ]]; then
      read -re -p "Please enter the SSID GrowOS should connect to: "
      ssid="$REPLY"
      while :; do
        IFS= read -re -p "Please enter the password for SSID $ssid: " 
        echo
        pwd="$REPLY"
        if ((${#pwd} >= 8)); then
	  break
	fi
        echo "SSID passwords must be at least 8 characters."
      done
      echo "
#define _SECRET_WIFI_SSID     $ssid
#define _SECRET_WIFI_PASSWORD $pwd

// These #defaults are in gos/network.yaml.  Override them here.
//  #default _SECRET_HA_PASSWORD        _SECRET_WIFI_PASSWORD
//  #default _SECRET_OTA_PASSWORD       _SECRET_WIFI_PASSWORD
" > secrets.h
      break;
    fi
  done
fi

echo "$0: done"

exit 0

