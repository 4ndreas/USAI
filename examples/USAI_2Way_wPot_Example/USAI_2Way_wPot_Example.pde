/*This is an example of the Universal Sensor Actor Interface (USAI)

The library uses a simple ASCII-protocoll to transfer data. | Name | : | Value |\r\n|.


For demonstration I used a Maple Mini STM32F103 board but the lib works also 
ESP8266 WiFi over UDP and on other Arduino boards

1. potentiometer attached to analog pin 15.
2. tow servos attached to pin 8 and 9.
3. Inbuild LED on Pin 33


*/

#include <Servo.h>
#include "usai.h"

const int LEDPIN = 33;
const int analogInPin = 15; 

Servo servo1;
Servo servo2;

int potiValue =  0;
int TXValue = 0;
int linCounter= 0;
float sinCounter = 0;
int RXValue = 0;
boolean switchValue = false;
int Servo1Value = 0;
int Servo2Value = 0;
uint32_t color = 0;	

bool boolConfig;
int  intConfig;
float floatConfig;
uint32_t colorConfig;

//start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc.
usai Sensors(&Serial);

int red;
int green;
int blue;

uValue V1("potiValue",Graph,&potiValue);
uValue V2("TXValue",Graph,&TXValue);
uValue V3("linCounter",Graph,&linCounter);
uValue V4("sinCounter",Graph,&sinCounter);

uValue V5("RXValue",Control,&RXValue);
uValue V6("switchValue",Control,&switchValue);
uValue V7("Servo1",Control,&Servo1Value);
uValue V8("Servo2",Control,&Servo2Value);

uValue V9("RGB",Control,&color);
uValue V10("Red",Graph,&red);
uValue V11("Greed",Graph,&green);
uValue V12("Blue",Graph,&blue);

uValue V13("BoolConfig",Config,&boolConfig);
uValue V14("intConfig",Config,&intConfig);
uValue V15("floatConfig",Config,&floatConfig);
uValue V16("colorConfig",Config,&colorConfig);

void setup(){
	Serial.begin(9600);

	servo1.attach(9);
	servo2.attach(8);

	V1.setMinMax(0,4096);  // ADC
	V3.setMinMax(0,255);  // lincounter
	V4.setMinMax(-1,1);   // sincounter

	V7.setMinMax(0,180);	// Servo1
	V8.setMinMax(0,180);	// Servo2

	V10.setMinMax(0,255);  // red
	V11.setMinMax(0,255);  // green
	V12.setMinMax(0,255);  // blue

	Sensors.begin();
	Sensors.addValue(&V1);
	Sensors.addValue(&V2);
	Sensors.addValue(&V3);
	Sensors.addValue(&V4);

	Sensors.addValue(&V5);
	Sensors.addValue(&V6);
	Sensors.addValue(&V7);
	Sensors.addValue(&V8);

	Sensors.addValue(&V9);
	Sensors.addValue(&V10);
	Sensors.addValue(&V11);
	Sensors.addValue(&V12);
	
	Sensors.addValue(&V13);
	Sensors.addValue(&V14);
	Sensors.addValue(&V15);
	Sensors.addValue(&V16);	
	
	pinMode(LEDPIN, OUTPUT);
	//enable pull-up
	pinMode(analogInPin, INPUT_ANALOG);
}

void loop(){
	
	// Read poti value
	potiValue =  analogRead(analogInPin);
	
	// set RX to TX
	TXValue = RXValue;
	
	linCounter++;
	if(linCounter > 255){linCounter = 0;}
	
	float angle = ( 2 * PI * linCounter) / 255 ;
	sinCounter= sin(angle);

	// Manage button
	if (switchValue)
	 { digitalWrite(LEDPIN,HIGH); }
	else 
	 { digitalWrite(LEDPIN,LOW); }

	// Manage Servos
	servo1.write(Servo1Value);
	servo2.write(Servo2Value);

	// Manage RGB Leds
	red = getRed(color);
	green = getGreen( color);
	blue = getBlue(color);


	

	Sensors.processSensorData();
	//delay
	delay(50);
}


uint32_t Color(byte r, byte g, byte b)
{
	uint32_t c;
	c = r;
	c <<= 8;
	c |= g;
	c <<= 8;
	c |= b;
	return c;
}

uint8_t getRed(uint32_t c) {
	return (c >> 16);
}
uint8_t getGreen(uint32_t c) {
	return (c >> 8);
}
uint8_t getBlue(uint32_t c) {
	return (c);
}