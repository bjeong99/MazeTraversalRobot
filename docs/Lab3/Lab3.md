#Lab 3
## Monday 11/4/2019

In this lab, we used a parallel interface to connect the arduino to a DE0-NANO FPGA board in order to display an arduino input on the monitor using a VGA connection between the monitor and the FPGA. Below includes the connection between the base station's arduino and the FPGA, and the drawing on the monitor based on input from arduino to FPGA. 

***1. Connection between Base Station's Arduino and FPGA***

In order to connect the arduino to the FPGA, we knew that we needed to create a voltage divider using resistances. The arduino outputs a 5V signal on high and the FPGA digital logic occurs at 3.3V. If we directly connected the arduino pins to the FPGA GPIO_1 pins, we would fry the FPGA. We knew that we needed 9 connections ( 8 for input color from arduino, 1 for the clock signal). We obtained 9 1.8 kilo Ohm resistors and 9 1 kilo Ohm resistors, and created a voltage divider on each output pin of the arduino. We 

In order to obtain this connection, we first downloaded the Lab 4 Template from the course website. This code included the DE0-NANO.v, IMAGE_PROCESSOR.v, Dual_Port_RAM_M9K.v, and the VGA_DRIVER.v. From this base code, the DE0-NANO.v code was the top level module which instantiated IMAGE_PROCESSOR.v, Dual_Port_RAM_M9K.v, and VGA_DRIVER.v. After downloading this, we focused on instantiating the clock using the pll tool from IP Catalog. 



To do this, we went to *Tools>IP Catalog>Library>Basic Functions>Clocks; PLLs; and Resets>PLL* 
This opened a setup window, where we set our input clock (inclk0) to 50 MHz, and instantiated c0_sig, c1_sig, and c2_sig, three clocks, c0 had a 24 MHz frequency, c1 had a 25 MHz frequency, and c2 had a 50 MHz frequency. We then instantiated the module in our DE0-NANO.v code and replaced all CLOCK_50 modules with c2_sig and all 25 MHz clocks with c1_sig. Running this function 
