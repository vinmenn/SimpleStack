/*
  =======================================================
  SimpleStack: a stack library
	
  Copyright (C) 2014 Vincenzo Mennella (see license.txt)

  History
    1.0.0 14/06/2014:   First public code release
    1.1.0 11/08/2014:   added free function & setRWIndex
    1.2.0 27/08/2014:   Added pushName & popName functions
  =======================================================
*/

#ifndef SIMPLESTACK_H
#define SIMPLESTACK_H
#define VERSION_SIMPLESTACK_H "1.2.0"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <EEPROM.h>

//-------------------------------------------------------
//Function handler
//delegate read sensor to external handler
//-------------------------------------------------------
extern "C" {

	// callback function types
    typedef uint8_t (*readHandler)(int index);
    typedef void (*writeHandler)(int index, uint8_t value);
} 
//===========================================================
// Main class: push and pop values into byte arrays or EEprom
//===========================================================
class SimpleStack {
	private:
		uint16_t _buffer_size;	// Buffer length
		uint8_t* _data;			    // Data buffer
    uint8_t _type;          // type 0 = eeprom, 1 = internal buffer, 2 = external buffer
    uint8_t _nameLength;    // Fixed string name length
		uint16_t _readIndex;	  // Read index
		uint16_t _writeIndex;	  // Write index
    readHandler _read;      // External read routine
    writeHandler _write;    // External write routine
        
    uint8_t read(int index);
    void write(int index, uint8_t value);
        
	public:
    //===========================================================
		//Initialization routines
    //===========================================================
		SimpleStack();
    SimpleStack(readHandler, writeHandler);
		~SimpleStack();
    void begin(uint8_t nameLength);
    void begin(uint16_t buffer_size, uint8_t nameLength);
    void begin(uint8_t* buffer, uint16_t buffer_size, uint8_t nameLength);
    void dispose();
		inline void clear()
		{
      _writeIndex = 0;
      _readIndex = 0;
		}
		inline void clearWriteIndex() {
        _writeIndex = 0;
    }
		inline void clearReadIndex() {
        _readIndex = 0;
    }
		inline void setWriteIndex(uint16_t start) {
        _writeIndex = start;
    }
		inline void setReadIndex(uint16_t start) {
        _readIndex = start;
    }
		void setData(uint8_t* array, uint16_t size);

