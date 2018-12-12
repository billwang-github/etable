/*
181206:
1. modify EE_Write_Byte(): inline assembly, WREN & WE
2. modify EE_Read_Byte(): inline assembly, REN & RE
181207:
1. modify atou()
*/
#include "my_func.h"

void Delay(uint16_t count) //1000 => 3ms
{
    volatile uint16_t i;
    for(i=0;i<count;i++)
    {
    	FeedWatchDog();
    }
}

/* Copy String */
uint8_t my_strcpy(uint8_t dest[], const char source[])
{
	int i = 0;
	while (source[i] != '\0')
	{
		dest[i] = (uint8_t)source[i];
		if (i > 16)
			break;
		else
			i++;
		FeedWatchDog();
	}
	dest[i] = '\0';
	return i; // not include '0'
}

/* Copy String with fixed number*/
void my_strcpy1(uint8_t num, uint8_t dest[], const char source[])
{
	int i = 0;
	for (i=0 ; i < num ; i++)
	{
		dest[i] = (uint8_t)source[i];
	}
}

/* Compare 2 strings */
uint8_t my_strcmp(uint8_t num, uint8_t dest[], const char source[])
{
	int i = 0;
	for (i=0 ; i < num ; i++)
	{
		if (dest[i] != (uint8_t)source[i])
			return FALSE;			
	}
	return TRUE;
}

/* Convert number to Hex String
	1 => '1'  0x0A -> 'A'
*/
uint8_t Hex2char(uint8_t num)
{
	volatile uint8_t u8temp;
	
	u8temp = num & 0x0F;
	if (u8temp <= 9)
	{			
		return u8temp + '0';
	}
	else 
	{
		return (u8temp - 10) + 'A';
	}	
}

/* Convert Hex char to number 
	'1' -> 1 'A' -> 0x0A
*/
uint8_t Char2hex(uint8_t num)
{
	if ((num >= '0') && (num <= '9'))
	{
		return (num - '0');	
	}
	else if ((num >= 'A') && (num <= 'F'))
	{
		return ((num - 'A') + 10);
	}
	else
		return 0;
}

/* decimal to string, n=123 , str[2~0] = "123" */
void utoa(uint16_t n , uint8_t *str)  
{
	uint8_t i;
	volatile uint16_t temp;
	
	temp = n;
	if (n ==0 )
	{
		for (i = 0; i < 3; i++)
			str[i] = '0';
	}
	else
	{
		for (i = 0; i < 3; i++)
		{
			str[i] = "0123456789"[temp%10];	// str[2]:MSB
			temp /= 10;
		}	
	}
}

uint8_t strlen(uint8_t *str)
{
	uint8_t n = 0;
	while (str[n] != '\0')
	{
		if (n > 32)
			break;
		else
			n++;
		FeedWatchDog();
	}	
	return n;	
}


/* string to decimal,str[3~0] = "1234", n=1234  */
uint32_t atou(uint8_t *str) 
{
	uint8_t n = 0, i = 0, j = 0;
	volatile uint32_t sum = 0, u32temp = 0, dig10 = 0;
	
	n= strlen(str);
	
	for (i = 0; i < n; i++)
	{
		u32temp = Char2hex(str[n - 1 - i]);
		dig10 = 1;
		for (j = 0; j < i; j++)
			dig10 *= 10;
		u32temp *= dig10;
		sum += u32temp;
	}
	return sum;
}


uint8_t EE_Write_Byte(uint8_t addr, uint8_t data)  // please add 5ms delay after next write
{
	uint16_t i = 0xFFFF;
	
	_eea = addr;
	_eed = data;
	_mp1 = 0x40; //EEC control register is located at address 40H in Bank 1
	_bp = 0x01;
	_emi = 0;
	iar1_3 = 1; iar1_2 = 1;
	_emi =1;
	while ((_iar1 & 0x04) == 0x04) // check if write cycle finished
	//while (iar1_2 == 1)
	{		
		GCC_CLRWDT();
		i--;
		if (i == 0)
		{
			break;
		}
	}
	_iar1 = 0;
	_bp = 0;
	if (i == 0)
		return 0;
	else
		return 1;	
}

void EE_Write(uint8_t n, uint8_t addr, uint8_t *buff)
{
	uint8_t i;
	for (i = 0; i < n; i++)
	{
		if (EE_Write_Byte(addr + i, buff[i]) == 0)
			break;	
	}
}

uint8_t EE_Read_Byte(uint8_t addr)
{	
	uint16_t i = 0xFFFF;	
	_eea = addr;
	_mp1 = 0x40; //EEC control register is located at address 40H in Bank 1
	_bp = 0x01;
	_emi = 0;
	iar1_1 = 1; iar1_0 = 1;
	_emi =1;
	while ((_iar1 & 0x01) == 0x01) // check if write cycle finished
	//while (iar1_0 == 1)
	{
		GCC_CLRWDT();	
		i--;
		if (i == 0)
			break;		
	}
	_iar1 = 0;
	_bp = 0;
	return _eed;
}

