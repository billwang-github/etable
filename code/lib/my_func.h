#ifndef _My_FUNC_H_
#define _MY_FUNC_H_
#include "HT66F319.h"

#define uint8_t unsigned char
#define uint16_t unsigned short
#define uint32_t unsigned long
#define HI 1
#define LO 0
#define ON 1
#define OFF 0
#define TRUE 1
#define FALSE 0

typedef struct {
uint8_t bit0 : 1;
uint8_t bit1 : 1;
uint8_t bit2 : 1;
uint8_t bit3 : 1;
uint8_t bit4 : 1;
uint8_t bit5 : 1;
uint8_t bit6 : 1;
uint8_t bit7 : 1;
}iar_bits;

DEFINE_SFR(iar_bits, iar1, 0x02);
#define iar1_4 iar1.bit4
#define iar1_3 iar1.bit3
#define iar1_2 iar1.bit2
#define iar1_1 iar1.bit1
#define iar1_0 iar1.bit0

//static uint16_t cnt_dly;
//#define DLY(x) for (cnt_dly= 0; cnt_dly < x; cnt_dly++) {FeedWatchDog();}  // 1023 ==> 16.5ms
#define FeedWatchDog() GCC_CLRWDT() 
#define EMI_ON() _emi = 1
#define EMI_OFF() _emi = 1

//uint16_t cnt_dly;

void Delay(uint16_t count);

/* string */
uint8_t my_strcpy(uint8_t dest[], const char source[]);
void my_strcpy1(uint8_t num, uint8_t dest[], const char source[]);
uint8_t my_strcmp(uint8_t num, uint8_t dest[], const char source[]);
uint8_t Hex2char(uint8_t num);
uint8_t Char2hex(uint8_t num);
void utoa(uint16_t n , uint8_t *str);
uint8_t strlen(uint8_t *str);
uint32_t atou(uint8_t *str);

/* EEPROM */
uint8_t EE_Write_Byte(uint8_t addr, uint8_t data);
void EE_Write(uint8_t n, uint8_t addr, uint8_t buff[]);
uint8_t EE_Read_Byte(uint8_t addr);

#endif //_MY_FUNC_H_
