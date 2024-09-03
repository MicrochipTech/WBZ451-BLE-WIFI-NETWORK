# MiWi-Dashboard

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

