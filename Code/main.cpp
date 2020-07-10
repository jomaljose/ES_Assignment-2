
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

void Serial_com();
void oscilloscope();
void Squaresignal();
void Sinesignal();
void trianglesignal();

#define OK_buttonpin 5
#define Mode_buttonpin 6
#define DAPin A14
#define ADCpin A0
int OK_buttonPushCounter = 0;   // counter for the number of button presses
int OK_buttonState = 0;         // current state of the button
int last_OK_ButtonState = 0;     // previous state of the button
bool OK_pressed = 0;
bool last_OK_state = 0;
int MODE_buttonPushCounter = 0;
int MODE_buttonState = 0;         // current state of the button
int last_MODE_ButtonState = 0;     // previous state of the button
const uint32_t debouncedelay_ms = 150;  // Small delay for debounce
const uint32_t MODE_debouncedelay_ms = 250;

void OK_buttonpress_ISR();                    // Interrupt function
int serialreaded;
int serial_cmnd  = 0;                         //variable for serial input
int serial_input = 0;
Adafruit_SSD1306 display(-1);                 //reset pin not valid

////// For sine wave/////////
int hCenter = 32;                               // horizontal center of 64 / 2 = 32
int Radius = 30;                                // radius of circle
const float Pi = 3.14159265359;                 // Pi

//states/////
enum states {
  Menu,
  Osciloscope, 
  FunctionGen, 
  LogicAnalyser, 
  SinWave, 
  SquareWave, 
  TriangularWave,
};
states currentState = Menu;                             //Welcome screen is also in this state
// 'welcome', 128x64px 
const unsigned char welcome [] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xfe, 0x1f, 0xe2, 0x03, 0x87, 0xfc, 0x1f, 0x87, 0xc7, 0xe3, 0x01, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xfe, 0x1c, 0xe3, 0x01, 0x87, 0xf1, 0x8e, 0x31, 0xc3, 0xe3, 0x80, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0x1c, 0xf7, 0x19, 0xcf, 0xe7, 0xcc, 0x79, 0xe3, 0xc3, 0x8e, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0x9c, 0x6f, 0x1f, 0xcf, 0xc7, 0xf8, 0xf8, 0xe1, 0xc3, 0x8f, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0x98, 0x6f, 0x1b, 0xcf, 0xc7, 0xf8, 0xf8, 0xe1, 0x83, 0x8d, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0x8a, 0x6f, 0x03, 0xcf, 0xc7, 0xf8, 0xf8, 0xe9, 0xa3, 0x81, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0x82, 0x1f, 0x1b, 0xcf, 0xc7, 0xf8, 0xf8, 0xe8, 0x23, 0x8d, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xc6, 0x1f, 0x1b, 0xcf, 0xc7, 0xf8, 0xf8, 0xcc, 0x63, 0x8f, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xc7, 0x1f, 0x1c, 0xcf, 0x47, 0xec, 0x78, 0xcc, 0x63, 0x8f, 0x7f, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xc7, 0x1f, 0x1d, 0xc6, 0x63, 0xcc, 0x79, 0xcc, 0xe3, 0x8e, 0x7f, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xef, 0xbe, 0x01, 0x00, 0x70, 0x1f, 0x03, 0x86, 0xc1, 0x00, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xf3, 0x8f, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0x80, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x33, 0x1f, 0x80, 0xff, 0xf3, 0xcf, 0xff, 0xff, 0xff, 
0xff, 0x9e, 0x7f, 0xff, 0xff, 0xff, 0xcf, 0x33, 0x3f, 0x9c, 0xff, 0xf3, 0xcf, 0xff, 0xff, 0xff, 
0xff, 0x9e, 0x61, 0xc7, 0xc3, 0x0f, 0x9f, 0x92, 0x3f, 0x9c, 0xce, 0x60, 0x83, 0x8f, 0x23, 0xff, 
0xff, 0x9c, 0x61, 0x83, 0x86, 0x1f, 0x9f, 0x90, 0x7f, 0x81, 0xce, 0x60, 0x83, 0x07, 0x01, 0xff, 
0xff, 0x80, 0xe7, 0x39, 0x9e, 0x7f, 0x9f, 0x90, 0x7f, 0x80, 0xce, 0x73, 0xce, 0x73, 0x39, 0xff, 
0xff, 0x81, 0xe7, 0x01, 0x86, 0x1f, 0x9f, 0x90, 0x3f, 0x9c, 0x4e, 0x73, 0xce, 0x73, 0x39, 0xff, 
0xff, 0x9f, 0xe7, 0x01, 0xc3, 0x0f, 0x9f, 0x93, 0x3f, 0x9e, 0x4e, 0x73, 0xce, 0x73, 0x39, 0xff, 
0xff, 0x9f, 0xe7, 0x3f, 0xf3, 0xcf, 0xcf, 0x33, 0x1f, 0x9e, 0x4e, 0x73, 0xce, 0x73, 0x39, 0xff, 
0xff, 0x9f, 0xe7, 0x83, 0x82, 0x0f, 0xc0, 0x33, 0x9f, 0x80, 0xc0, 0x70, 0xc3, 0x07, 0x39, 0xff, 
0xff, 0x9f, 0xe7, 0xc1, 0x86, 0x1f, 0xf0, 0xf3, 0x8f, 0x81, 0xe2, 0x78, 0xe3, 0x8f, 0x39, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

