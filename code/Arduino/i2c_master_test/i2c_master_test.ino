/* EEPROM Read Write TEst */
#include <Wire.h>
//A4 (SDA), A5 (SCL)
#define SLAVE_ADDRESS 0x38
#define SERIAL_BAUD 9600

byte payload[16];
int i;
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

  // Write EEPROM, master write
  Serial.println("Write EEPROM....");
  for (i = 0; i < 16; i++)
  {
    Wire.beginTransmission(SLAVE_ADDRESS);  
    payload[0] = 0x04;
    payload[1] = i;
    //payload[2] = 0xA0 + i;
    payload[2] = 0x00;
    while (Wire.write(payload, 3) != 3);
    Wire.endTransmission();  
    delay(100);
  }
  // Read EEPROM, master read
  Serial.println("Read EEPROM....");  
  Wire.beginTransmission(SLAVE_ADDRESS);
  payload[0] = 0x03; // assign number of data and start address
  payload[1] = 16;
  payload[2] = 0;
  Wire.write(payload, 3);
  Wire.endTransmission();
   
  Wire.requestFrom(SLAVE_ADDRESS, 16);
  while (Wire.available()) // slave may send less than requested
  { 
    byte c = Wire.read(); // receive a byte as character
    Serial.print(c,HEX);         // print the character
    Serial.print(" ");
  }
  Serial.println();      // prints another carriage return
}


void loop()
{  
  u8temp = ~u8temp;
  
  // Write EEPROM, master write
  
  Serial.println("Write EEPROM....");
  for (i = 0; i < 16; i++)
  {
    Wire.beginTransmission(SLAVE_ADDRESS);  
    payload[0] = 0x04;
    payload[1] = i;
    //payload[2] = 0xA0 + i;
    payload[2] = u8temp;
    Wire.write(payload, 3);
    Wire.endTransmission();  
    delay(100);
  }
  
  
  // Read EEPROM, master read
  Serial.println("Read EEPROM....");  
  Wire.beginTransmission(SLAVE_ADDRESS);
  payload[0] = 0x03; // assign number of data and start address
  payload[1] = 16;
  payload[2] = 0;
  Wire.write(payload, 3);
  Wire.endTransmission();
  
  delay(100);

  //Clear the I2C buffer
  while (Wire.available()) // slave may send less than requested
  { 
    byte c = Wire.read(); // receive a byte as character
  }
  Wire.requestFrom(SLAVE_ADDRESS, 16);
  while (Wire.available()) // slave may send less than requested
  { 
    byte c = Wire.read(); // receive a byte as character
    Serial.print(c,HEX);         // print the character
    Serial.print(" ");
  }
  Serial.println();      // prints another carriage return
}
