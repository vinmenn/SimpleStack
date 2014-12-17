//-------------------------------------------------------------------------------------
// SimpleStack: a stack library
//
// History
//  0.1.0 14/06/2014:   First public code release
//
// License:
// "MIT Open Source Software License":
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in the
// Software without restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
// and to permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//-------------------------------------------------------------------------------------

#ifndef SIMPLESTACK_H
#define SIMPLESTACK_H
#define LIBRARY_VERSION_SIMPLESTACK_H "0.0.1"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class SimpleStack {
	private:
		uint16_t _buffer_size;	// Buffer length
		uint8_t* _data;			// Data buffer
		uint16_t _readIndex;	// Read index
		uint16_t _writeIndex;	// Write index
	public:
		//-------------------------------------------------------
		//Init routines
		//-------------------------------------------------------
		SimpleStack();
		~SimpleStack();
        void begin(uint16_t buffer_size);
		inline void clear()
		{
			clearReadIndex();
			clearWriteIndex();
		}
		void clearWriteIndex();
		void clearReadIndex();
		void setData(uint8_t* array, uint16_t size);
        
		//-------------------------------------------------------
		//Write routines
		//-------------------------------------------------------
		inline uint16_t writeIndex()
		{
			return _writeIndex;
		}
		void pushByte(uint8_t value);
		void pushInteger(uint16_t value);
		void pushLong(uint32_t value);
		void pushFloat(float value);
		void pushString(char* name);
		void pushFixedString(char* name, uint8_t length);
		void pushArray(uint8_t* array, uint16_t length);
		//-------------------------------------------------------
		//Read routines
		//-------------------------------------------------------
		inline uint16_t readIndex()
		{
			return _readIndex;
		}
		uint8_t* getData();
		uint8_t popByte();
		uint16_t popInteger();
		uint32_t popLong();
		float popFloat();
		void popString(char* string);
		void popFixedString(char* fixedString, uint8_t length);
		char** popFixedStringArray(uint8_t count, uint8_t length);
        void popArray(uint8_t* array, uint16_t* length);
};
//Start with no buffer
SimpleStack::SimpleStack()
{
	_data = 0;
	_buffer_size = 0;
	_readIndex = 0;
	_writeIndex = 0;
}
//Release resources
SimpleStack::~SimpleStack()
{
	if (_data)
		free(_data);
}
//Initialize stack
void SimpleStack::begin(uint16_t buffer_size)
{
    if (_data != 0)
        free(_data);

    _data = (uint8_t*) malloc(buffer_size);
	_buffer_size = buffer_size;
	_readIndex = 0;
	_writeIndex = 0;
}
//Fill stack with a buffer array
void SimpleStack::setData(uint8_t* array, uint16_t size)
{
	if (size <= _buffer_size)
	{
		for(uint16_t i=0;i < size;i++) {
			_data[i] =  array[i]; 
		}
		_writeIndex = size;
	 }
}
//Clear write index to push values into stack
void SimpleStack::clearWriteIndex()
{
	_writeIndex = 0;
}
//Push a byte value
void SimpleStack::pushByte(uint8_t value)
{
	if (_writeIndex<_buffer_size)
		_data[_writeIndex++] = value;
}
//Push a 16 bit value
void SimpleStack::pushInteger(uint16_t value)
{
	if ((_writeIndex+1)<_buffer_size)
	{
		_data[_writeIndex++] = (value >> 8);
		_data[_writeIndex++] = value & 0xff;
	}
}
//Push a 32 bit value
void SimpleStack::pushLong(uint32_t value)
{
	if ((_writeIndex+3)<_buffer_size)
	{
		_data[_writeIndex++] = (value >> 24) & 0xff;
		_data[_writeIndex++] = (value >> 16) & 0xff;
		_data[_writeIndex++] = (value >> 8) & 0xff;
		_data[_writeIndex++] = value & 0xff;
	}
}
//Push a float value (4 bytes)
void SimpleStack::pushFloat(float value)
{
	if ((_writeIndex+3)<_buffer_size)
	{
        //float temp = value;
		uint8_t *p = (uint8_t*)&value;
		
		for (int i = 0; i < sizeof(value); i++) 
			_data[_writeIndex++] = p[i];
        
	}
}
//Push a fixed length string
void SimpleStack::pushFixedString(char* name, uint8_t length)
{
	if ((_writeIndex + length-1) < _buffer_size)
		for(uint8_t i = 0;i < length;i++)
			_data[_writeIndex++] =  name[i]; 

}
//Push a null terminated string
void SimpleStack::pushString(char* name)
{
	uint8_t i = 0;
	while(name[i] != 0 && _writeIndex < _buffer_size)
		_data[_writeIndex++] =  name[i++]; 
    //Add termination
    _data[_writeIndex++] = 0;
    
}
//Push a byte array with it's length
void SimpleStack::pushArray(uint8_t* array, uint16_t length)
{
	if ((_writeIndex + length + 1) >= _buffer_size)
		return;

	_data[_writeIndex++] = (length >> 8) & 0xff; 
	_data[_writeIndex++] = length & 0xff; 

	if (length > 0)
	{
		for(uint16_t i=0;i < length;i++) {
			_data[_writeIndex++] =  array[i]; 
		}
	}
}