void setup()   
{  
   // initialize the button pins as an input:
  
   pinMode(OK_buttonpin, INPUT_PULLUP); // Enabling internal pullup resistor for button
   pinMode(Mode_buttonpin, INPUT_PULLUP);
   Serial.begin(9600);  
  //delay(3000); 

	// initialize with the I2C addr 0x3C
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
	attachInterrupt(digitalPinToInterrupt(OK_buttonpin), OK_buttonpress_ISR, FALLING); // OK button press enable interrupt function 'buttonpress_ISR'
}


void loop() 
{ 
Serial_com() ;                                      //function to reads serail data

// MODE BUTTON//

MODE_buttonState = digitalRead(Mode_buttonpin);									// read the pushbutton input pin:
static uint32_t previoustime_MODE_button_ms = 0;                              // Variable for debouncing
uint32_t currenttime_MODE_button_ms = millis();                               // Variable for debouncing
if((currenttime_MODE_button_ms - previoustime_MODE_button_ms) >= MODE_debouncedelay_ms) // Code inside loop only executes if buttonpress time is greater than debounce delay_ms
  { 
  if (MODE_buttonState != last_MODE_ButtonState) 
  	{
    // if the state has changed, increment the counter
    if (MODE_buttonState == HIGH) 
		{
      
      MODE_buttonPushCounter++;
	  
    }
		 last_MODE_ButtonState = MODE_buttonState;
		  if(MODE_buttonPushCounter >3)
			{
		MODE_buttonPushCounter=1;                         //limits the counter value between 1 to 3
			}
	 //Serial.println(MODE_buttonPushCounter);          for error check
	 							  
 	}
  }
 switch (currentState) {                              //states and events
      case Menu:
 if (OK_buttonPushCounter >= 1 )                      //when OK pressed once it will enter to Menu
 {

display.clearDisplay();
display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(0,0);
display.println("Menu");
display.setCursor(10,20);
display.print("Osciloscope");
display.setCursor(10,35);
display.print("Function Generator");
display.setCursor(10,50);
display.print("Logic Analyser");
display.display();

	if (MODE_buttonPushCounter==1 || serial_cmnd ==1)             //shows the selection according to the mode press count
	{
		display.setTextSize(1);
		display.setCursor(0,20);
		display.print("o");
		display.display();
	  if(OK_pressed || serial_cmnd ==1)                         //if ok pressed or serial command 1 then enter to oscilloscope state
	  	{
		  currentState = Osciloscope;
		  OK_pressed = 0;
		  }

	}
	if (MODE_buttonPushCounter==2  || serial_cmnd ==2)
	{
		display.setCursor(0,35);
		display.print("o");
		display.display();
		if(OK_pressed || serial_cmnd ==2)                      //when satisfies enterr  to Function generator
	  	{
		  currentState = FunctionGen;
		  OK_pressed = 0;
		  }
	}
		if (MODE_buttonPushCounter==3  || serial_cmnd ==3)
	{
		display.setCursor(0,50);
		display.print("o");
		display.display();
		if(OK_pressed  || serial_cmnd ==3)
	  	{
		  currentState = LogicAnalyser;                       // when satisfies enter into Logic analyser 
		  OK_pressed = 0;
		  }
	}
	OK_pressed = 0;
 }
 else                                                     // till Ok button first press shows Welcome screen
 {	if (OK_buttonPushCounter < 1  || serial_cmnd == 0)
   {// Clear the buffer.
	display.clearDisplay();

	// Display bitmap
	display.drawBitmap(0, 0,  welcome, 128, 64, WHITE);
	display.display();

//instructions for serial operations//
	Serial.println("Modes");
	Serial.println("Enter 1 for Oscilloscope");
	Serial.println("Enter 2 for Function Generator");
	Serial.println("Enter 3 for Logic Analyzer");
	Serial.println("Function Generator Modes");
	Serial.println("Enter 4 for Sine Wave");
	Serial.println("Enter 5 for Square Wave");
	Serial.println("Enter 6 for Triangular Wave");
	OK_pressed = 0;
   }
 }
 break;

case Osciloscope:                                           // Oscilloscope mode
oscilloscope();
 break;

case FunctionGen:                                            //Functiongenerator mode
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(0,0);
display.println("Function Generator");
display.setCursor(10,20);
display.print("Sine Wave");
display.setCursor(10,35);
display.print("Square Wave");
display.setCursor(10,50);
display.print("Triangular Wave");
display.display();

	if (MODE_buttonPushCounter==1 || serial_cmnd ==4)         //Function selection for Function generator
	{
		display.setTextSize(1);
		display.setCursor(0,20);
		display.print("o");
		display.display();
	  if(OK_pressed || serial_cmnd ==4)
	  	{
		  currentState = SinWave;                               //sinewave selected if Ok pressed or serial command 4
		  OK_pressed = 0;
		  }

	}
	if (MODE_buttonPushCounter==2  || serial_cmnd ==5)
	{                                                         //Squarewave selected if Ok pressed or serial command 5
		display.setCursor(0,35);
		display.print("o");
		display.display();
		if(OK_pressed  || serial_cmnd ==5)
	  	{
		  currentState = SquareWave;
		  OK_pressed = 0;
		  }
	}
		if (MODE_buttonPushCounter==3  || serial_cmnd ==6)
	{                                                        //Triangularwave selected if Ok pressed or serial command 6
		display.setCursor(0,50);
		display.print("o");
		display.display();
		if(OK_pressed  || serial_cmnd ==6)
	  	{
		  currentState = TriangularWave;
		  OK_pressed = 0;
		  }
	}
break;

case LogicAnalyser:                                       //Logic analyser reads the serial input numbers and convert it into Binary code
    display.clearDisplay();
    Serial.println("Data in BIN :");
    Serial.println(serial_cmnd, BIN);                     //Serial display of BIN values
    display.setCursor(0,0);
	display.print("Logic Analyser");
	display.setCursor(0,30);
	display.println("Data in BIN :");
    display.println(serial_cmnd,BIN);                     //Oled display of BIN values
	display.println("Data in HEX :");
    display.println(serial_cmnd,HEX);
    display.setTextSize(1);
    display.display();
break;

case SinWave:
  display.clearDisplay();                                      // clears display
  Sinesignal();                                                //Sine wave generating function
break;

case SquareWave:
	Squaresignal();                                              
break;

case TriangularWave:
trianglesignal();
break;
}}
void Serial_com()                                               //Serial communication function
{

  if( Serial.available())
  {
     char serial_read = Serial.read();                          //serial read and store it
    if(serial_read >= '0' && serial_read <= '9')
    {
    serial_input = (serial_input * 10) + (serial_read - '0');   //convert ASCI code
    }
    else if (serial_read == 10)                                 // is the character the newline character
    {
      serial_cmnd = serial_input;                                // stored to Serial command
      //Serial.println(serial_cmnd); error check
	  //serialreaded = serial_cmnd;  
      serial_input = 0;                                         // reset val to 0 ready for the next sequence of digits
    }
   }
}

