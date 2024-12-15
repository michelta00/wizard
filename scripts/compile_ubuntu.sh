#!/bin/bash

# path to the assets folder containing the .ttf files
ASSETS_DIR="./assets"
FONT_NAME1="JunicodeBold.ttf"
FONT_NAME2="MagicSchoolOne.ttf"

# create new directory for wizard fonts if it doesn't exist yet
if [ ! -d "~/.local/share/fonts" ]; then
    echo "Directory ~/.local/share/fonts does not exist. Creating it now..."
    mkdir ~/.local/share/fonts
    echo "Directory ~/.local/share/fonts created successfully."
else
    echo "Directory ~/.local/share/fonts already exists."
fi

# copy the fonts into the new directory
cp $ASSETS_DIR/$FONT_NAME1 ~/.local/share/fonts
cp $ASSETS_DIR/$FONT_NAME2 ~/.local/share/fonts

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
