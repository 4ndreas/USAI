/*Universal Sensor Actor Interface (USAI)
* 
*The library uses a simple ASCII-protocoll to transfer data. | Name | : | Value |\r\n|
*/

// usai.h

#ifndef _USAI_h
#define _USAI_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "Stream.h"

#ifdef ARDUINO_ARCH_ESP8266
#include <WiFiUDP.h>
#endif
#define CMDBUFFERSIZE 40
#define CHARBUFFERSIZE 50

enum connectionType
{
	_SERIAL,
	_UDP
};

enum cmdType
{
	Graph,Control,Config
};

enum dataType
{
	_void,
	_bool,
	_char,
	_int,
	_long,
	_float,
	_string,
	_color
};

struct sensorValues
{
	cmdType _cmd;
	dataType _dataType;
	void *ptr;
	char * _name;
};

class uValue
{
	public:
	uValue(const char * Name, cmdType Type, void *PTR, dataType DataType=_void);
	uValue(const char * Name, cmdType _Type, boolean *ptr);
	uValue(const char * Name, cmdType _Type, char *ptr);
	uValue(const char * Name, cmdType _Type, int *ptr);
	uValue(const char * Name, cmdType _Type, long *ptr);
	uValue(const char * Name, cmdType _Type, float *ptr);
	uValue(const char * Name, cmdType _Type, double *ptr);
	uValue(const char * Name, cmdType _Type, uint32_t *ptr);
	

	cmdType _cmd;
	dataType _dataType;
	void *_ptr;
	const char * _name;
	int _minimum;
	int _maximum;

	void setValue(String val);
	void setValue(const char * Name, cmdType Type, void *PTR, dataType DataType=_void);
	void setMinMax();
	void setMinMax(int min, int max);

	void (*callback)();
	void setCallback(void (*handler)());

};

class usai
{
	public:
	usai( Stream * _Serial);
#ifdef ARDUINO_ARCH_ESP8266
	usai( WiFiUDP * udp);
#endif
	void begin();
	void processSensorData();
	void addValue(uValue * val);
	void sendConfig();

	protected:
	
	private:
	void printAll(int _nr);
	void _addValue(cmdType _Type, void  *ptr, dataType _dType);
	void processInput();
	void processOutput();
	void processGraph(int _nr);
	void processConfig(int _nr);
	void processControl(int _nr);
	void printValue(int _nr);
	void noOp(void);

	connectionType _connectionType;
	Stream * _serial;
#ifdef ARDUINO_ARCH_ESP8266
	WiFiUDP * _udp;
	IPAddress _remoteIP;
#endif
	unsigned int _remotePort;

	uValue * _sValues[CMDBUFFERSIZE];
	char _Buffer[CHARBUFFERSIZE];
	int _sensorNumber;

	String inputString = "";         // a string to hold incoming data
	boolean stringComplete = false;  // whether the string is complete
};



#endif