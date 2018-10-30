#include "my_func.h"

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

uint8_t Hex2char(uint8_t num)
{
	if (num <= 9)
	{			
		return num + '0';
	}
	else 
	{
		return (num - 10) + 'A';
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

uint16_t atou(uint8_t *str)
{
	return Char2hex(str[0]) + Char2hex(str[1]) * 10 + Char2hex(str[2]) * 100;
}