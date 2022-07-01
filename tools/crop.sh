#!/usr/bin/bash

# This script generates our landscape sprites.

for filename in *.png; do
    # This takes a really long time...
    result=$(convert $filename -channel a -separate -scale 1x1! -format "%[fx:mean]\n" info:)
    if [[ $result == "0" ]]; then
        echo "Blank! Removing..."
        rm $filename
    fi
done

# Create our spritesheet.
montage * -tile x32 -geometry 32x32 -background none sprites-2.png && mv sprites-2.png ../sprites-2.png
