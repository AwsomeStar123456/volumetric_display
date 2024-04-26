# Intel Engineers Volumetric Display
 This is team Intel Engineers senior project GitHub. Our project is a volumetric display that uses an LED matrix which we spin to display a 3D image. The initial design is a small handheld display which will showcase models rendered from Blender.

## Team Members
Semrah Odobasic | u1293090 | semrahodobasic@icloud.com <br />
Logan Allen | u1190764 | Logan.Allen@utah.edu <br />
Jin Jeong   | u1375534 | djwls97865@gmail.com <br />


## Parts List
Waveshare RP2040 Tiny: Microcontroller <br />
Adafruit IS31FL3731: Matrix Controller <br />
Adafruit Dot Matrix Module: Charlieplexed LED Matrix <br />
Vishay TCRT5000: IR Sensor <br />
RF-410CA: Motor <br />
ADAFRUIT DRV8833: Motor Driver <br />

## Projects (Under Code-Testing)
Display - This is the location of the final code which integrates all of the systens.
Matrix - Project used to do the code conversion from the C++ library into our own C library. <br />
Motor - Project used to set up the motor control so that we can drive the motor. <br />
Pico - Example projects downloaded from rasperri pi <br />

## Installation Setup
### Windows
The easiest way to get the toolchain running is to install the Visual Studio Code setup from rasperry pi. It will install all of the
required programs for you. It will also ask you if you want to install the pico SDK which will save you a step in the installation
process.

Install Pico-Visual Studio Code: https://www.raspberrypi.com/news/raspberry-pi-pico-windows-installer/ <br />
Install the Pico SDK: https://github.com/raspberrypi/pico-sdk

When attempting to build a file make sure the CMake filepath is set for the pico-sdk. Also ensure to delete the build folder as you will have to do the build from scratch when starting up on a new machine.

Compiling generates a .uf2 file in the build folder. This file is the one you need to upload to the RP2040 to get the display running.