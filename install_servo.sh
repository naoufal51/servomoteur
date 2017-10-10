#!/usr/bin/sudo /bin/bash
sudo apt-get install libusb-dev
sudo apt-get install unzip
wget "https://github.com/naoufal51/servomoteur/archive/master.zip"
unzip master.zip
cd servomoteur-master/
tar -xf libphidget_2.1.8.20151217.tar.gz
cd libphidget-2.1.8.20151217/
./configure
make
make install
cp /udev/99-phidgets.rules /etc/udev/
cd examples
make manager
cd
cd servomoteur-master/
make
