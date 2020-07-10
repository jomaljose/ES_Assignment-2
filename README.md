# LOGIC ANALYZER,OSCILLOSCOPE,FUNCTIONAL GENERATOR

### Embedded system assignment -2



## AIM

Set up a prototype of an Embedded System Logic analyzer, Function generator and simple oscilloscope using Teensy 3.2 board, Adafruit OLED display (SSD1306 128X64). I2C communication is used to interface with the Teensy and display.


## INTRODUCTION

The project is of finite states so that we can design it as a finite state machine. So we can make a finite state diagram and follow that to do the coding. 2 push buttons are used, one for OK and one for mode selection also we can choose mode by serial communication.
## METHODS
### Hardware Description


#### Components


 1. Teensy 3.2
 2. Adafruit OLED display (SSD1306 128X64)
 3. 10kOhm potentiometer
 4. Push button
 5. Wires
 6. Breadboard

#### Adafruit OLED display

The operating voltage of the SSD1306 controller is from 1.65V to 3.3V. It can communicate using I2C or SPI. I2C is used in this project. SSD1306 driver has a built-in 1KB Graphic Display Data RAM (GDDRAM) for the screen which holds the bit pattern to be displayed. This 1K memory area is organized in 8 pages (from 0 to 7). Each page contains 128 columns/segments (block 0 to 127). And each column can store 8 bits of data (from 0 to 7). SDApin of display is connected to the A4 pin (SDA0) of teensy and SCL is connected to the A5(SCL0) pin. 3.3V and GND are also connected as per pinout.
#### Pull up resistor calculation for display


"I2C bus drivers are  "open drain", meaning that they can pull the corresponding signal line low, but cannot drive it high. Thus, there can be no bus contention where one device is trying to drive the line high while another tries to pull it low, eliminating the potential for damage to the drivers or excessive power dissipation in the system. Each signal line has a  pull-up resistor on it, to restore the signal to high when no device is asserting it low.

Resistor selection varies with devices on the bus, but a good rule of thumb is to start with **4.7kΩ resistor and adjust down if necessary**. I2C is a fairly robust protocol, and can be used with short runs of wire (2-3m). For long runs, or systems with lots of devices, smaller resistors are better." - [([1]"I2C," n.d.)](https://learn.sparkfun.com/tutorials/i2c/i2c-at-the-hardware-level)
#### Push Button

The mechanical push button may produce errors in switching therefore we have written codes for debouncing and enabled input pullup resistor internally in teensy 3.2.

### Circuit
The display pins, SDA is connected to the A4 pin (SDA0) of teensy and SCL is connected to the A5(SCL0) pin. 3.3V and GND are also connected as per pinout. The push button is connected to the D5 and D6 pin of Teensy 3.2 and GND pin. A14 used as DAC pin for function generator mode. 
It is also explained in this [Youtube video](https://youtu.be/nFGAEsI4nOw).
Connection Schematic
![Connection Schematic](https://github.com/jomaljose/logic_analyser/blob/master/media/Schematic.png)
![Breadboard connection](https://github.com/jomaljose/logic_analyser/blob/master/media/circuit.jpg)
A0 is connected with a 10K ohm pot for giving a sample input to oscilloscope mode.
![Additional circuit](https://github.com/jomaljose/logic_analyser/blob/master/media/Additional%20circuit%20for%20osci.jpg)