void Squaresignal()
{
  display.clearDisplay();                                      // clears display
  display.drawRect(0, 0, 120, 64, WHITE);                      // draws border
  display.drawLine(0,30,120,30,WHITE);                         // draws grid horizontal center
  display.drawLine(30,0,30,64,WHITE);                          // draws grid vertical first
  display.drawLine(60,0,60,64,WHITE);                          // draws grid vertical second
  display.drawLine(90,0,90,64,WHITE);                          // draws grid vertical third
	
	display.setCursor(95,34);									                  // X-axis labels
	display.print("t(ms)");
	display.setCursor(0,0);
	display.print("v(mv)");									                    // Y-axis labels
	display.display();
  static int y = 0 ;										
  
  for (int i=0; i<120; i++)									                  // draws 120 pixels per loop I stores X Axis value and y stores Yaxis value
  {                                   
    
      if(i<30)                                                //Y plotted high till half timeperiod
      {
        y = 10;
      }
      if(i==30)
      {
       display.drawLine(30,10,30,54,WHITE);                   //Drop line
      }
      if(i>30)                                                //Y plotted low till next half timeperiod
      {
        if(i<60)
        {y = 54;}
      }
      if(i==60)
      {
       display.drawLine(60,10,60,54,WHITE);                   //Drop line
      }
      if(i>60)
      {
      if(i<90)
        {y = 10;}
      }
	  if(i==90)
      {
       display.drawLine(90,10,60,54,WHITE); 
      }
      if(i>90)
      {
        y=54;
      }
    display.drawPixel(i,y,WHITE);                              // draws each pixel for Square wave
    display.display();                                         // displays new screen information
    analogWrite(DAPin, y);                                     // send signal to DAC
  }
}

