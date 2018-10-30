#ifndef _My_FUNC_H_
#define _MY_FUNC_H_

#define uint8_t unsigned char
#define uint16_t unsigned short
#define uint32_t unsigned long
#define HI 1
#define LO 0
#define ON 1
#define OFF 0

uint8_t my_strcpy(uint8_t dest[], const char source[]);
uint8_t Hex2char(uint8_t num);
uint8_t Char2hex(uint8_t num);
void utoa(uint16_t n , uint8_t *str);
uint16_t atou(uint8_t *str);
#endif //_MY_FUNC_H_
