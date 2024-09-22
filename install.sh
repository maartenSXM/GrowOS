#!/usr/bin/env bash

# This script can be re-run as it will only install things if needed.

# Note: cpphash is updated using git pull if it exists.
# To update GrowOS itself, please do it manuallly using git pull.

set -o nounset

declare -r CDIR="./cpphash"
declare -r VENV="./build/venv"

# Check that cpphash has been cloned into $CDIR and is up to date

if [ ! -f "$CDIR/cpphash.mk" ]; then
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
else
  cd "$CDIR"
  echo "git pull"
  git pull
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

echo "$0: done"

exit 0