void Sinesignal()
{
  display.drawRect(0, 0, 120, 64, WHITE);                      // draws border
  display.drawLine(0,30,120,30,WHITE);                         // draws grid horizontal center
  display.drawLine(30,0,30,64,WHITE);                          // draws grid vertical first
  display.drawLine(60,0,60,64,WHITE);                          // draws grid vertical second
  display.drawLine(90,0,90,64,WHITE);                          // draws grid vertical third
	display.setCursor(95,34);								                     // axis labels
	display.print("t(ms)");
	display.setCursor(0,0);
	display.print("v(mv)");
	display.display();
  for (int i=0; i<120; i++)                                   // draws 120 pixels per loop
   { 
    float Angle = i*3 ;                                        // 120 X 3 = 360°
    int a = (hCenter - (sin(Angle * (Pi / 180)) * Radius));    // Pi/180 converts degrees to radians
    //Serial.println(a);                                        // for serial plotting
    display.drawPixel(i,a,WHITE);                              // draws each pixel for Sine wave
    display.display();                                         // displays new screen information
    analogWrite(DAPin,a);                                     // send signal to DAC  
  }
}

void trianglesignal()
{
	display.clearDisplay();                                      // clears display
  display.drawRect(0, 0, 120, 64, WHITE);                      // draws border
  display.drawLine(0,30,120,30,WHITE);                         // draws grid horizontal center
  display.drawLine(30,0,30,64,WHITE);                          // draws grid vertical first
  display.drawLine(60,0,60,64,WHITE);                          // draws grid vertical second
  display.drawLine(90,0,90,64,WHITE);                          // draws grid vertical third
	
	display.setCursor(95,34);									                   // axis labels
	display.print("t(ms)");
	display.setCursor(0,0);
	display.print("v(mv)");
	display.display();
  static int y = 0 ;										
  
  for (int i=0; i<120; i++)									                 // draws 120 pixels per loop
  {                                   
    
      if(i<=30)                                              // Y increasing till 1/4 timeperiod
      {
        y = i;
      }
      if(i>30)
      {
        if(i<60)
        {y--;}                                                //Y decreasing next 1/4 timeperiod
      }
      if(i>=60)
      {
      if(i<90)
        {y = i-60;}
      }
      if(i>90)
      {
        y--;
      }
    display.drawPixel(i,y,WHITE);                              // draws each pixel for Square wave
    display.display();                                         // displays new screen information
    analogWrite(DAPin, y);                                     // send signal to DAC  
  }
}

void oscilloscope()
{
  display.clearDisplay();                                      // clears display
  display.drawRect(0, 0, 120, 64, WHITE);                      // draws border
  display.drawLine(0,30,120,30,WHITE);                         // draws grid horizontal center
  display.drawLine(30,0,30,64,WHITE);                          // draws grid vertical first
  display.drawLine(60,0,60,64,WHITE);                          // draws grid vertical second
  display.drawLine(90,0,90,64,WHITE);                          // draws grid vertical third
	
	display.setCursor(95,34);									                   // axis labels
	display.print("t(ms)");
	display.setCursor(0,0);
	display.print("3.3V");                                        //we get max volt o/p of 3.3V from teensy
	display.setCursor(0,32);
	display.print("0");
	display.display();
  static int y = 0 ;										
  
for(int x=0;x<120;x++)
    {
     
   int sensorValue = analogRead(A0);                                // read the input on analog pin 0:
  float voltage = sensorValue * (3.3 / 1023.0);                     // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 3.3V):
  voltage = 32-(voltage*7);                                         // y axis value: multiplied with 7 to scale y value 
  display.drawPixel(x,voltage,WHITE);                               // draws each pixel 
    display.display();                                              // displays new screen information
  
    }
  
}

void OK_buttonpress_ISR()                                                  // Interrupt function for OK button
{
  static uint32_t previoustimebutton_ms = 0;                              // Variable for debouncing
  uint32_t currenttimebutton_ms = millis();                               // Variable for debouncing
   if((currenttimebutton_ms - previoustimebutton_ms) >= debouncedelay_ms) // Code inside loop only executes if buttonpress time is greater than debounce delay_ms
  { OK_pressed = !OK_pressed;
    previoustimebutton_ms = currenttimebutton_ms;
	OK_buttonPushCounter++;                                                 //Ok press counts
  }
}