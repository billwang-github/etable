#include "my_func.h"
#include "i2c_master.h"
void Init_I2c(void)
{
	I2c_SCL_1();
	I2c_SDA_1();
}

void I2c_SendStart(void)  
{
	// bus free time , Time the bus must be free 
	// before a new transmission can start
	//Delay_us(1); 
	// SDA H->L , SCL H
	I2c_SCL_1();
	I2c_BITDLY();
	I2c_SDA_1();
	I2c_BITDLY();
	I2c_SDA_0();
	I2c_BITDLY();
	I2c_BITDLY();
	I2c_SCL_0();
	I2c_BITDLY();
}

void I2c_SendStop(void)
{
	// SDA L->H , SCL H
	I2c_SCL_0();
	I2c_SDA_0();
	I2c_BITDLY();
	I2c_SCL_1();;
	I2c_BITDLY();
	I2c_SDA_1();	
	I2c_BITDLY();	
}

uint8_t Wait_Ack(void)
{
	uint16_t errtmr = 1024;
	I2c_SDA_1(); I2c_BITDLY(); //release SDA
	I2c_SCL_1(); I2c_BITDLY();
	while (SDA0 == 1) // wait ACK
	{
		if (--errtmr == 0)
		{
			I2c_SendStop();
			return FALSE;
		}
	}
	I2c_SCL_0();
	return TRUE;		
}

//void SendNotAck(void)
// {
//     I2c_SDA_1(); 
//     I2c_BITDLY();
//     I2c_SCL_1();
//     I2c_BITDLY();
//     I2c_SCL_0();
// }
//
//void SendAck(void)
// {
//     I2c_SDA_0(); 
//     I2c_BITDLY();
//     I2c_SCL_1();
//     I2c_BITDLY();
//     I2c_SCL_0();
// }

uint8_t I2c_SendByte(uint8_t bt) // return ACK or NAK
{
	uint8_t loop;

    for (loop = 0; loop < 8; loop++)
    {
      if ((bt & 0x80) == 0x80) 
      {
      	I2c_SDA_1();
      }
      else
      	I2c_SDA_0();      	
      I2c_BITDLY();	
      I2c_SCL_1();;
      I2c_BITDLY();
      I2c_SCL_0();     
      bt <<= 1; 
      FeedWatchDog();	    
    }	
    
	return Wait_Ack();
}

uint8_t I2c_Write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t len, uint8_t *buff) // 7 bit address
{
	uint8_t i = 0;
	
	// Send I2c Address
	EMI_OFF();
	I2c_SendStart();
	if (I2c_SendByte(i2c_addr << 1) == FALSE)
	{
		EMI_ON();
		return 0;	
	}
	// Send Register address
	if (I2c_SendByte(reg_addr) == FALSE)
	{
		EMI_ON();
		return 0;	
	}
	for (i = 0 ; i < len; i++)
	{
		if (I2c_SendByte(*(buff + i)) == FALSE)
			return 0;		
	}
	I2c_SendStop();
	EMI_ON();
	return 1;
}

uint8_t I2c_ReadByte(uint8_t ack)
{
	uint8_t i, u8temp = 0;
	
	I2c_SDA_1();
	
	for (i = 0; i < 8; i++)
	{
		I2c_SCL_1();
		u8temp <<= 1;
		if (SDA0 == 1)
			u8temp |=0x01;
		I2c_SCL_0(); I2c_BITDLY();
	}
	
	if (ack != 0) I2c_SDA_0();
	I2c_SCL_1(); I2c_BITDLY();
	I2c_SCL_0(); I2c_BITDLY();	
	I2c_SDA_1();
	return u8temp;
}

/* ==== I2C master read function ====
	mode: 0-> no reg address writting 0-> add reg address before reading
   ==================================*/
uint8_t I2c_Read(uint8_t mode, uint8_t i2c_addr, uint8_t reg_addr, uint8_t len, uint8_t *buff) // 7 bit address
{	
	EMI_OFF();
	if (mode !=0 )
	{
		// Send I2c Address	
		I2c_SendStart();
		if (I2c_SendByte(i2c_addr << 1) == FALSE) // mask write bit
		{
			EMI_ON();
			return 0;	
		}
		// Send Register address
		if (I2c_SendByte(reg_addr) == FALSE)
		{
			EMI_ON();
			return 0;	
		}
	}
	// Send I2c Address
	I2c_SendStart();
	if (I2c_SendByte((i2c_addr << 1) | 0x01) == FALSE) // mask write bit
	{
		EMI_ON();
		return 0;	
	}
		
			
	for (; len > 1; len--)
	{
		*buff++ = I2c_ReadByte(TRUE);
	}
	
	*buff = I2c_ReadByte(FALSE);
	I2c_SendStop();
	I2c_BITDLY();
	EMI_ON();
	return 1;
}
