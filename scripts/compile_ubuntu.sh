#!/bin/bash

# INSTRUCTIONS
# run this file from the wizard directory
# make executable (if necessary) with chmod +x scripts/compile_ubuntu.sh
# run with ./scripts/compile_ubuntu.sh


# path to the assets folder containing the .ttf files
ASSETS_DIR="./assets"
FONT_NAME1="JunicodeBold.ttf"
FONT_NAME2="MagicSchoolOne.ttf"

# create new directory for wizard fonts if it doesn't exist yet
if [ ! -d "~/.fonts" ]; then
    echo "Directory ~/.fonts does not exist. Creating it now..."
    mkdir ~/.fonts
    echo "Directory ~/.fonts created successfully."
else
    echo "Directory ~/.fonts already exists."
fi

# copy the fonts into the new directory
cp $ASSETS_DIR/$FONT_NAME1 ~/.fonts
cp $ASSETS_DIR/$FONT_NAME2 ~/.fonts
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
