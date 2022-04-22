<p align="center">
	<a href="https://github.com/lboroWMEME-TeamProject/CCC-ProjectDocs"><img src="https://i.imgur.com/VwT4NrJ.png" width=650></a>
	<p align="center"> This repository is part of  a collection for the 21WSD001 Team Project. 
	All other repositories can be access below using the buttons</p>
</p>


<p align="center">
	<a href="https://github.com/lboroWMEME-TeamProject/CCC-ProjectDocs"><img src="https://i.imgur.com/rBaZyub.png" alt="drawing" height = 33/></a> 
	<a href="https://github.com/lboroWMEME-TeamProject/Dashboard"><img src="https://i.imgur.com/fz7rgd9.png" alt="drawing" height = 33/></a> 
	<a href="https://github.com/lboroWMEME-TeamProject/Cloud-Server"><img src="https://i.imgur.com/bsimXcV.png" alt="drawing" height = 33/></a> 
	<a href="https://github.com/lboroWMEME-TeamProject/Drone-Firmware"><img src="https://i.imgur.com/yKFokIL.png" alt="drawing" height = 33/></a> 
	<a href="https://github.com/lboroWMEME-TeamProject/Simulated-Drone"><img src="https://i.imgur.com/WMOZbrf.png" alt="drawing" height = 33/></a>
</p>

<p align="center">
	Below you can find buttons that link you to the repositories that host the code for the module itself. These can also be found linked in the collection repository: <a href="https://github.com/lboroWMEME-TeamProject/Drone-Firmware">Drone Firmware</a>. 
</p>


<p align="center">
	<a href="https://github.com/lboroWMEME-TeamProject/Main-Pi"><img src="https://i.imgur.com/4knNDhv.png" alt="drawing" height = 33/></a> 
	<a href="https://github.com/lboroWMEME-TeamProject/EnviroSensor"><img src="https://i.imgur.com/lcYUZBw.png" alt="drawing" height = 33/></a> 
	<a href="https://github.com/lboroWMEME-TeamProject/Geiger-Counter"><img src="https://i.imgur.com/ecniGik.png" alt="drawing" height = 33/></a> 
	<a href="https://github.com/lboroWMEME-TeamProject/Thermal-Camera"><img src="https://i.imgur.com/kuoiBTc.png" alt="drawing" height = 33/></a> 
	<a href="https://github.com/lboroWMEME-TeamProject/ai-cam"><img src="https://i.imgur.com/30bEKvR.png" alt="drawing" height = 33/></a>
</p>

------------

# Geiger-Counter

This repository contains code that uses a Raspberry Pi Pico to interface with the MightyOhm Geiger Counter to pull data from the Geiger Counter and send it over I2C to the Main Pi.

------------

## Table of Contents

