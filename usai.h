/*Universal Sensor Actor Interface (USAI)
* 
*The library uses a simple ASCII-protocoll to transfer data. | Name | : | Value |\r\n|
*/

// usai.h

#ifndef _USAI_h
#define _USAI_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
typedef void (*voidFuncPtr)(void);
typedef void (*intFuncPtr)(int);
#else
#include "WProgram.h"
#endif

#include "Stream.h"

#ifdef ARDUINO_ARCH_ESP8266
// #include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#else 
//#include <EthernetUdp.h>
#include <Ethernet.h>
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
	_void, // 0
	_bool, // 1
	_char, // 2
	_int,
	_long,
	_float,
	_string,
	_color,
	_double
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
	uValue(const char * Name, cmdType _Type, bool *ptr);
	uValue(const char * Name, cmdType _Type, char *ptr);
	uValue(const char * Name, cmdType _Type, int *ptr);
	// uValue(const char * Name, cmdType _Type, long *ptr);
	uValue(const char * Name, cmdType _Type, float *ptr);
	uValue(const char * Name, cmdType _Type, double *ptr);
	uValue(const char * Name, cmdType _Type, int32_t *ptr);
	uValue(const char * Name, cmdType _Type, uint32_t *ptr);
	

	cmdType _cmd;
	dataType _dataType;
	void *_ptr;
	const char * _name;
	int _minimum;
	int _maximum;
	uint32_t _updateRate; // update rate in ms
	uint32_t previousMillis;

	void setValue(String val);
	void setValue(const char * Name, cmdType Type, void *PTR, dataType DataType=_void);
	void setMinMax();
	void setMinMax(int min, int max);
	

	voidFuncPtr callback;
	void setCallback(voidFuncPtr handler);

	private:
		uint32_t _lastUpdate;

};

class usai
{
	public:
	usai( Stream * _Serial);
#ifdef ARDUINO_ARCH_ESP8266
	usai( WiFiUDP * udp);
#else
	usai (EthernetUDP * udp);
#endif

	void begin();
	void processSensorData();
	int addValue(uValue * val);
	int addValue(uValue * val, int min, int max);
	void sendConfig();
    void setSampletime(uint32_t stime);

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
#else
	EthernetUDP * _udp;
	IPAddress _remoteIP;	
#endif
	unsigned int _remotePort;

	uValue * _sValues[CMDBUFFERSIZE];
	char _Buffer[CHARBUFFERSIZE];
	int _sensorNumber;

	String inputString = "";         // a string to hold incoming data
	bool stringComplete = false;  // whether the string is complete


	uint32_t sampletime = 50;	// 50ms -> 20Hz
	uint32_t previousMillis = 0;
	uint32_t currentMillis = 0;;
};



#endif