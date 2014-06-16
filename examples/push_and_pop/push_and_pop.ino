#include <SimpleStack.h>

SimpleStack stack;

void setup()
{
    Serial.begin(38400); 
    Serial.println("SimpleStack push values program");
    Serial.println("===============================");
 
    stack.begin(255); 
}

void loop()
{
  byte bval = 0xff;
  unsigned int ival = 0x0102;
  unsigned long lval = 0x03040506;
  float fval = 9.99;
  char* fstring = "01234";
  char* string = "56789\0";
  
  Serial.println("Start to push values");
  Serial.println("--------------------");
  
  stack.clear();
  
  //Byte
  Serial.print("byte value: ");
  Serial.println(bval, HEX);
  stack.pushByte(bval);
  Serial.print("index = ");
  Serial.println(stack.writeIndex());
  
  //Integer
  Serial.print("int value: ");
  Serial.println(ival, HEX);
  stack.pushInteger(ival);
  Serial.print("index = ");
  Serial.println(stack.writeIndex());
  
  //Long
  Serial.print("long value: ");
  stack.pushLong(lval);
  Serial.println(lval, HEX);
  Serial.print("index = ");
  Serial.println(stack.writeIndex());

  //Float
  Serial.print("float value: ");
  Serial.println(fval);
  stack.pushFloat(fval);
  Serial.print("index = ");
  Serial.println(stack.writeIndex());
  
  //Fixed string  
  Serial.print("fixed string value: ");
  Serial.println(fstring);
  stack.pushFixedString(fstring, 5);
  Serial.print("index = ");
  Serial.println(stack.writeIndex());
  
  //String  
  Serial.print("string value: ");
  Serial.println(string);
  stack.pushString(string);
  Serial.print("index = ");
  Serial.println(stack.writeIndex());
  
  //Array
  Serial.print("array value: ");
  Serial.println(fstring);
  stack.pushArray((uint8_t*)fstring, 5);
  Serial.print("index = ");
  Serial.println(stack.writeIndex());
  
  Serial.println("Now pop out values");

  //Byte  
  Serial.print("byte value: ");
  Serial.println(stack.popByte(), HEX);
  Serial.print("read index = ");
  Serial.println(stack.readIndex());

  //Integer
  Serial.print("int value: ");
  Serial.println(stack.popInteger(), HEX);
  Serial.print("read index = ");
  Serial.println(stack.readIndex());

  //Long
  Serial.print("long value: ");
  Serial.println(stack.popLong(), HEX);
  Serial.print("read index = ");
  Serial.println(stack.readIndex());

  //Float
  Serial.print("float value: ");
  Serial.println(stack.popFloat());
  Serial.print("read index = ");
  Serial.println(stack.readIndex());

  //Fixed string
  char outstring[6];
  stack.popFixedString(outstring, 5);
  Serial.print("fixed string value: ");
  Serial.println(outstring);
  Serial.print("read index = ");
  Serial.println(stack.readIndex());

  //String
  stack.popString(outstring);
  Serial.print("null terminated string value: ");
  Serial.println(outstring);
  Serial.print("read index = ");
  Serial.println(stack.readIndex());

  //Array
  uint16_t length;
  stack.popArray((uint8_t*)outstring, &length);
  Serial.print("Array length: ");
  Serial.println(length);
  Serial.print("Array value: ");
  Serial.println(outstring);
  Serial.print("read index = ");
  Serial.println(stack.readIndex());

  Serial.println("--------------------");
  Serial.println("the end");

  while(true);
}