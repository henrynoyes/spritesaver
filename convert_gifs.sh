#!/bin/bash

# Convert two GIF files to C header files
# Usage: ./convert_gifs.sh /path/to/left.gif /path/to/right.gif
# Note: Both GIFs must have the same parent folder

set -e

# check args
[[ $# -eq 2 ]] || { echo -e "\nUsage: $0 /path/to/left.gif /path/to/right.gif"; exit 1; }
[[ -f "$1" ]] || { echo -e "\nError: $1 not found"; exit 1; }
[[ -f "$2" ]] || { echo -e "\nError: $2 not found"; exit 1; }

# check sprite dir
left_dir=$(dirname "$1")
right_dir=$(dirname "$2")
[[ "$left_dir" == "$right_dir" ]] || { echo -e "\nError: Both GIF files must be in the same directory, $left_dir ≠ $right_dir"; exit 1; }

# probe frame counts independently
num_left_frames=$(ffprobe -v quiet -select_streams v:0 -count_frames -show_entries stream=nb_read_frames -of csv=p=0 "$1")
num_right_frames=$(ffprobe -v quiet -select_streams v:0 -count_frames -show_entries stream=nb_read_frames -of csv=p=0 "$2")

left_pad=${#num_left_frames}
right_pad=${#num_right_frames}

echo -e "\nEntering $left_dir..."
cd "$left_dir"

left_name=$(basename "$1" .gif)
right_name=$(basename "$2" .gif)

# create directory
mkdir -p sprite_images

# convert GIFs to PNGs
ffmpeg -v quiet -i "${left_name}.gif"  "sprite_images/${left_name}_%0${left_pad}d.png"  -y
ffmpeg -v quiet -i "${right_name}.gif" "sprite_images/${right_name}_%0${right_pad}d.png" -y

echo -e "\nSuccessfully converted GIFs to PNGs"

# create master header
cat > sprites.h << EOF
#ifndef SPRITES_H
#define SPRITES_H

#define NUM_LEFT_FRAMES  $num_left_frames
#define NUM_RIGHT_FRAMES $num_right_frames

EOF

left_entries=""
right_entries=""

# add includes
for i in $(seq -w 1 $num_left_frames); do
    echo "#include \"sprite_headers/${left_name}_${i}_png.h\"" >> sprites.h
    left_entries+="{${left_name}_${i}_png, sizeof(${left_name}_${i}_png)},\n"
done

for i in $(seq -w 1 $num_right_frames); do
    echo "#include \"sprite_headers/${right_name}_${i}_png.h\"" >> sprites.h
    right_entries+="{${right_name}_${i}_png, sizeof(${right_name}_${i}_png)},\n"
done

# add arrays
cat >> sprites.h << EOF

static struct {
    const unsigned char *data;
    unsigned long size;
} left_frame_data[NUM_LEFT_FRAMES] = {
$(echo -e "$left_entries")};

static struct {
    const unsigned char *data;
    unsigned long size;
} right_frame_data[NUM_RIGHT_FRAMES] = {
$(echo -e "$right_entries")};

EOF

echo "#endif" >> sprites.h
echo -e "\nSuccessfully generated master header file: ${left_dir}/sprites.h"