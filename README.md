# rkdeveloptool

`rkdeveloptool` gives you a simple way to read/write rockusb devices.

## Compile and install

1. Install the necessary packages:
```bash
sudo apt-get install pkg-config autoconf libudev-dev libusb-1.0-0-dev
```
2. `cd` into the root folder of rkdeveloptool
3. `./autogen.sh`
4. `./configure`
5. `make`

## Usage

Run `rkdeveloptool -h` for the full list of commands.

Example of downloading `kernel.img`:
```bash
sudo ./rkdeveloptool db RKXXLoader.bin       # Download usbplug to device
sudo ./rkdeveloptool wl 0x8000 kernel.img    # 0x8000 is base of the kernel partition, unit is in sectors
sudo ./rkdeveloptool rd                      # Reset device
```
