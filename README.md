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
- [Test Plan](#Test-Plan)
- [Installation](#Installation)
- [Deployment](#Deployment)

------------

## Subsystem Overview

The Mighty Ohm Geiger Counter Operates using UART serial communication and outputs the radiation levels in uSv, CPS and CPM however, we do not need all that data. The Raspberry Pi Pico reads the data from the UART connection and extracts only the data the system needs and passes that along to the Main Pi using a I2C connection.

**Subsystem Diagram :**

<p align="center">
	<img src="https://i.imgur.com/x1AuBI3.jpg" alt="drawing"/>
</p>



### Wiring Diagram
*To be added when finalised*

------------

## Code Overview

*breakdown of how the code is structured*

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

------------

## Deployment

*How to get it running and outputting data*

------------