- [Subsystem Overview](#Subsystem-Overview)
    - [Wiring Diagram](Wiring-Diagram)
- [Code Overview](#Code-Overview)
	- [Registers](#Registers)
	- [Event Loops](#Event-Loops)
- [Test Plan](#Test-Plan)
- [Installation](#Installation)
	- [Prerequisites](#Prerequisites)
- [Deployment](#Deployment)

------------

## Subsystem Overview

The Mighty Ohm Geiger Counter Operates using UART serial communication and outputs the radiation levels in uSv, CPS and CPM however, we do not need all that data. The Raspberry Pi Pico reads the data from the UART connection and extracts only the data the system needs and passes that along to the Main Pi using a I2C connection.

**Subsystem Diagram :**

<p align="center">
	<img src="https://i.imgur.com/x1AuBI3.jpg" alt="drawing"/>
</p>



### Wiring Diagram
<p align="center">
	<img src="https://i.imgur.com/XkVusI3.jpg" alt="drawing"/>
</p>

------------

## Code Overview

The Raspberry Pico acts as an I2C Slave when connected, this slave has the default address **0x4D**. It has 2 16 bit registers in memory to store and access the data from the geiger counter.

## Registers

### 0x00

The first register at location **0x00** holds the device address of the connected I2C Slave

This is sent as 2 I2C Data packets when accessed however the first 8 bits can be ignored as I2C slaves can only take 8 bit address values.
<p align="center">
	<table align="center">
		<thead align="center">
			<tr align="center">
				<th align="center">Bit 15</th>
				<th align="center">Bit 14</th>
				<th align="center">Bit 13</th>
				<th align="center">Bit 12</th>
				<th align="center">Bit 11</th>
				<th align="center">Bit 10</th>
				<th align="center">Bit 09</th>
				<th align="center">Bit 08</th>
			</tr>
		</thead>
		<tbody align="center">
			<tr align="center">
				<td align="center">Reserved</td>
				<td align="center">Reserved</td>
				<td align="center">Reserved</td>
				<td align="center">Reserved</td>
				<td align="center">Reserved</td>
				<td align="center">Reserved</td>
				<td align="center">Reserved</td>
				<td align="center">Reserved</td>
			</tr>
		</tbody>
		<thead align="center">
			<tr>
				<th align="center">Bit 07</th>
				<th align="center">Bit 06</th>
				<th align="center">Bit 05</th>
				<th align="center">Bit 04</th>
				<th align="center">Bit 03</th>
				<th align="center">Bit 02</th>
				<th align="center">Bit 01</th>
				<th align="center">Bit 00</th>
			</tr>
		</thead>
		<tbody align="center">
			<tr>
				<td align="center">DEVICE_ADDRESS.07</td>
				<td align="center">DEVICE_ADDRESS.06</td>
				<td align="center">DEVICE_ADDRESS.05</td>
				<td align="center">DEVICE_ADDRESS.04</td>
				<td align="center">DEVICE_ADDRESS.03</td>
				<td align="center">DEVICE_ADDRESS.02</td>
				<td align="center">DEVICE_ADDRESS.01</td>
				<td align="center">DEVICE_ADDRESS.00</td>
			</tr>
		</tbody>
	</table>
</p>

### 0x01

The second register at location **0x01** holds the CPM from the Geiger counter

This is sent as 2 I2C Data packets when accessed the first data packet contains bits 15:8 of the CPM and the second data packet contains bits 7:0 of the CPM value.

When the data is received you must reconstruct the data packets to get the actual reading of teh geiger counter.

`geiger.py` contains code that allows you to reconstruct the bits as well as initialise the I2C geiger counter.

<p align="center">
	<table align="center">
		<thead>
			<tr>
				<th align="center">Bit 15</th>
				<th align="center">Bit 14</th>
				<th align="center">Bit 13</th>
				<th align="center">Bit 12</th>
				<th align="center">Bit 11</th>
				<th align="center">Bit 10</th>
				<th align="center">Bit 09</th>
				<th align="center">Bit 08</th>
			</tr>
		</thead>
		<tbody>
			<tr>
				<td align="center">CPM.15</td>
				<td align="center">CPM.14</td>
				<td align="center">CPM.13</td>
				<td align="center">CPM.12</td>
				<td align="center">CPM.11</td>
				<td align="center">CPM.10</td>
				<td align="center">CPM.09</td>
				<td align="center">CPM.08</td>
			</tr>
		</tbody>
		<thead>
			<tr>
				<th align="center">Bit 07</th>
				<th align="center">Bit 06</th>
				<th align="center">Bit 05</th>
				<th align="center">Bit 04</th>
				<th align="center">Bit 03</th>
				<th align="center">Bit 02</th>
				<th align="center">Bit 01</th>
				<th align="center">Bit 00</th>
			</tr>
		</thead>
		<tbody>
			<tr>
				<td align="center">CPM.07</td>
				<td align="center">CPM.06</td>
				<td align="center">CPM.05</td>
				<td align="center">CPM.04</td>
				<td align="center">CPM.03</td>
				<td align="center">CPM.02</td>
				<td align="center">CPM.01</td>
				<td align="center">CPM.00</td>
			</tr>
		</tbody>
	</table>
</p>

------------

## Event Loops

The Program is dependent on interrupts and the main loop does nothing after setting up the peripherals.

There are two event loops in the program, the UART Loop and the I2C Loop.

The I2C Loop takes priority over the UART Loop.

### UART Loop

<p align="center">
	<img src="https://i.imgur.com/RwyUM4n.jpg" alt="drawing"/>
</p>

### I2C Loop

<p align="center">
	<img src="https://i.imgur.com/PZDLVok.jpg" alt="drawing"/>
</p>


------------

## Test Plan


<div align="center">

|Objective|Testing Strategy|Expected Output|Current Output|Pass/Fail|
|--|--|--|--|:--:|
|Geiger counter functions as expected independently.|Connect Geiger directly to computer through serial to USB and view displayed output data (CSV) when both in range and out of range (i.e. background radiation) of test sample. Listen for hum of high voltage tube. View LED flash.|Output data printed on screen corresponds to expected values when in range of (CPM) and out of range of (background radiation: 10 - 20 CPM) test sample. High voltage tube should emit hum, LED should light up.|Correct data is displayed. Hardware reacts as expected.|:heavy_check_mark:|
|Geiger counter sends data to Raspberry Pi Pico.|Connect Geiger counter to Raspberry Pi Pico through UART and view data output in Thonny when in and out of range of test sample.|Thonny window should display expected sample and background radiation values.| Correct data is displayed.|:heavy_check_mark:|
|Raspberry Pi Pico sends CPM to the Raspberry Pi 4 (master).| Obtain values from I2C bus and check the data correlates with the data collected from previous two tests.|Correct data should display in C program window.|Correct data is displayed.|:heavy_check_mark:|
|Correct data is being sent to GUI.|Observe data is bring printed in GUI. Ensure data corresponds with previously collected data to calibrate against.|Data on GUI should appear and correlate with expected results.|Correct data is displayed.|:heavy_check_mark:|

</div>

------------

## Installation

This program is written in C for the Raspberry Pico. You can compile the code using the supplied makefile however, you must make sure you have the required toolchains and environment variables installed don your local machine.

### Prerequisites

- [Pico C/C++ SKD](https://github.com/raspberrypi/pico-sdk)
- [GNU Embedded Toolchain for Arm](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain)
- [CMake](https://cmake.org/)
- [VScode](https://code.visualstudio.com/)
- [CMake Tools for VScode](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
- [Python 3.x](https://www.python.org/)
- [Git](https://git-scm.com/)

if you're on windows you will also need:

- [MinGW-w64](https://www.mingw-w64.org/)


Once all the required tool chains are installed and configured open the repository in VScode.

A full guide on hot to get the tool chains set up can be found on the [Getting Started With Raspberry Pi Pico](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf) documentation written by Raspberry Pi Foundation.

Once opened in VScode CMake Tools will automatically configure the project.

You can edit the `CMakeLists.txt` files to add or remove compilation files/libraries.

Once you have made the changes you want, you can compile using CMake. It will create a `.uf2` file with the specified settings in the `build/` directory that you can use and upload to the Pico.


------------

## Deployment

Once you have compiled the code you will end up with a `.uf2` file. This contains the binary code that needs to be uploaded to the Raspberry Pico. 

This repository contains a `.uf2` file with the default settings in the `build/` directory that you can use.

If you need to change options you need to recompile the code, refer to the [Installation](#installation) section for instructions on how to do that.

**Step 1:** Plug in the Raspberry Pico whilst holding down the BOOTSEL button.

**Step 2:** Navigate to the Pico file directory

**Step 3:** Copy over the `.uf2` file from the `build/` directory. 

Now once you have connected the device as the [Wiring Diagram](#wiring-diagram) your I2C device should be accessible and outputting data when requested.

------------

