#!/bin/bash

# path to the assets folder containing the .ttf files
ASSETS_DIR="./assets"
FONT_NAME1="JunicodeBold.ttf"
FONT_NAME2="MagicSchholOne.ttf"

# path to the system fonts directory
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    FONT_DIR="$HOME/.fonts"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    FONT_DIR="$HOME/Library/Fonts"
else
    echo "Unsupported operating system. Exiting."
    exit 1
fi

# check if the .ttf files exists
if [[ ! -f "$ASSETS_DIR/$FONT_NAME1" ]]; then
    echo "Font file $FONT_NAME1 not found in $ASSETS_DIR. Exiting."
    exit 1
fi
if [[ ! -f "$ASSETS_DIR/$FONT_NAME2" ]]; then
    echo "Font file $FONT_NAME2 not found in $ASSETS_DIR. Exiting."
    exit 1
fi

# copy font 1
cp "$ASSETS_DIR/$FONT_NAME1" "$FONT_DIR"
# refresh cache
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    fc-cache -f -v
fi

# copy font 2
cp "$ASSETS_DIR/$FONT_NAME2" "$FONT_DIR"
# refresh cache
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    fc-cache -f -v
fi

# compile code
cd sockpp/ || exit
mkdir cmake-build-debug
cd cmake-build-debug || exit
cmake ..
make
cd ../..
mkdir cmake-build-debug
cd cmake-build-debug || exit
cmake ..
make