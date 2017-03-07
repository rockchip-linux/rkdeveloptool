rkDevelopTool gives you a simple way to read/write rockusb device.let's start.

compile and install
1 download libusb source from https://github.com/libusb/libusb.git
2 go into root of libusb
3 ./autogen.sh (libudev must be installed by apt-get install libudev1 or run cmd "./configure --disable-udev")
4 make && make install
5 go into root of rkDevelopTool
6 make && make install

rkDeveloptool usage,input "rkDevelopTool -h" to see

example:
1.download kernel.img
./rkDevelopTool db RKXXLoader.bin     //download usbcode to device
./rkDevelopTool wl 0x8000 kernel.img //0x8000 is base of kernel partition,unit is sector.
./rkDevelopTool rd                   //reset device