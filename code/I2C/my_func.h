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
uint16_t atou(uint8_t *str);

#endif //_MY_FUNC_H_
