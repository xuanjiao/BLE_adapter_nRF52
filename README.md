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

2.  Import the project to VS Code.

3. Build the application in VS code using mBed and generate the .hex file.

4. Use Jlink FlashLite to erase the MCU and flash the .hex file.

5. Use Android App nRF Connect scan BLE devices

6.  Use RTT Viewer to monitor information from MCU.


## Sensor information

[CC2560 sensorTag](https://processors.wiki.ti.com/index.php/CC2650_SensorTag_User's_Guide)
contains a light sensor to measure the light and a movement sensor to detect the door status (open and close)

-   Light sensor [OPT3001](http://www.ti.com/lit/ds/symlink/opt3001.pdf)

-   Movement sensor [MPU-9250](https://invensense.tdk.com/products/motion-tracking/9-axis/mpu-9250/)

