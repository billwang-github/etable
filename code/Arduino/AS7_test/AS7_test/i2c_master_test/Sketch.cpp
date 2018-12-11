/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

/* EEPROM Read Write TEst */
#include <Wire.h>
//Beginning of Auto generated function prototypes by Atmel Studio
//End of Auto generated function prototypes by Atmel Studio


//A4 (SDA), A5 (SCL)
#define SLAVE_ADDRESS 0x38
#define SERIAL_BAUD 9600

byte c, payload[16],buff_ee[16];
int i, j;
byte u8temp = 0x00;

void setup()
{
  Wire.begin();
  Serial.begin(SERIAL_BAUD);
  Serial.println("I2C Master Read/Write Test");

  // Set Height, master write
  Wire.beginTransmission(SLAVE_ADDRESS);
  payload[0] = 0x01;
  payload[1] = '1';
  payload[2] = '2';
  payload[3] = '3';
  Wire.write(payload, 4);
  Wire.endTransmission();
}


void loop()
{
  u8temp = ~u8temp;
  // Write EEPROM, master write
  Serial.print("Write EEPROM....");
  for (i = 0; i < 16; i++)
  {
    Wire.beginTransmission(SLAVE_ADDRESS);
    payload[0] = 0x04;
    payload[1] = i;
    //payload[2] = 0xC0 + i;
    payload[2] = u8temp;
    Wire.write(payload, 3);
    Wire.endTransmission();
    delay(5);
  }
  Serial.println(payload[2],HEX);
    
  // Read EEPROM, master read  
  Serial.println("Read EEPROM....");
  Wire.beginTransmission(SLAVE_ADDRESS);
  payload[0] = 0x03; // assign number of data and start address
  payload[1] = 16;
  payload[2] = 0;
  Wire.write(payload, 3);
  Wire.endTransmission();

 Wire.requestFrom(SLAVE_ADDRESS, 16);
 j = 0;
 while (Wire.available()) // slave may send less than requested
 {
   byte c = Wire.read(); // receive a byte as character
   buff_ee[j++] = c;
   Serial.print(c, HEX);        // print the character
   Serial.print(" ");
 }
 Serial.println();      // prints another carriage return
 delay(500);
  
}
