#!/bin/bash

# Configure xscreensaver installation for spritesaver installation
# Usage: sudo ./configure.sh
# Note: Only needs to be run once

set -e

# check dir
if [[ "$(dirname "$0")" != "." ]]; then
    echo -e "\nError: Script must be run from its parent directory\nUsage: sudo ./configure.sh"; exit 1
fi

# check sudo
if [[ $EUID -ne 0 ]]; then
    echo -e "\nError: This script must be run as root\nUsage: sudo ./configure.sh"; exit 1
fi

# check for a valid xscreensaver installation
command -v xscreensaver >/dev/null 2>&1 || { echo -e "\nError: xscreensaver is not installed"; exit 1; }

# check for ffmpeg installation
command -v ffmpeg >/dev/null 2>&1 || { echo -e "\nError: ffmpeg is not installed"; exit 1; }

# install spritesaver configuration
CONFIG_DIR='/usr/share/xscreensaver/config'
chmod 644 spritesaver.xml
cp spritesaver.xml "$CONFIG_DIR/"
echo -e "spritesaver.xml installed in $CONFIG_DIR"

APPDEFAULTS_FILE='/etc/X11/app-defaults/XScreenSaver'
if grep -q "spritesaver" "$APPDEFAULTS_FILE"; then
    echo -e "spritesaver already configured in $APPDEFAULTS_FILE"
else
    sed -i '/^\*programs:/a\
  GL: 				spritesaver -root			    \\n\\' "$APPDEFAULTS_FILE"
    echo -e "\nspritesaver configured in $APPDEFAULTS_FILE"
fi