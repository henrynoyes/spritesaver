#!/bin/bash

# Automatically install spritesaver using Docker 
# Usage: sudo ./autobuild.sh /path/to/left.gif /path/to/right.gif
# Note: Assumes parent folder of GIFs = sprite_name

set -e

# check dir
if [[ "$(dirname "$0")" != "." ]]; then
    echo -e "\nError: Script must be run from its parent directory\nUsage: sudo ./autobuild.sh /path/to/left.gif /path/to/right.gif"; exit 1
fi

# check sudo
if [[ $EUID -ne 0 ]]; then
    echo -e "\nError: Script must be run as root\nUsage: sudo ./autobuild.sh /path/to/left.gif /path/to/right.gif"; exit 1
fi

# check args
[[ $# -eq 2 ]] || { echo -e "\nUsage: sudo ./autobuild.sh /path/to/left.gif /path/to/right.gif"; exit 1; }
[[ -f "$1" ]] || { echo -e "\nError: $1 not found"; exit 1; }
[[ -f "$2" ]] || { echo -e "\nError: $2 not found"; exit 1; }

# convert GIFs
./convert_gifs.sh $1 $2

sprite_dir=$(basename "$(dirname "$1")")

# build image if not built
IMAGE_NAME="xscreensaver-build"
if [[ -z $(docker images -q $IMAGE_NAME) ]]; then
    echo -e "\nImage '$IMAGE_NAME' not found. Building..."
    docker build -q -t $IMAGE_NAME .
fi

# compile binary in container
echo -e "\nCompiling $sprite_dir binary in container..."
docker run --rm -v $(pwd)/sprites:/xscreensaver/hacks/sprites $IMAGE_NAME make -s -C hacks/sprites sprite_dir=$sprite_dir
chown -R $SUDO_UID:$SUDO_GID sprites

# install binary
BIN_DIR="/usr/libexec/xscreensaver"
echo -e "\nInstalling binary to $BIN_DIR..."
cp sprites/spritesaver $BIN_DIR

# restart xscreensaver
echo -e "\nRestarting xscreensaver to apply changes..."
xscreensaver-command -restart > /dev/null 2>&1

echo -e "\nInstallation success!"