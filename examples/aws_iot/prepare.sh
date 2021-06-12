#!/bin/bash

# exit on error
set -e

echo -e 'Updating software from git\n'

git stash
git pull

echo -e 'Preparing new device\n'

# -p so there's no error if dir exists
mkdir -p aws_credentials

# Edit this to the path to esp-idf on your system
# This brings idf.py into your path
source ~/code/esp/esp-idf/export.sh

python3 ./registerDevice.py

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


