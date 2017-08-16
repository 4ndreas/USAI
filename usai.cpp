//
//
//

#include "usai.h"


usai::usai(Stream *_Serial)
{
	_serial = _Serial;
	_connectionType = _SERIAL;
}

#ifdef ARDUINO_ARCH_ESP8266
 usai::usai(WiFiUDP * udp)
 {
	 _serial = udp;
	 _udp = udp;
	 //_remoteIP = IP;
	 _connectionType = _UDP;
 }
 #endif

void usai::begin()
{
	_sensorNumber = 0;
}

void usai::addValue(uValue * val)
{
	if (_sensorNumber < CMDBUFFERSIZE)
	{
		_sValues[_sensorNumber] = val;
		_sensorNumber++;
	}
}


void usai::sendConfig()
{
	for (uint8_t i = 0; i < _sensorNumber; i++)
	{
		printAll(i);
	}
	//_serial->println();	
}

void usai::printAll(int _nr)
{
	if ((_nr >= 0) && (_nr <_sensorNumber))
	{
		_serial->print("ID:");
		_serial->print(_nr);
		_serial->print(";");
		_serial->print(_sValues[_nr]->_name);
		_serial->print(";");
		_serial->print(_sValues[_nr]->_cmd);
		_serial->print(";");
		_serial->print(_sValues[_nr]->_dataType);
		_serial->print(";");
		printValue(_nr);
		_serial->print(";");
		_serial->print(_sValues[_nr]->_minimum);
		_serial->print(";");
		_serial->print(_sValues[_nr]->_maximum);
		_serial->println(";");
	}
}

void usai::processInput()
{
		String rxName = "";
		while (_serial->available())
		{
			char inChar = (char)_serial->read();
			inputString += inChar;
			if (inChar == '\n')
			{
				rxName = inputString.substring(0,inputString.indexOf(':'));
				if (rxName.equals("getAll"))
				{
					sendConfig();
				}
				else if (rxName.equals("get"))
				{
					// get value of id x
					int id = (inputString.substring(inputString.indexOf(':')+1)).toInt();
					printAll(id);
				}
				else
				{
					for (uint8_t i = 0; i < _sensorNumber; i++)
					{
						if (rxName.equals(_sValues[i]->_name))
						{
							_sValues[i]->setValue(inputString.substring(inputString.indexOf(':')+1));
						}
					}
				}
				inputString = "";
			}
		}
}

void usai::processOutput()
{
	for (uint8_t i = 0; i< _sensorNumber; i++)
	{
		switch(_sValues[i]->_cmd)
		{
			case Graph:{
				processGraph(i);
				break;
			}
			case Control:{
				processControl(i);
				break;
			}
			case Config:{
				processConfig(i);
				break;
			}
		}
	}
}

void usai::processSensorData()
{
	if(_connectionType == _SERIAL)
	{
		processInput();
		processOutput();
		//_serial->println();
	}
#ifdef ARDUINO_ARCH_ESP8266
	else if(_connectionType == _UDP)
	{
		int packetSize = _udp->parsePacket();
		if (packetSize)
		{
			_remoteIP = _udp->remoteIP();
			_remotePort = _udp->remotePort();
			processInput();
		}
		if (_remoteIP)
		{
			_udp->beginPacket(_remoteIP, _remotePort);
			processOutput();
			_udp->endPacket();
		}
	}
#endif
}

void usai::printValue(int _nr)
{
	// data type
	switch(_sValues[_nr]->_dataType)
	{
		case _bool: {
			bool * _ptr = (bool*)_sValues[_nr]->_ptr;
			_serial->print(*_ptr);
			break;
		}
		case _char: {
			char * _ptr = (char*)_sValues[_nr]->_ptr;
			_serial->print(*_ptr);
			break;
		}
		case _int: {
			int * _ptr = (int*)_sValues[_nr]->_ptr;
			_serial->print(*_ptr);
			break;
		}
		case _float: {
			float * _ptr = static_cast<float*>(_sValues[_nr]->_ptr);
			_serial->print(*_ptr,6);
			break;
		}
		case _string: {
			//string * _ptr = (string)_sValues[_nr].ptr;
			//_serial->print(string);
			break;
		}
		case _color: {
			uint32_t * _ptr = (uint32_t*)_sValues[_nr]->_ptr;
			_serial->print(*_ptr);
			break;
		}
		default: {
			// to do
		break;
		}
	}
}

void usai::processGraph(int _nr)
{
	// sample rate
	_serial->print(_sValues[_nr]->_name);
	_serial->print(":");
	printValue(_nr);
	_serial->println("");
}

void usai::processControl(int _nr)
{

}

void usai::processConfig(int _nr)
{

}

uValue::uValue(const char * Name, cmdType Type, void * PTR, dataType DataType/*=_void*/)
{
	_cmd = Type;
	_dataType = DataType;
	_ptr = PTR;
	_name = Name;
}

uValue::uValue(const char * Name, cmdType _Type, boolean *ptr)
{
	setValue(Name,_Type,ptr,_bool);
}

uValue::uValue(const char * Name, cmdType _Type, char *ptr)
{
	setValue(Name,_Type,ptr,_char);
}

uValue::uValue(const char * Name, cmdType _Type, int *ptr)
{
	setValue(Name,_Type,ptr,_int);
}

uValue::uValue(const char * Name, cmdType _Type, long *ptr)
{
	setValue(Name,_Type,ptr,_long);
}

uValue::uValue(const char * Name, cmdType _Type, float *ptr)
{
	setValue(Name,_Type,ptr,_float);
}

uValue::uValue(const char * Name, cmdType _Type, uint32_t *ptr)
{
	setValue(Name,_Type,ptr,_color);
}


void uValue::setValue(const char * Name, cmdType Type, void *PTR, dataType DataType/*=_void*/)
{
	_cmd = Type;
	_dataType = DataType;
	_ptr = PTR;
	_name = Name;
	setMinMax();
}

void uValue::setValue(String val)
{
	switch(_dataType)
	{
		case _bool:
		{
			bool * _valueptr = static_cast<bool*>(_ptr);
			*_valueptr = val.toInt();
			break;
		}
		case _char:
		{
			char * _valueptr = static_cast<char*>(_ptr);
			*_valueptr = val.charAt(0);
			break;
		}
		case _int:
		{
			int * _valueptr = static_cast<int*>(_ptr);
			*_valueptr = val.toInt();
			break;
		}
		case _long:
		{
			int * _valueptr = static_cast<int*>(_ptr);
			*_valueptr = val.toInt();
			break;
		}
		case _float:
		{
			float * _valueptr = static_cast<float*>(_ptr);
			*_valueptr = val.toFloat();
			break;
		}
		case _color:
		{
			uint32_t * _valueptr = static_cast<uint32_t*>(_ptr);
			*_valueptr = val.toInt();
			break;
		}
		default: {
			// to do
			break;
		}
	}
}

void uValue::setMinMax(int min, int max)
{
	_minimum = min;
	_maximum = max;
}

void uValue::setMinMax()
{
	switch(_dataType)
	{
		case _bool:
		{
			setMinMax(0,1);
			break;
		}
		case _char:
		{
			setMinMax(0,255);
			break;
		}
		case _int:
		{
			setMinMax(0,1023);
			break;
		}
		case _long:
		{
			setMinMax(-4095,4095);
			break;
		}
		case _float:
		{
			setMinMax(-100,100);
			break;
		}
		case _color:
		{
			setMinMax(0,255);
			break;
		}
		default: {
			// to do
			break;
		}
	}
}