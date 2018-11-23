#include "my_func.h"

void Delay(uint16_t count) //1000 => 3ms
{
    volatile uint16_t i;
    for(i=0;i<count;i++)
    {
    	FeedWatchDog();
    }
}

uint8_t my_strcpy(uint8_t dest[], const char source[])
{
	int i = 0;
	while (source[i] != '\0')
	{
		dest[i] = (uint8_t)source[i];
		i++;
	}
		dest[i] = '\0';
	return i; // not include '0'
}

void my_strcpy1(uint8_t num, uint8_t dest[], const char source[])
{
	int i = 0;
	for (i=0 ; i < num ; i++)
	{
		dest[i] = (uint8_t)source[i];
	}
}

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


void utoa(uint16_t n , uint8_t *str) // decimal to string, n=1234 , str[3~0] = "1234"
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

uint16_t atou(uint8_t *str) //// string to decimal, str[3~0] = "1234", n=1234 
{
	return Char2hex(str[0]) + Char2hex(str[1]) * 10 + Char2hex(str[2]) * 100;
}

