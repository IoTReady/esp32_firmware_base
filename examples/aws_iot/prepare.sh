#!/bin/bash

# exit on error
set -e

echo -e 'Updating software from git\n'

git stash
git pull

echo -e 'Preparing new device\n'

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


