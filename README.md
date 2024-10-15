# Team Intel Engineers Volumetric Display

This is our senior project for the Fall 2024 semester at the University of Utah.

Volumetric three-dimensional displays are displays which give viewers a sense of depth and immersion
by displaying an image in a three-dimensional space. The motivation for such technology is the fact that viewers
can be more immersed into the content they are viewing. Specifically they can offer more accurate representations of three-dimensional objects. The method we are using involves taking a light emitting diode (LED) matrix and spinning it. As it’s spinning the matrix will be displaying different slices of a three dimensional image. These slices then form a three dimensional volumetric image. Our project is handheld and will display volumetric images from Blender.

Our proposal for this project can be found [here](https://drive.google.com/file/d/1ndLRNWP-nPX-9-LFJ2siUfhwasraIrXO/preview).

## Project Members

Our team is composed of three Computer Engineering students from the Department of Electrical & Computer Engineering.

#### Members
- Semrah Odobasic | u1293090 | semrahodobasic@icloud.com
- Logan Allen | u1190764 | Logan.Allen@utah.edu
- Jin Jeong   | u1375534 | djwls97865@gmail.com


## Parts List
### GPIO Implemetation
This implementation directly drives the matrix using the GPIO pins. This allows for us to change the image fast enough to show non circular volumetric images.

| Part Model            | Function                 |
|-----------------------|--------------------------|
| WaveShare RP2040 Tiny | Microcontroller          |
| Adafruit Dot Matrix   | Charlieplexed LED Matrix |
| Vishay TCRT5000       | IR Sensor                |
| RF-410CA              | Motor                    |
| Oshpark Custom PCB    | PCB Board                |

### I2C Implemetation (ONLY WORKS FOR CIRCULAR IMAGES)
This implementation uses the I2C controller for the LED matrix. We have determined that this cannot switch frames fast enough to show non circular shapes. An example of a circular shape that works is a cylinder since the display just has to have one 2D frame that spins.

| Part Model            | Function                 |
|-----------------------|--------------------------|
| WaveShare RP2040 Tiny | Microcontroller          |
| Adafruit IS31FL3731   | I2C LED Matrix Controller|
| Adafruit Dot Matrix   | Charlieplexed LED Matrix |
| Vishay TCRT5000       | IR Sensor                |
| RF-410CA              | Motor                    |
| ADAFRUIT DRV8833      | Motor Driver             |


## Projects (Under Code-Testing)
Within the Code-Testing folder there is various projects. As we have developed the project and changed the hardware we have made new projects to keep the old code avalible for our older designs. This also includes some projects that are for testing individual component implementations and verification.


- Display GPIO - This is our full implemetation code using GPIO for display control. Timing is very strict for this implementation. <br />
- Display - This is the first version of our full implemtation code using I2C. This code doesn't have the most accurate timing which caused issues. <br />
- Display V2 - This is the second version of our full implemenation code using I2C. This code works well but the I2C display isn't fast enough for what we need. <br />
- Matrix - Project used to do the code conversion from the C++ library into our own C library. <br />
- Motor - Project used to set up the motor control so that we can drive the motor. <br />
- Pico - Example projects downloaded from Rasperri Pi. <br />
- LED Onboard - This project is used to figure out how to drive the WS2818 LED that is onboard the waveshare RP2040. <br />

## Environment Setup
### Windows
The easiest way to get the toolchain running is to install the Visual Studio Code setup from rasperry pi. It will install all of the required programs for you. It will also ask you if you want to install the pico SDK which will save you a step in the installation process. You should also install the Pico Extras addon.

- Install Pico-Visual Studio Code: https://www.raspberrypi.com/news/raspberry-pi-pico-windows-installer/ <br />
- Install the Pico SDK: https://github.com/raspberrypi/pico-sdk

When attempting to build a file make sure the CMake filepath is set for the pico-sdk variable. You also have to set the filepath for the pico_extra variable. Ensure that you delete the build folder as you will have to do the build from scratch when starting up on a new machine.

Compiling generates a .uf2 file in the build folder. This file is the one you need to upload to the RP2040 to get the display running.