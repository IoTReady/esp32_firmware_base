#!/bin/bash

# exit on error
set -e

echo -e 'Updating software from git\n'

git stash
git pull

echo -e 'Preparing new device\n'

# -p so there's no error if dir exists
mkdir -p aws_credentials

# Ensure we are in the local venv

source venv/bin/activate

python3 registerDevice.py

# Edit this to the path to esp-idf on your system
# This brings idf.py into your path
source ~/esp/esp-idf/export.sh

idf.py -C source build flash

echo
echo "=========================="
echo
echo "MAC Address:"
echo
cat mac_address.txt
echo
echo "=========================="
echo
echo -e "All done! On to the next one :-)\n"


