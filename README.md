# rkdeveloptool

A command-line tool for reading and writing Rockchip devices over USB in
MASKROM or ROCKUSB mode.

## Build

CMake is the recommended way to build. Install the required library and build
tools (a C++11-compatible compiler, CMake, and Ninja) first:

```shell
apt-get install cmake ninja-build libusb-1.0-0-dev pkg-config
```

### CMake

```bash
cmake -B build -S . -G Ninja
cmake --build build
```

The binary is placed at `build/rkdeveloptool`.

### Autotools

Install required dependencies.

```bash
apt-get install dh-autoreconf make
```

Generate `Makefile` and build.

```bash
./autogen.sh
./configure
make
```

## Usage

Most commands require the device to be connected in MASKROM or ROCKUSB mode.

```bash
./rkdeveloptool -h
```

### Example: Flash a Kernel Image

1. Load the bootloader into RAM (device must be in MASKROM mode).
   ```bash
   ./rkdeveloptool db RKXXLoader.bin
   ```
2. Write the kernel to its partition (0x8000 is the sector offset).
   ```bash
   ./rkdeveloptool wl 0x8000 kernel.img
   ```
3. Reset the device.
   ```bash
   ./rkdeveloptool rd
   ```

## Troubleshooting

### USB Device Permissions

To use the tool without `sudo`, install the bundled udev rules.

```bash
cp 99-rk-rockusb.rules /etc/udev/rules.d/
udevadm control --reload-rules
```
