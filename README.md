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

The Raspberry Pico acts as an I2C Slave when connected, this slave has the default adress **0x4D**. It has 2 16 bit registers in memory to store and access the data from the geiger counter.

## Registers

### 0x00

The first register at location **0x00** holds the device address of the connected I2C Slave

This is sent as 2 I2C Data packets when accessed however the first 8 bits can be ignored as I2C slaves can only take 8 bit address values.
<p align="center">

|||||||||
|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
|15|14|13|12|11|10|09|08|
|res|res|res|res|res|res|res|res|
|07|06|05|04|03|02|01|00|
|DEVICE_ADDRESS.7|DEVICE_ADDRESS.6|DEVICE_ADDRESS.5|DEVICE_ADDRESS.4|DEVICE_ADDRESS.3|DEVICE_ADDRESS.2|DEVICE_ADDRESS.1|DEVICE_ADDRESS.0|

</p>

### 0x01

The second register at location **0x01** holds the CPM from the Geiger counter

This is sent as 2 I2C Data packets when accessed the first data packet contains bits 15:8 of the CPM and the second data packet contains bits 7:0 of the CPM value.

When the data is received you must reconstruct the data packets to get the actual reading of teh geiger counter.

`geiger.py` contains code that allows you to reconstruct the bits as well as initialise the I2C geiger counter.

<p align="center">

|||||||||
|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
|15|14|13|12|11|10|09|08|
|CPM.15|CPM.14|CPM.13|CPM.12|CPM.11|CPM.10|CPM.9|CPM.8|
|07|06|05|04|03|02|01|00|
|CPM.7|CPM.6|CPM.5|CPM.4|CPM.3|CPM.2|CPM.1|CPM.0|

</p>

## Event Loops

<p align="center">
	<img src="https://i.imgur.com/jf4rQC8.png" alt="drawing"/>
</p>

The code is split into 3 main sections, the Raspberry Pi Pico will read the serial data coming in from the UART0 pins, it will then filter the data to only extract the uSv value, then when asked output the data onto the I2C bus. 


------------

## Test Plan


<div align="center">

|Objective|Testing Strategy|Expected Output|Current Output|Pass/Fail|
|--|--|--|--|:--:|
|Functionality of Geiger counter|Plug Geiger directly to computer and view csv values when in range of mild and safe radioactive source. Listen for hum of HV tube. View LED flash|Hardware should react as specified when on switch engaged. CSV values should print automatically onto computer monitor|Hardware reacts as expected. CSV values viewed. CSV values increase when in range of test source|:heavy_check_mark:|
|Connect to Raspberry Pi Pico|Obtain values directly from Pi Pico and printing them through **chosen software program**|The values should display on in program|Testing not yet implement|To be announced post testing|
|Raspberry Pi Pico sends relevant data to the Raspberry Pi 3 master|Obtain values from **serial line/relevant i2c bus" or obtain values from directly from master Pi and printing them through **chosen software program**|Values from Geiger counter only should display in program|Testing not yet implement|To be announced post testing|
|Connects to GUI|Radioactivity section on GUI will be viewed when drone is connected to see if data is present|Geiger data from Master Raspberry Pi 3 can be viewed on GUI in relevant section|Testing not yet implement|To be announced post testing|
|Correct data is displayed in GUI|Two test sources, of which the team is aware of how many counts should be produced for each, are put in range of the Geiger counter at separate intervals and the data viewed|Data on GUI should correlate with expected results|Testing not yet implement|To be announced post testing|

</div>

------------

## Installation

*How to download and setup the code on the Pi Pico*

### Prerequisites

------------

## Deployment

*How to get it running and outputting data*

------------

