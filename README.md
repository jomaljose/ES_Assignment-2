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

Resistor selection varies with devices on the bus, but a good rule of thumb is to start with **4.7kΩ resistor and adjust down if necessary**. I2C is a fairly robust protocol, and can be used with short runs of wire (2-3m). For long runs, or systems with lots of devices, smaller resistors are better." - [([2]"I2C," n.d.)](https://learn.sparkfun.com/tutorials/i2c/i2c-at-the-hardware-level)
#### Push Button

The mechanical push button may produce errors in switching therefore we have written codes for debouncing and enabled input pullup resistor internally in teensy 3.2.

### Circuit
The display pins, SDA is connected to the A4 pin (SDA0) of teensy and SCL is connected to the A5(SCL0) pin. 3.3V and GND are also connected as per pinout. The push button is connected to the D5 and D6 pin of Teensy 3.2 and GND pin. A14 used as DAC pin for function generator mode. 
It is also explained in this [Youtube video](https://youtu.be/nFGAEsI4nOw)
Connection Schematic
![Connection Schematic](https://github.com/jomaljose/logic_analyser/blob/master/media/Schematic.png)
![Breadboard connection](https://github.com/jomaljose/logic_analyser/blob/master/media/circuit.jpg)
A0 is connected with a 10K ohm pot for giving a sample input to oscilloscope mode.
![Additional circuit](https://github.com/jomaljose/logic_analyser/blob/master/media/Additional%20circuit%20for%20osci.jpg)

### Design of finite state machine
#### States
 1. Welcome 
 2. Menu 
 3. Oscilloscope 
 4. Function generator 
 5. Logic analyser 
 6. Sinewave 
 7. Square wave 
 8. Triangular wave
#### Events
 1. OK pressed (OK count is used in the program to switch from welcome state to Menu.)
 2. Reset
 3. Mode count 1
 4. Mode count 2
 5. Mode count 3
 6. Serial command 1
 7. Serial command 2
 8. Serial command 3
 9. Serial command 4 
10. Serial command 5
11. Serial command 6
  #### State Diagram
  ![state diagram](https://github.com/jomaljose/logic_analyser/blob/master/media/State%20diagram.jpg)

### [Software](https://github.com/jomaljose/logic_analyser/blob/master/main.cpp)
 VSCode with Platform IO extension is used to code in this project. The pin 5 and 6 are defined as *OKpin* and *Modepin* respectively. Internal pullup resistor is enabled by `pinMode(OK_buttonpin, INPUT_PULLUP)` for the button pin. An Interrupt is triggered on the falling edge of the switching action of OK button. and press count is updated. this OK count is compared to exit from Welcome state to menu. Then onwards we only care about the OK button state, not its count. But the MODE button count is used to define all other states. *enum* datatype is used to define states. If the Mode press count change the selection changes accordingly and when OK or serial command received jump into that state. The teensy reset pin is used as the reset pin so it always start from beginning when resets. The DAC pin is used as function generator output but along with that signals are plotted in display. For that adafruit libraries Adafruit_GFX.h , 
Adafruit_SSD1306.h are used.
	In Oscilloscope mode ADC pin A0 is used as input channel. The input signal is converted to digial by a 10 bit ADC which is configured by default in teensy 3.2. The recieved signal is maped mathematically by
	

    float voltage = sensorValue * (3.3 / 1023.0); // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 3.3V):

    voltage = 32-(voltage*7); // y axis value: multiplied with 7 to scale y value
In logic analyser I just convert the serial input value to Binary bit and Hex values. And displayed them on Serial monitor and OLED display. I dont know is that what we intended to do.
## [RESULT]
### Output Waveform Display
Sine wave
![Sine](https://github.com/jomaljose/logic_analyser/blob/master/media/Sine.jpg)

Square wave
![Square](https://github.com/jomaljose/logic_analyser/blob/master/media/Square.jpg)
Triangular wave
![Triangle](https://github.com/jomaljose/logic_analyser/blob/master/media/Triangle.jpg)

The waves generated is intented to, plotted by serial printing the yaxis value and plotting it using [Serialplot](https://hackaday.io/project/5334-serialplot-realtime-plotting-software) software. (sorry my plots were wiered)


## FUTURE SCOPE & LIMITATION
The current prototype is giving function generator outputs which are not based on time functions like millis() or micros() so that it cannot be used for any precise uses like labs or research. In the coming weeks I will correct the serial plot values and try to understand more about logic analyser. 
## CONCLUSION

All the desired states are obtained. In this project  *enum* datatypes, I2C communication and OLED displaying is used successfully. And was successfully able to make a prototype in a tiny breadboard with minimum components. Even though there are some drawbacks which will be corrected soon.

## REFERENCE

 1. Engineers, L. M. (2019, December 13).  _Interface OLED graphic display module with Arduino_. Last Minute Engineers.  [https://lastminuteengineers.com/oled-display-arduino-tutorial/](https://lastminuteengineers.com/oled-display-arduino-tutorial/)
 2. _I2c_. (n.d.). Learn at SparkFun Electronics - learn.sparkfun.com.  [https://learn.sparkfun.com/tutorials/i2c/i2c-at-the-hardware-level](https://learn.sparkfun.com/tutorials/i2c/i2c-at-the-hardware-level)
 3. Teensyduino: Using the UART (real serial) with Teensy on the Arduino IDE. (2020). Retrieved from https://www.pjrc.com/teensy/td_uart.html
 4. _Adafruit SSD1306: Adafruit_SSD1306 class reference_. (n.d.). [https://adafruit.github.io/Adafruit_SSD1306/html/class_adafruit___s_s_d1306.html](https://adafruit.github.io/Adafruit_SSD1306/html/class_adafruit___s_s_d1306.html)
 5. _Sine_. (2006, May 4). Wikipedia, the free encyclopedia. Retrieved July 11, 2020, from [https://en.wikipedia.org/wiki/Sine](https://en.wikipedia.org/wiki/Sine)
 6. _Guide for I2C OLED display with Arduino_. (2020, May 7). Random Nerd Tutorials. [https://randomnerdtutorials.com/guide-for-oled-display-with-arduino/](https://randomnerdtutorials.com/guide-for-oled-display-with-arduino/)

