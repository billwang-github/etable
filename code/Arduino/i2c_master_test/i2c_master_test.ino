/* EEPROM Read Write TEst */
#include <Wire.h>
//A4 (SDA), A5 (SCL)
#define SLAVE_ADDRESS 0x38
#define SERIAL_BAUD 9600

byte c, payload[32];
int i;
int height = 40;
byte u8temp = 0x00;
int err_flag = 0, err_cnt = 0;;

void setup()
{
  Wire.begin(); // I2C
  Serial.begin(SERIAL_BAUD); // UART

  Serial.println();
  Serial.println("====================================");
  Serial.println("I2C Master Read/Write Test");
  Serial.println("====================================");

  // Read firmware version
  Serial.println();
  Serial.println("====================================");
  Serial.println("Read firmware version...");
  payload[0] = 0x00;
  Write_I2c_Bytes(1, payload);
  Read_I2c_Bytes(8, payload);
  Print_Buff(8, 'D', payload); Serial.println();
  delay(2000);

  Serial.println();
  Serial.println("====================================");
  Serial.println("Write Display Test...");

  // Set Led brightness
  Set_Led_Current(3);

  // led flash duty
  Set_Led_Duty(1000,1000);

  // Set Height string, master write
  payload[0] = 0x01;
  payload[1] = '1';
  payload[2] = '2';
  payload[3] = '3';
  Write_I2c_Bytes(4, payload);
  delay(2000);

  // Write EEPROM bytes
//  Serial.println();
//  Serial.println("====================================");
//  Serial.println("Write EEPROM bytes Test...");
//  payload[0] = 0x05;
//  payload[1] = 8; // number to write
//  payload[2] = 0; // start address
//  for (i = 0; i < 8; i++) //Read_EE_Bytes
//  {
//    payload[3 + i] = 0xA0 + i;
//  }
//  Write_I2c_Bytes(11, payload);
//  delay(100);

//  for (i = 0; i < 8; i++) //
//  {
//    payload[0] = 0x04;      //command
//    payload[1] = i;         // start address
//    payload[2] = 0xB0 + i;  // data
//    Write_I2c_Bytes(3, payload);
//    delay(10);
//  }
    
//  Serial.println();
//  Serial.println("====================================");
//  Serial.println("Read EEPROM....");
//  // Read EEPROM Buffer buffer, master read
//  payload[0] = 0x03; // 
//  payload[1] = 8; // number
//  payload[2] = 0; // address
//  Write_I2c_Bytes(3, payload);
//  //delay(100);
//  Read_I2c_Bytes(8, payload);
//  Print_Buff(8, 'H', payload); Serial.println();

//  Serial.println();
//  Serial.println("====================================");
//  Serial.println("Write Test Buffer....");
//  for (i = 0; i < 8; i++)
//  {
//    payload[0] = 0x11;
//    payload[1] = i;
//    payload[2] = 0xC0 + i;
//    Write_I2c_Bytes(3, payload);
//  }
//  Serial.println();
//  Serial.println("====================================");
//  Serial.println("Read Test Buffer....");
//  payload[0] = 0x10; // assign number of data and start address
//  payload[1] = 8;
//  payload[2] = 0;
//  Write_I2c_Bytes(3, payload);
//  delay(100);
//  Read_I2c_Bytes(8, payload);
//  Print_Buff(8, 'H', payload); Serial.println();        
}


void loop()
{
  u8temp = ~u8temp;
  Serial.println();
  Serial.println("====================================");
  Serial.print("Write EEPROM bytes Test...");
  Serial.println(u8temp);
//  for (i = 0; i < 8; i++) 
//  {
//    payload[0] = 0x04;      //command
//    payload[1] = i;         // start address
//    payload[2] = u8temp;  // data
//    Write_I2c_Bytes(3, payload);
//  }

  payload[0] = 0x05;
  payload[1] = 8; // number to write
  payload[2] = 0; // start address
  for (i = 0; i < 8; i++) //Read_EE_Bytes
  {
    payload[3 + i] = u8temp;
  }
  Write_I2c_Bytes(11, payload);
  delay(100);
  

  Serial.println();
  Serial.println("====================================");
  Serial.println("Read EEPROM....");
  // Read EEPROM Buffer buffer, master read
  payload[0] = 0x03; // 
  payload[1] = 8; // number
  payload[2] = 0; // address
  Write_I2c_Bytes(3, payload);
  //delay(100);
  Read_I2c_Bytes(8, payload);
  err_flag = 0;
  for (i = 0; i < 8; i++)
  {
    if (payload[i] != u8temp)
      err_flag = 1;
  }
  if (err_flag == 1)
  {
    err_cnt ++;
    //Serial.println(err_cnt);
    //Print_Buff(8, 'H', payload); Serial.println();
  }
  Serial.println(err_cnt);Print_Buff(8, 'H', payload); Serial.println();
  delay(500);
  
  //  Serial.println();
  //  Serial.println("====================================");
  //  Serial.println("Write Display Test 2...");
  //  payload[0] = 0x02;
  //  payload[1] = height >> 8;
  //  payload[2] = height & 0xFF;
  //  Write_I2c_Bytes(3, payload);
  //
  //  if (height >= 100)
  //    height = 40;
  //   else
  //    height++;
  //  delay(1000);
}


unsigned char Read_Key()
{
  unsigned char buff_data[1];
  buff_data[0] = 0x06; // assign number of data and start address
  Write_I2c_Bytes(1, buff_data);
  return buff_data[0];
}

void Read_I2c_Bytes(int n, unsigned char *buff_data)
{
  int i;
  Wire.requestFrom(SLAVE_ADDRESS, n);
  i = 0;
  while (Wire.available()) // clear
  {
    c = Wire.read(); // receive a byte as character
    buff_data[i] = c;
    i++;
    if (i >= n)
      break;
  }  
  //delay(10);
}

void Print_Buff(int n, uint8_t format, unsigned char *buff_data)
{
  byte c;
  int i;

  for (i = 0; i < n; i++)
  {
    c = buff_data[i];
    if (format == 'H')
    {
      Serial.print(c, HEX);        // print the character
      Serial.print(" ");
    }
    else
    {
      Serial.write(c);
    }
  }
}

void Write_I2c_Bytes(int n , unsigned char *buff_data)
{
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write(buff_data, n );
  Wire.endTransmission();
  delay(20);
}

void Set_Led_Current(unsigned char i)
{
  unsigned char buff_tx[2];
  buff_tx[0] = 0x20;
  buff_tx[1] = i;
  Write_I2c_Bytes(2, buff_tx);
}  

void Set_Led_Duty(unsigned int duty, unsigned int period)
{
  unsigned char buff_tx[5];  
  buff_tx[0] = 0x21;
  buff_tx[1] = (duty >> 8) & 0xFF;
  buff_tx[2] = duty & 0xFF;
  buff_tx[3] = (period >> 8) & 0xFF;
  buff_tx[4] = period & 0xFF;
  Write_I2c_Bytes(5, buff_tx);
}
  
