rkdeveloptool gives you a simple way to read/write rockusb device.let's start.

compile and install
1. install libusb and libudev
	sudo apt-get install libudev-dev libusb-1.0-0-dev dh-autoreconf
2. go into root of rkdeveloptool
3. ./autogen.sh
4. ./configure
5. make

rkdeveloptool usage,input "rkdeveloptool -h" to see

When multiple Rockusb boards are connected, select the target board with the
0-based board index option. Use "ld" to list the connected boards and their
indexes.

example:
1.download kernel.img
sudo ./rkdeveloptool db RKXXLoader.bin        //download usbplug to device
sudo ./rkdeveloptool wl 0x8000 kernel.img     //0x8000 is base of kernel partition,unit is sector.
sudo ./rkdeveloptool rd                       //reset device

2.download kernel.img to the second connected board
sudo ./rkdeveloptool ld                       //list boards and indexes
sudo ./rkdeveloptool -i 1 db RKXXLoader.bin   //select board index 1
sudo ./rkdeveloptool -i 1 wl 0x8000 kernel.img
sudo ./rkdeveloptool -i 1 rd

compile error help
if you encounter the error like below:
./configure: line 4269: syntax error near unexpected token `LIBUSB1,libusb-1.0'
./configure: line 4269: `PKG_CHECK_MODULES(LIBUSB1,libusb-1.0)'

You should install pkg-config libusb-1.0:
	sudo apt-get install pkg-config libusb-1.0
