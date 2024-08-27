# MiWi Sensor Network with BLE Commissioning
> A solution providing a BLE interface to a user for scanning for and connecting to MiWi networks.

One of the challenges of 802.15.4 networks can be commissioning devices on the network and 
providing feedback to an end user on what is happening in the process.  We have provided an example
implementation that uses the BLE radio on the WBZ451 to interface to a phone to initiate the search
for MiWi networks and to choose a MiWi network to connect to.  The application also provides control
of the LEDs from the BLE and MiWi interface as well as reporting sensor data (temp and moisture) over
the BLE and MiWi interfaces.

## Installing / Getting started

### Hardware Requirements:

WBZ451 Curiosity Board - https://www.microchip.com/en-us/development-tool/EV96B94A

![alt text](https://www.microchip.com/content/dam/mchp/mrt-dam/products/wsg/220209-wsg-photo-ev96b94a-front-transparent.jpg)

Hydro Probe Click Board - https://www.mikroe.com/hydro-probe-click

![alt text](https://cdn1-shop.mikroe.com/img/product/hydro-probe-click/hydro-probe-click-large_default-1.jpg)

Mikroe Shuttle Click - https://www.mikroe.com/shuttle-click

![alt text](https://cdn1-shop.mikroe.com/img/product/shuttle-click/shuttle-click-large_default-1.jpg)

MikroBus Shuttle - https://www.mikroe.com/mikrobus-shuttle

![alt text](https://cdn1-shop.mikroe.com/img/product/mikrobus-shuttle/mikrobus-shuttle-large_default-1.jpg)

NOTE:  The click boards are not essential to the operation of the demo.  The moisture readings will obviously be 
erroneous in that case.

### Software Requirements:

MPLAB X v6.20 (or later) https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide

XC32 Compiler v4.40 or v4.45 - https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers/xc32
