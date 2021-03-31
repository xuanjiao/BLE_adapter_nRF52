# BLE adapter nRF52 (2019)

A customized BLE adapter program. It communicates with sensor modules via BLE, requests the sensor data, and transmits data to the gateway.

The Raspberry Pi Gateway needs a BLE adapter because it can pair the not BLE module and read the characteristic. Link to the Raspberry Pi gateway program: [Raspberry Pi Gateway](https://github.com/xuanjiao/ble_gateway)


## Requirements

[nRF52 DK](https://www.nordicsemi.com/Software-and-Tools/Development-Kits/nRF52-DK)

[Microsoft Studio Code](https://code.visualstudio.com/) and its C/C++ extensions.

[Command line tool Mbed Cli](https://github.com/ARMmbed/mbed-cli/blob/1.8.3/README.md) 

[GCC-ARM compiler](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)

[GNU Arm Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm) 
 
[Make-3.81](https://sourceforge.net/projects/gnuwin32/files/make/3.81/) 

[Jlink FlashLite V6.54](https://www.segger.com/products/debug-probes/j-link/technology/flash-download/)

[RTT Viewer](https://www.segger.com/products/debug-probes/j-link/tools/rtt-viewer/).

[nRF Connect](https://www.nordicsemi.com/Software-and-tools/Development-Tools/nRF-Connect-for-mobile)

# Installation
```
git clone https://github.com/xuanjiao/ble_gateway.git
```

# Usage

1.  Change the system variable PATH for make tool.

2. Build the application and generate the .hex file.

3. Use Jlink FlashLite to erase the MCU and flash the .hex file.

4. Use Android App nRF Connect scan BLE devices

5.  Use RTT Viewer to monitor information from MCU.