    //===========================================================
		//Write routines
    //===========================================================
		inline uint16_t writeIndex() { return _writeIndex; }
		void pushByte(uint8_t value);
		void pushInteger(uint16_t value);
		void pushLong(uint32_t value);
		void pushFloat(float value);
		void pushString(char* name);
		void pushArray(uint8_t* array, uint16_t length);
		void pushFixedString(char* name, uint8_t length);
		void pushFixedStringArray(char** strings, uint8_t count, uint8_t length);
    void pushName(char *name);
    //===========================================================
		//Read routines
    //===========================================================
		inline uint16_t readIndex() { return _readIndex; }
		uint8_t* getData();
		uint8_t popByte();
		uint16_t popInteger();
		uint32_t popLong();
		float popFloat();
		void popString(char* string);
		void popFixedString(char* fixedString, uint8_t length);
		void popFixedStringArray(char** strings, uint8_t count, uint8_t length);
    void popArray(uint8_t* array, uint16_t* length);
    void popName(char* name);
};
// -------------------------------------------------------
//Start SimpleStack without buffer
// -------------------------------------------------------
SimpleStack::SimpleStack()
{
    _read = 0;
    _write = 0;
	_data = 0;
	_buffer_size = 0;
	_readIndex = 0;
	_writeIndex = 0;
}
// -------------------------------------------------------
// Initialize stack with read/write handlers
// -------------------------------------------------------
SimpleStack::SimpleStack(readHandler read, writeHandler write)
{
  _read = read;
  _write = write;
	_data = 0;
	_buffer_size = 0;
	_readIndex = 0;
	_writeIndex = 0;
}
// -------------------------------------------------------
//Release resources
// -------------------------------------------------------
SimpleStack::~SimpleStack()
{
    dispose();
}
// -------------------------------------------------------
//Initialize stack with different boards
// -------------------------------------------------------
void SimpleStack::begin(uint8_t nameLength)
{
    _nameLength = nameLength;
    _buffer_size = 128; //TODO: Set different values fo different boards
    _write = 0;
    _type = 0;
    _readIndex = 0;
    _writeIndex = 0;
}
// -------------------------------------------------------
// Initialize stack with buffer array and default name length
// -------------------------------------------------------
void SimpleStack::begin(uint16_t buffer_size, uint8_t nameLength)
{
    _nameLength = nameLength;
	if (_data && !_type==2)
		free(_data);
    _data = (uint8_t*) malloc(buffer_size);
	_buffer_size = buffer_size;
    _nameLength = nameLength;

    _type = 1;
	_readIndex = 0;
	_writeIndex = 0;
}
// -------------------------------------------------------
// Initialize stack with external buffer array and 
// default name length
// -------------------------------------------------------
void SimpleStack::begin(uint8_t* buffer, uint16_t buffer_size, uint8_t nameLength)
{
	if (_data && !_type==2)
		free(_data);
    _data = buffer;
	_buffer_size = buffer_size;
    _nameLength = nameLength;

    _type = 2;
	_readIndex = 0;
	_writeIndex = 0;
}
//-------------------------------------------------------
// Release allocated buffer array
// -------------------------------------------------------
void SimpleStack::dispose()
{
	if (_data && !_type==2)
		free(_data);
}
// -------------------------------------------------------
//Fill stack with a buffer array
// -------------------------------------------------------
void SimpleStack::setData(uint8_t* array, uint16_t size)
{
	if (size <= _buffer_size)
	{
		for(uint16_t i=0;i < size;i++) {
            write(i, array[i]);
		}
		_writeIndex = size;
	 }
}
// -------------------------------------------------------
//Push a byte value
// -------------------------------------------------------
void SimpleStack::pushByte(uint8_t value)
{
	if (_writeIndex<_buffer_size)
        write(_writeIndex++, value);
}
// -------------------------------------------------------
//Push a 16 bit value
// -------------------------------------------------------
void SimpleStack::pushInteger(uint16_t value)
{
	if ((_writeIndex+1)<_buffer_size)
	{
		write(_writeIndex++, value >> 8);
		write(_writeIndex++, value & 0xff);
	}
}
// -------------------------------------------------------
//Push a 32 bit value
// -------------------------------------------------------
void SimpleStack::pushLong(uint32_t value)
{
	if ((_writeIndex+3)<_buffer_size)
	{
		write(_writeIndex++, value >> 24);
		write(_writeIndex++, value >> 16);
		write(_writeIndex++, value >> 8);
		write(_writeIndex++, value & 0xff);
	}
}
// -------------------------------------------------------
//Push a float value (4 bytes)
// -------------------------------------------------------
void SimpleStack::pushFloat(float value)
{
	if ((_writeIndex+3)<_buffer_size)
	{
        //float temp = value;
		uint8_t *p = (uint8_t*)&value;
		
		for (int i = 0; i < sizeof(value); i++) 
			write(_writeIndex++, p[sizeof(value) - i -1]);
        
	}
}
// -------------------------------------------------------
//Push a fixed length string
// -------------------------------------------------------
void SimpleStack::pushFixedString(char* name, uint8_t length)
{
	if ((_writeIndex + length-1) < _buffer_size)
		for(uint8_t i = 0;i < length;i++)
			write(_writeIndex++, name[i]); 

}
// -------------------------------------------------------
// Push array of fixed strings from stack
// -------------------------------------------------------
void SimpleStack::pushFixedStringArray(char** strings, uint8_t count, uint8_t length)
{
	for(uint8_t n = 0; n < count; n++) 	{
		pushFixedString(strings[n], length);
	}
}
// -------------------------------------------------------
//Push a null terminated string
// -------------------------------------------------------
void SimpleStack::pushString(char* name)
{
	uint8_t i = 0;
	while(name[i] != 0 && _writeIndex < _buffer_size)
		write(_writeIndex++, name[i++]); 
    //Add termination
    write(_writeIndex++, 0);
    
}
// -------------------------------------------------------
// Push name (fixed string with default length) into stack
// -------------------------------------------------------
void SimpleStack::pushName(char *name)
{
    if ((_writeIndex + _nameLength-1) < _buffer_size)
        for(uint8_t i = 0;i < _nameLength;i++)
            write(_writeIndex++, name[i]); 
}
// -------------------------------------------------------
//Push a byte array with it's length
// -------------------------------------------------------
void SimpleStack::pushArray(uint8_t* array, uint16_t length)
{
	if ((_writeIndex + length + 1) >= _buffer_size)
		return;

	write(_writeIndex++, length >> 8); 
	write(_writeIndex++, length & 0xff); 

	if (length > 0)
		for(uint16_t i=0;i < length;i++) 
			write(_writeIndex++, array[i]); 
}

