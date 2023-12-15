# 3DPaint
A 3D drawing tool and software, coordinating a light point in 3D space using three photocells connected to a microcontroller and drawing the result in real-time within the software developed in C++ with OpenGL.

## Demo
The GIF below demonstrates the 3DPaint system in action.
As can be seen by the reflection of the lightbulb on the screen, I'm drawing a spiral by moving the lightbulb. The software captures the lightbulb's 3D position and instantly draws it onscreen.

![3DPaint](https://github.com/SamanMohseni/3DPaint/assets/51726090/b1be975c-c152-4c2e-94a0-97bce05ebb91)
*A demo of 3D paint*

## The 3D drawing tool
The 3D drawing tool is composed of an AVR ATMEGA168 microcontroller, with three orthogonal photocells connected to it over the ADC pins on the microcontroller (PC0, PC1, PC2 in this case).
Each photocell is connected in series with a 10KΩ resistor, with one end of the photocell attached to VDD and the other end of the resistor grounded. The voltage at their junction is the sensor's reading.
The microcontroller's internal 10-bit ADC measures these voltages and sends the data to a computer via a USB-to-UART bridge.

Additionally, a simple switch connected to the microcontroller (at PD2 with the pull-up enabled) allows the user to control whether the 3D cursor draws on the software or not.

## The Software
The software reads the values from the three orthogonal photocells, and using some mathematical formulations, it calculates the position of the lightbulb, which is then visualized as a 3D cursor on the screen. When the switch is activated, the software generates a line that tracks the cursor's path.
