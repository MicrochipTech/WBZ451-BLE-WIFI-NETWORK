# MiWi Sensor Network with BLE Commissioning - End Node

This project provides the end node sensor device in the MiWi + BLE Sensor Network solution.  The project will read the hydro probe click board, temp sensor, and switch state, and transmit that data over BLE and MiWi (if connected).  

## Installing / Getting started

Hardware Requirements:

WBZ451 Curiosity Board - https://www.microchip.com/en-us/development-tool/EV96B94A

Hydro Probe Click Board - https://www.mikroe.com/hydro-probe-click

Mikroe Shuttle Click - https://www.mikroe.com/shuttle-click

MikroBus Shuttle - https://www.mikroe.com/mikrobus-shuttle

The Hydro Probe Click Board is not essential to the operation of the project, the sensor data will always show 100% moisture in that case.

### Software Requirements:

MPLAB X v6.20 (or later) https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide

XC32 Compiler v4.40 or v4.45 - https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers/xc32

### Initial Configuration

The name for the device can be found:
 >app.c - line 87
This name appears in the advertisement packet for BLE and is also the device name transmitted over the MiWi connection.  When connecting multiple nodes to one or more PAN coordinators, it is advised to make unique names for each board that is programmed.

## Running the Demo

Once the board has been programmed, the device will begin advertising over BLE.  Using the Android app provided in this repo or using another BLE app such as LightBlue, the user can connect to the device.  The readme in the Android app folder has further instructions on how to use that app to connect to the device over BLE, search for MiWi Networks, connect to a MiWi Network, read the values from the temp sensor, hydro probe click, and user switch, as well as control the RGB LED on the curiosity board.

If using the LightBlue app, the below commands can be issued using the second characteristic listed in the LightBlue app (UUID ends in 9bb3).  The first characteristic (UUID ends in 9616) can be subscribed to in order to get the hydro probe, temp sensor, and switch values.

Once connected to the PAN coordinator, pressing the user switch on that board will send out a broadcast message that cycles through all the LED commands, toggling each color of the RGB LED.

### Command Interface

The command structure for the end node is quite simple, just a few ASCII characters are used for the different commands.

LED commands - These commands all begin with 'L' followed by the color ('R', 'G', or 'B') and terminated with a '0' or '1' to signify OFF/ON.  For example, the command "LB1" would turn on the blue LED.

Network commands - These commands all begin with 'N'.  To search for MiWi networks, the command "NF" is used (network find).  After one or more MiWi networks have been found, the commands "N0", "N1", etc. will select the network to connect to with "N0" being the first network found, "N1" being the second, etc.

### Debug Output

The project does make use of the USB serial port on the WBZ451 Curiosity Board to send debug info.  The WBZ451 Curiosity Board will enumerate as two USB COM devices, so make sure you select the USB COM device and not the PICKit 4 On Board Virtual COM in your terminal program of choice.  The project uses 115200 8N1 on the serial port.

The debug output will print out the current moisture and temperature readings, echo commands sent over BLE, information about MiWi packets sent and received, and error codes generated if a MiWi or BLE packet failed to send.