// -------------------------------------------------------
//Get all buffer array
// -------------------------------------------------------
uint8_t* SimpleStack::getData()
{
	return _data;
}
// -------------------------------------------------------
// Get 1 byte from stack
// -------------------------------------------------------
uint8_t SimpleStack::popByte()
{
	return read(_readIndex++); 
}
// -------------------------------------------------------
// Get integer (2 bytes) from stack 
// -------------------------------------------------------
uint16_t SimpleStack::popInteger()
{
	uint16_t value = read(_readIndex++) << 8;
	value |= 		 read(_readIndex++);
	
	return value; 
}
// -------------------------------------------------------
// Get long (4 bytes) from stack
// -------------------------------------------------------
uint32_t SimpleStack::popLong()
{
	uint32_t value = (uint32_t) read(_readIndex++) << 24;
    value |= (uint32_t) read(_readIndex++) << 16;
    value |= (uint32_t) read(_readIndex++) << 8;
    value |= (uint32_t) read(_readIndex++);
    
	return value; 
}
// -------------------------------------------------------
// Get float (4 bytes) from stack
// -------------------------------------------------------
float SimpleStack::popFloat()
{
	uint32_t value = (uint32_t) read(_readIndex++) << 24;
    value |= (uint32_t) read(_readIndex++) << 16;
    value |= (uint32_t) read(_readIndex++) << 8;
    value |= (uint32_t) read(_readIndex++);

    float f = *((float*) &value);

    return f;
}
// -------------------------------------------------------
// Get fixed string from stack
// -------------------------------------------------------
void SimpleStack::popFixedString(char* fixedString, uint8_t length)
{
	if (length < _buffer_size) {
		for(uint8_t i=0; i < length; i++) 
			fixedString[i] = read(_readIndex++);
		
		fixedString[length] = 0;
	}
}
// -------------------------------------------------------
// Get null terminated string from stack
// -------------------------------------------------------
void SimpleStack::popString(char* string)
{
	uint8_t i = 0;
	
    uint8_t r = 1;
	while(r != 0 && _readIndex < _buffer_size) {
        r = read(_readIndex++);
		string[i++] = r;
    }
}
// -------------------------------------------------------
// Get array of fixed strings from stack
// -------------------------------------------------------
void SimpleStack::popFixedStringArray(char** strings, uint8_t count, uint8_t length)
{
	for(uint8_t n = 0; n < count; n++) 	{
		popFixedString(strings[n], length);
	}
}
// -------------------------------------------------------
// Pop name (fixed string with default length) from stack
// -------------------------------------------------------
void SimpleStack::popName(char* name)
{
    if (_nameLength < _buffer_size) {
        for(uint8_t i=0; i < _nameLength; i++) 
            name[i] = read(_readIndex++);
        
        name[_nameLength] = 0;
    }
}
// -------------------------------------------------------
//Pop a byte array with it's length
// -------------------------------------------------------
void SimpleStack::popArray(uint8_t* array, uint16_t* length)
{
    *length  = read(_readIndex++) << 8;
    *length |= read(_readIndex++) & 0xff;

	if (*length == 0)
        return;
        
    if (array == 0)
        array = (uint8_t*) malloc(*length);
        
    for(uint16_t i=0;i < *length;i++) 
        array[i] = read(_readIndex++);
}
// -------------------------------------------------------
// read byte from byte array or EEprom storage
// -------------------------------------------------------
uint8_t SimpleStack:: read(int index)
{
    if (_read != 0)
        return _read(index);
    if (_type == 0)
        return EEPROM.read(index);
    return _data[index];
}
// -------------------------------------------------------
// write byte into byte array or EEprom storage
// -------------------------------------------------------
void SimpleStack:: write(int index, uint8_t value)
{
    if (_write != 0)
        _write(index, value);
    else {
        if (_type == 0)
            EEPROM.write(index, value);
        else
            _data[index] = value;
    }
}
#endif