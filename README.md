# General
 
rkdeveloptool gives you a simple way to read/write rockusb device

## Compile and install

1) Install *libusb* and *libudev*
```
sudo apt-get install libudev-dev libusb-1.0-0-dev dh-autoreconf
```
2) Download rkdeveloptool
```
git clone https://github.com/rockchip-linux/rkdeveloptool.git
```
3) Go into root of rkdeveloptool
```
cd rkdeveloptool
```
4) Generate *aclocal.m4*
```
aclocal
```
5) Update the GNU Autotools build system
```
autoreconf -i
```
6) Generate *config.h.in* file
```
autoheader
```
7) Add missing auxiliary files
```
automake --add-missing
```
8) Prepare the project for compilation
```
./configure
```
9) Invoke the build process
```
make
```

## Usage

To see the help information and available options for the rkdeveloptool command input
```
rkdeveloptool -h
```

## Example:

### Download kernel.img

```
sudo ./rkdeveloptool db RKXXLoader.bin    //download usbplug to device
sudo ./rkdeveloptool wl 0x8000 kernel.img //0x8000 is base of kernel partition,unit is sector.
sudo ./rkdeveloptool rd                   //reset device
```

## Compile error help

If you encounter the error like below:

./configure: line 4269: syntax error near unexpected token `LIBUSB1,libusb-1.0'

./configure: line 4269: `PKG_CHECK_MODULES(LIBUSB1,libusb-1.0)'

You should install *pkg-config* and *libusb-1.0*:
```
sudo apt-get install pkg-config libusb-1.0 
```


