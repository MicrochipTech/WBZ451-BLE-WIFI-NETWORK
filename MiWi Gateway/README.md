# MiWi-Gateway

This is the MiWi Gateway that collects network data and streams it to a Python Dashboard.

The MiWi Gateway project is configured as a gateway between two MiWi PAN Coordinators and the Dashboard software running on a PC or laptop. 
The PC or Laptop connection is made using the micro-USB connector on the ATSAMD21 Nano Curiosity Board. This serial port is using SERCOM 5 on the SAMD21.<br><br>
PAN Coordinator A (Mikro BUS socket 1) is setup with a PAN ID of 0x2017, CH 20 and passes data packets to the SAMD21 over SERCOM 0.<br>
PAN Coordinator B (Mikro BUS socket 2) is setup with a PAN ID of 0x3030, CH 25 and passes data packets to the SAMD21 over SERCOM 3.<br>
The data from SERCOM 0 and SERCOM 3 feed into a circular buffer that collects the data.<br><br>
Timer TC3 is setup for 100mS ticks. Every 200mS the timer checks for data from the circular buffer to transmit to the MiWi network.
If an LED image on the Dashboard is clicked a message is sent to the corresponding PAN Coordinator. The message is then sent to the end node to control the LED. This function works similar to the Android App in the way it controls the LED of the end node.  Only one LED can be on at a time.

The message format is: <br>
&nbsp;&nbsp;&nbsp;&nbsp; **UP,3030,9E956EFFFE40EF84,LR1``<CR><LF>``** <br>

    UP:                Unicast Packet <br>
    3030:              PAN ID <br>
    9E956EFFFE40EF84:  End Node’s Long Address <br>
    L:                 LED control <br>
    R:                 Red LED (G=Green, B=Blue) <br>
    1:                 LED ON command (OFF=0) <br>
    <CR>:              Carriage return <br>
    <LF>:              Line Feed <br>


# RNBD451 Add-On Board
  To program the RNBD451 Add-On Board (EV25F14A) requires that a two pin 0.1” header be soldered to the bottom of the PCB. On the bottom of the PCB are two gold pads labeled SWDCLK & SWDIO. By laying the header down and turning slightly the pins can line up to the two pads. Place the jumper to “Mikro bus power”.
  
  ![RNBD451 Add On](https://github.com/user-attachments/assets/aa759932-2a4e-45e3-a1f1-211a59960dc3)

Using an ICD4 and the Debugger Adapter Board(AC102015) you can easily connect to the RNBD451 Add-On board using jumper wires (Dupont wires). 

  ![ICD4 Programming](https://github.com/user-attachments/assets/c5936575-c785-4d18-a941-35f65c142c31)

Program one of the RNBD451 Add-On Boards with the PAN Coordinator code configured for PAN ID 2017 and CH 20. The second RNBD451 Add-On Board with the PAN Coordinator code configured for PAN ID 3030 and CH 25. Label each module and ensure they are plugged into the correct Mikroe BUS socket for proper operation.


# RNBD350 Add-On Board

  ![RNBD350 Add On](https://github.com/user-attachments/assets/5914905f-6fce-494a-94c7-60876fb17ec0)

NOTE: The RNBD350 Add-On Board (EV65F15A) has been released and has a programming header (J8) available which would make this task easier. This has not been tested yet so no promises here.

# The IoT1 Gateway

The IoT1 Gateway is comprised of four evaluation boards – <br>

	1 – Curiosity Nano Base for Click Boards (AC164162) 
	2 – RNBD451 Add-On Board (EV25F14A) programmed with PAN ID 2017, CH 20. 
	3 – RNBD451 Add-On Board (EV25F14A) programmed with PAN ID 3030, CH 25. 
	4 – SAMD21 Curiosity Nano Board (DM320119) 

*The completed Gateway*
   ![SAMD21 Gateway Board](https://github.com/user-attachments/assets/d22894fa-931a-44a9-918a-a9801a6cbca1)

**Note:** *For the Gateway to accept the End Node data it must be in the proper format.* <br><br>
Each packet sent from the End Node should be formatted as follows - <br>

&nbsp;&nbsp;&nbsp;&nbsp; **0, 9E956EFFFE40FE6C, 3030, Node_1, 100, 36.1, 1, 0, 0, 0, ``<CR><LF>``** <br>

    0:                 Connection table index
    9E956EFFFE40FE6C:  Long Address 
    3030:              PAN ID 
    Node_1:            Node Name 
    100:               Moisture level 
    36.1:              Temperature 
    1:                 Red LED status (1=ON, 0=OFF)
    0:                 Green LED status
    0:                 Blue LED status
    0:                 Switch status (0=Released, 1=Pressed) 
    <CR>
    <LF>

# Windows Laptop Python Dashboard

To enable the classroom to see the data received from the Gateway I have written a Python application. The application is a grid arrangement that shows each End Node’s information. The rows are alternating colors – light grey and light blue. When a Node sends an update the row will blink light green. If a Node does not send an update within 30 seconds, the row will change to a light red color. If the Node has not sent an update within another 90 seconds, the row is deleted. 
The LED columns display the status of the End Node. You can click on one of them and send a message back to the End Node to change the LED color. This is a toggle function so only one LED color is allowed.
The Switch Status displays the status of the user switch on the End Node. Grey color for released and orange color for pressed.

The data format in each End Node row – <br>
 
    1 – Row: 		sequential number assigned when the End Node joins 
    2 – Name: 		name assigned to the End Node 
    3 – PAN ID: 		PAN ID associated with e End Node
    4 – Temperature: 	End Node temperature in degrees Celsius
    5 – Moisture:		End Node moisture level (0% to 100%)
    6 – Red LED:		End Node red LED status/control
    7 – Green LED:		End Node green LED status/control
    8 – Blue LED:		End Node blue LED status/control
    9 – Switch Status:	End Node switch status

*Dashboard screenshot –* <br>
  ![Dashboard Screen Shot](https://github.com/user-attachments/assets/3388cefa-4900-4b19-afcf-09be7fb81170)

**Note:** *The Dashboard is supplied as Python code, to run this I use Thonny. The first time running this might require some libraries to be installed such as PyQT5 and pySerial. The serial port is defined toward the bottom of the code, be sure to set this to the proper COM port on your computer.* <br><br>
