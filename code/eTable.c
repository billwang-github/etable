/*	Panel with 3 digits 7-segment, 6 function key, EEPROM R/W with UART interfac
**	Written by Bill Wang@GMT 2018/10/23
*/
//#include "HT66F319.h"
#include "HT66F318.h"
#include "my_func.h"

#define BUFF_LEN 17 // plus end character '0'

#define TX 	_pc3
#define RX 	_pc4

#define SA 	_pb0
#define SB 	_pb1
#define SC 	_pb2
#define SD 	_pc6
#define SE 	_pc5
#define SF 	_pa4
#define SG 	_pa5
#define DOT _pa6
#define C1 	_pa7
#define C2	_pb3
#define C3	_pb4

#define KS1	_pc0
#define KS2	_pc1
#define KS3	_pc2 
#define KS4	_pa1
#define KS5	_pa3
#define KS6	_pb6
#define KS7	_pb5

#define FeedWatchDog() GCC_CLRWDT() //		_clrwdt( )
#define WDT_SET(x) (x == ON)? (_wdtc = 0b01010111): (_wdtc = 0b10101111) //wdtc on off

const uint8_t seg_7_table[16] ={0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
const char date_code[9] ="18102300";

uint8_t buff_tx[BUFF_LEN];;
volatile uint8_t buff_rx[BUFF_LEN];
uint8_t height_str[3] = "FFF"; // height_str[2]:MSB
//uint16_t height;
uint8_t key_stat = 0;
volatile uint8_t ptr_buff = 0; // number of the received data
volatile uint8_t com =0x01;
volatile uint16_t led_period = 900, led_duty = 450;//1sec=1953
volatile bit rx_received = 0;	
volatile bit key_cont = 0;

void Init_System(void);
void Init_Vars(void);

void Led_Current_Set(uint8_t level);
void Digit_Set(uint8_t led);
void Com_Sel(uint8_t sel);
void Led_scan(void);

uint8_t EE_Write(uint8_t addr, uint8_t data);
uint8_t EE_Read(uint8_t addr);

void RS232_SendBuf(uint8_t *buf, uint8_t size);
void Command_Parse(void);

uint8_t Key_Scan(void);
void Key_Tx(void);

void Delay(unsigned int count);

void main()
{	
	//volatile uint8_t i = 0 , len = 0;
	
	Init_Vars();
	Init_System();
			
	RS232_SendBuf(buff_tx, my_strcpy(buff_tx, "GMT VER "));	// send version information
	RS232_SendBuf(buff_tx, my_strcpy(buff_tx, date_code) + 1);	
	
//	for (i = 0; i < 7; i++)
//	{
//		EE_Write(i, 0xB1+i);
//	}
	
/* Interrupt enable */	
	_emi = 1;
		
	while (1)
	{		
		if (rx_received == 1)
		{ 
			rx_received = 0;			
			Command_Parse();	
		}
		if (key_cont == 1)
			Key_Tx();
		FeedWatchDog();
	}
}

void Command_Parse(void)
{
	volatile uint8_t ee_addr, ee_data, u8temp, i, n;

	if ((buff_rx[0] == 'A') && (buff_rx[1] == '0')) // welcome test
	{
		RS232_SendBuf(buff_tx, my_strcpy(buff_tx, "@WELCOME") + 1);
	}
	else if ((buff_rx[0] == 'A') && (buff_rx[1] == '1')) // A1: set height
	{
		height_str[2] = buff_rx[2];
		height_str[1] = buff_rx[3];
		height_str[0] = buff_rx[4];
		//height = atou(height_str);
		my_strcpy(buff_tx, "@A1");
		buff_tx[3] = height_str[2];
		buff_tx[4] = height_str[1];
		buff_tx[5] = height_str[0];
		buff_tx[6] = 0;
		RS232_SendBuf(buff_tx, 7);
	}
	else if ((buff_rx[0] == 'A') && (buff_rx[1] == '2')) // A2: read key status
	{
		if (buff_rx[2] == '0')
			key_cont = 0;
		else
			key_cont = 1;	
		Key_Tx();
	}
	else if ((buff_rx[0] == 'A') && (buff_rx[1] == '3')) // A3: read EEPROM
	{
		n = Char2hex(buff_rx[2]);
		if (n > 7)
			n =7;
		ee_addr = (Char2hex(buff_rx[3]) << 4) | Char2hex(buff_rx[4]);
		my_strcpy(buff_tx, "@A3");		
		for (i = 0; i < n; i++)
		{
			ee_data = EE_Read(ee_addr + i);
			buff_tx[3 + i * 2] = Hex2char((ee_data >> 4) & 0x0F);
			buff_tx[4 + i * 2] = Hex2char(ee_data & 0x0F);			
		}
		buff_tx[n * 2 + 3] = 0;
		RS232_SendBuf(buff_tx, n * 2 + 4);						
	}
	else if ((buff_rx[0] == 'A') && (buff_rx[1] == '4')) // A4: Write EEPROM
	{
		ee_addr = (Char2hex(buff_rx[2]) << 4) | Char2hex(buff_rx[3]);
		ee_data = (Char2hex(buff_rx[4]) << 4) | Char2hex(buff_rx[5]);
		EE_Write(ee_addr, ee_data);
		ee_data = 0xFF;
		ee_data = EE_Read(ee_addr);
		my_strcpy(buff_tx, "@A4");	
		buff_tx[3] = Hex2char((ee_data >> 4) & 0x0F);
		buff_tx[4] = Hex2char(ee_data & 0x0F);
		buff_tx[5] = 0;
		RS232_SendBuf(buff_tx, 6);						
	}	
	else if ((buff_rx[0] == 'A') && (buff_rx[1] == '5')) // A5:SET LED Current Level
	{
		u8temp = Char2hex(buff_rx[2]);
		Led_Current_Set(u8temp);
		my_strcpy(buff_tx, "@A5");	
		buff_tx[3] = buff_rx[2];
		buff_tx[4] = 0;
		RS232_SendBuf(buff_tx, 5);						
	}
	else if ((buff_rx[0] == 'A') && (buff_rx[1] == '6')) // A6:SET LED flash period and duty
	{
		led_duty= Char2hex(buff_rx[2]) * 1000 + Char2hex(buff_rx[3]) * 100 + Char2hex(buff_rx[4]) * 10 + Char2hex(buff_rx[5]);
		led_period= Char2hex(buff_rx[6]) * 1000 + Char2hex(buff_rx[7]) * 100 + Char2hex(buff_rx[8]) * 10 + Char2hex(buff_rx[9]);

		for (i = 0; i < 10; i++)
		{ 
			buff_tx[i] = buff_rx[i];
		}
		buff_tx[10] = 0;
		RS232_SendBuf(buff_tx,11);						
	}	
	else if ((buff_rx[0] == 'B') && (buff_rx[1] == '0')) // B1: firmware version
	{
		RS232_SendBuf(buff_tx, my_strcpy(buff_tx, "@VER"));	
		RS232_SendBuf(buff_tx, my_strcpy(buff_tx, date_code) + 1);
	}			
	else
	{
		RS232_SendBuf(buff_tx, my_strcpy(buff_tx, "@WRONG") + 1);
	}
}

void Init_System(void)
{
	WDT_SET(OFF);
	
	_acerl = 0x00; //disable ADC
	_cos = 1; //CX disabled
	_csel = 0; // disable C+, C-
	
/* I/O */
	Led_Current_Set(0);	
	// 7 segments output
	SA = 0;
	SB = 0;
	SC = 0;
	SD = 0;
	SE = 0;
	SF = 0;
	SG = 0;
	DOT = 0;
	C1 = 0;
	C2 = 0;
	C3 = 0;			
	_pac4 = 0; 
	_pac5 = 0;
	_pac6 = 0;
	_pac7 = 0;
		
	_pbc0 = 0; 
	_pbc1 = 0; 
	_pbc2 = 0;
	_pbc3 = 0;
	_pbc4 = 0;
			
	_pcc5 = 0;
	_pcc6 = 0;


	// keys, pull up
	_pcpu0 = 1;
	_pcpu1 = 1;
	_pcpu2 = 1;
	_papu1 = 1;
	_papu3 = 1;
	_pbpu5 = 1;
	_pbpu6 = 1;
	
	KS1 = 1;
	KS2 = 1;
	KS3 = 1;
	KS4 = 1;
	KS5 = 1;
	KS6 = 1;
	KS7 = 1;
	
/* UART */
	_uartf = 0; 	//interrupt enable
	_uarte = 1;
	_rie = 1; 
	
	_brgh = 1; 		// high speed
	_brg = 77; 		// 9600 bps
	_uarten = 1; 	// pins switched to TX,RX
	_txen = 1;	
	_rxen = 1;
	
/* Time base */
	_tbc = 0b11000001; //fsys(12MHz)/4, tb1=4096/ftb = 732Hz, tb0=512/ftb==5.9kHz 
	_tb0f = 0;
	_tb0e = 1;
	_tb1f = 0;
	_tb1e = 1;	
}

void Init_Vars(void)
{
	uint8_t i;
	height_str[0] = 'F';	
	height_str[1] = 'F';	
	height_str[2] = 'F';
//	height = 888;	
	key_stat = 0;
	ptr_buff = 0;
	rx_received = 0;
	for (i = 0; i < BUFF_LEN ; i++)
	{
		buff_tx[i] = 0;
		buff_rx[i] = 0;
	}
}

void Delay(unsigned int count) //1000 => 3ms
{
    volatile unsigned int i;
    for(i=0;i<count;i++)
    {
    	FeedWatchDog();
        //asm("nop");
    }
}

void Led_Current_Set(uint8_t level)
{
	if (level == 1) //7mA
	{
		_sledc0 =0b01010101;	
		_sledc1 =0b01010101;	
	}
	else if (level == 2) // 10mA
	{
		_sledc0 =0b10101010;	
		_sledc1 =0b10101010;	
	}	
	else if (level == 3) // 22mA
	{
		_sledc0 =0b11111111;	
		_sledc1 =0b11111111;	
	}
	else	// 4mA
	{
		_sledc0	= 0x00;
		_sledc1 = 0x00;		
	}		
}
void Digit_Set(uint8_t led)
{
	SA = (led >> 0) & 0x01;
	SB = (led >> 1) & 0x01;
	SC = (led >> 2) & 0x01;
	SD = (led >> 3) & 0x01;
	SE = (led >> 4) & 0x01;
	SF = (led >> 5) & 0x01;
	SG = (led >> 6) & 0x01;
	DOT = (led >> 7) & 0x01;	
}

void Com_Sel(uint8_t sel)
{ 
	C1 = 0;
	C2 = 0;
	C3 = 0;
	
	if (sel == 0)
	{
		C1 = 1;
	}
	else 	if (sel == 1)
	{
		C2 = 1;
	}
	else 	if (sel == 2)
	{
		C3 = 1;
	}
}

void Led_Scan(void)
{
	uint8_t digit = 'O';
	static uint32_t led_tmr = 0;
	
/* scan the 7-segment display */
	Digit_Set(0x00);
	
	if (led_tmr <= led_duty)
		Com_Sel(com);
	else
		Com_Sel(8);	

	digit = height_str[com];	
	if ((digit >= '0') && (digit <= '9'))
	{
		digit = digit - '0';
		Digit_Set(seg_7_table[digit]);
	}
	else if ((digit >= 'A') && (digit <= 'F'))
	{
		digit = digit - 'A' + 10;
		Digit_Set(seg_7_table[digit]);
	}
	
	if (com >= 2)
	{	
		com = 0;
		if (led_tmr >= led_period)
			led_tmr = 0;
		else
			led_tmr++;
	}
	else
		com ++;	
}	
	
uint8_t EE_Write(uint8_t addr, uint8_t data)
{
	_eea = addr;
	_eed = data;
	_mp1 = 0x40; //EEC control register is located at address 40H in Bank 1
	_bp = 0x01;
	_emi = 0;
	_iar1 |= 0x08; //WREN
	_iar1 |= 0x04; //WR
	_emi =1;
	while ((_iar1 & 0x04) == 0x04) // check if write cycle finished
	{
		FeedWatchDog();
	}
	_iar1 = 0;
	_bp = 0;
	return 1;	
}

uint8_t EE_Read(uint8_t addr)
{
	_eea = addr;
	_mp1 = 0x40; //EEC control register is located at address 40H in Bank 1
	_bp = 0x01;
	_emi = 0;
	_iar1 |= 0x02; //RDEN
	_iar1 |= 0x01; //RD
	_emi =1;
	while ((_iar1 & 0x01) == 0x01) // check if write cycle finished
	{
		FeedWatchDog();
	}
	_iar1 = 0;
	_bp = 0;
	return _eed;
}

uint8_t Key_Scan(void)
{
	return ~(0x80 | (KS7 << 6) | (KS6 << 5) | (KS5 << 4) | (KS4 << 3) | (KS3 << 2) | (KS2 << 1) | (KS1 << 0));
}

void Key_Tx(void)
{
		my_strcpy(buff_tx, "@A2");	
		buff_tx[3] = Hex2char((key_stat >> 4) & 0x0F);
		buff_tx[4] = Hex2char(key_stat & 0x0F);
		buff_tx[5] = 0;
		RS232_SendBuf(buff_tx, 6);
}	

void RS232_SendBuf(uint8_t *buf, uint8_t size)
{
	uint8_t i = 0;
	if (size > BUFF_LEN) size = BUFF_LEN;
	//_txen = 1;
	for (i = 0; i < size; i++)
	{
		while (_txif == 0) { FeedWatchDog();}  // wait txif
		_txr_rxr = *(buf + i);
		while (_tidle == 0) {FeedWatchDog();}	// wait tx idle
	}
	//_txen = 0;
}



/*---------------------------------------------------------------
	Interrupt Subroutines
-----------------------------------------------------------------
*/
DEFINE_ISR(Int_Tb0, 0x1C) //11.718kHz 
{			
	
	FeedWatchDog();
	Led_Scan();	
	_tb0f = 0;
}

DEFINE_ISR(Int_Tb1, 0x20) //732Hz
{		
	FeedWatchDog();	
//	Led_Scan();	
/* Read Key */	
	key_stat = Key_Scan();	
			
	_tb1f = 0;
}

DEFINE_ISR(Int_Rx, 0x2C)
{
	volatile uint8_t temp, i;
	
	FeedWatchDog();
	
	if (_rxif == 1)
	{
		temp = _txr_rxr;
		if (temp == '@')
		{
			ptr_buff = 0;
			rx_received = 0;				
			for (i = 0; i < BUFF_LEN ; i++)
			{
				buff_rx[i] = 0;
			}			
		}
		else if ((temp == 0) || (temp == '#'))
		{
			rx_received = 1;				
		}
		else
		{
			if (ptr_buff < BUFF_LEN)
			{
				buff_rx[ptr_buff] =	temp;
				ptr_buff ++;
			}
		}
	}
	
	_uartf = 0;
}
