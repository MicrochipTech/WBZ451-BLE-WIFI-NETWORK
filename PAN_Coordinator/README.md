# MiWi Sensor Network with BLE Commissioning - PAN Coordinator

This project provides the PAN coordinator device in the MiWi + BLE Sensor Network solution.  The project will start a MiWi network on the configured channel and PAN ID as a PAN Coordinator.  The PAN coordinator will allow devices to connect to it and report MiWi messages received from other nodes on the network.  Also, the PAN coordinator will send a broadcast message each time the user switch is pressed on the WBZ451.  The content of the message sent will be a single LED command but it changes every switch press, cycling through all the LED commands for the RGB LED.

## Installing / Getting started

Hardware Requirements:

WBZ451 Curiosity Board - https://www.microchip.com/en-us/development-tool/EV96B94A

### Software Requirements:

MPLAB X v6.20 (or later) https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide

XC32 Compiler v4.40 or v4.45 - https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers/xc32

### Initial Configuration

The PAN ID can be set:
 >miwi_config.h (line 103)
The PAN ID is a MAC layer value that will filter connection attempts and traffic out from devices without a matching PAN ID.  The end node searches for networks and retrieves the PAN ID from the search results setting it.

The channel can be set:
>miwi_app.c (line 105)
Channels 20 and 25 have been used for the demo project as they are 802.15.4 channels whose frequencies do not overlap with WiFi channels 1, 6, or 11 AND do not overlap with any of the BLE advertising channels.  PLEASE NOTE: If you use channels other than 20 and 25, you will need to modify the SCAN_CHANNEL_MAP define in the end node project (app_miwi_star.c - line 86) and provide the appropriate 32-bit bitmap value to scan for the specific channels used.

## Running the Demo

Once the board has been programmed, the device will create a MiWi network as a PAN coordinator using the configured channel and PAN ID.  Once MiWi end nodes are connected to the PAN coordinator, pressing the user switch on the PAN coordinator will send broadcast packets out with RGB LED commands.

The PAN coordinator does not have any BLE functionality currently even though the BLE stack has been added to the project.

### Debug Output

The project does make use of the USB serial port on the WBZ451 Curiosity Board to send debug info.  The WBZ451 Curiosity Board will enumerate as two USB COM devices, so make sure you select the USB COM device and not the PICKit 4 On Board Virtual COM in your terminal program of choice.  The project uses 115200 8N1 on the serial port.

The debug output will print out information about MiWi packets sent and received, and error codes generated if a MiWi packet failed to send.

