#!/usr/bin/bash

# Generates the effect sprite sheet.
#
# Run this in media/images/animations/effects

montage * -tile 1x -geometry +0+0 -background none ../sprites.png