//Clear read index to pop out values
void SimpleStack::clearReadIndex()
{
	_readIndex = 0;
}
//Get all buffe array
uint8_t* SimpleStack::getData()
{
	return _data;
}
// Get 1 byte from stack
uint8_t SimpleStack::popByte()
{
	return _data[_readIndex++]; 
}
// Get integer (2 bytes) from stack 
uint16_t SimpleStack::popInteger()
{
	uint16_t value = (_data[_readIndex++] << 8);
	value |= 		 (_data[_readIndex++]);
	
	return value; 
}
// Get long (4 bytes) from stack
uint32_t SimpleStack::popLong()
{
	uint32_t value = ((uint32_t) _data[_readIndex++]) << 24;
    value |= ((uint32_t) _data[_readIndex++]) << 16;
    value |= ((uint32_t) _data[_readIndex++]) << 8;
    value |= ((uint32_t) _data[_readIndex++]);
    
	return value; 
}
// Get float (4 bytes) from stack
float SimpleStack::popFloat()
{
	uint32_t value = ((uint32_t) _data[_readIndex++]);
    value |= ((uint32_t) _data[_readIndex++]) << 8;
    value |= ((uint32_t) _data[_readIndex++]) << 16;
    value |= ((uint32_t) _data[_readIndex++]) << 24;

    float f = *((float*) &value);

    return f;
}
// Get fixed string from stack
void SimpleStack::popFixedString(char* fixedString, uint8_t length)
{
	//char * name = (char *) malloc(length+1);
	if (length < _buffer_size) {
		for(uint8_t i=0; i < length; i++) 
			fixedString[i] = _data[_readIndex++];
		
		fixedString[length] = 0;
	}
}
// Get null terminated string from stack
void SimpleStack::popString(char* string)
{
	uint8_t i = 0;
	
	while(_data[_readIndex] != 0 && _readIndex < _buffer_size)
		string[i++] = _data[_readIndex++];
        
    string[i++] = _data[_readIndex++];
    
}
// Get array of fixed strings from stack
char** SimpleStack::popFixedStringArray(uint8_t count, uint8_t length)
{
	char* names[count];
	
	for(uint8_t n = 0; n < count; n++)
	{
		popFixedString(names[n], length);
	}
	
	return names; 
}
//Pop a byte array with it's length
void SimpleStack::popArray(uint8_t* array, uint16_t* length)
{
    *length  = _data[_readIndex++] << 8;
    *length |= _data[_readIndex++] & 0xff;

	if (*length == 0 || array == 0)
        return;
        
    for(uint16_t i=0;i < *length;i++) {
    
        array[i] = _data[_readIndex++];
        
    }
}

#endif