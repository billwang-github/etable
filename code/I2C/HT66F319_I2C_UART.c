/*	Panel with 3 digits 7-segment, 6 function key, EEPROM R/W with UART interfac
**	Written by Bill Wang@GMT 2018/10/23
*/
//#include "HT66F319.h"
//#include "HT66F318.h"
#include "my_func.h"

#define BUFF_LEN 16 

//#define TX 	_pc3
//#define RX 	_pc4

#define SA 	_pb0
#define SB 	_pb1
#define SC 	_pb2
#define SD 	_pc3
#define SE 	_pc4
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

#define BUZZER _pa6

#define SDA _pc5
#define SCL _pc6
#define I2C_DATA _iicd
#define I2C_ADDR (0x38 << 1)
#define I2C_ADDR_MATCH _iichaas
#define I2C_BUSY _iichbb
#define I2C_READ _iicsrw
#define I2C_TIMEOUT _i2ctof
#define I2C_INT_FLAG _iicf
#define I2C_TRANSMITTER _iichtx
#define I2C_TX_NOACK _iictxak
#define I2C_RX_NOACK _iicrxak
#define I2C_TIMEOUT_EN _i2ctoen


#define WDT_SET(x) (x == ON)? (_wdtc = 0b01010111): (_wdtc = 0b10101111) //wdtc on off

const uint8_t seg_7_table[16] ={0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
const char date_code[9] ="18120700";

volatile uint8_t com =0x01;
volatile uint16_t led_period = 900, led_duty = 900;//1sec=1953

uint16_t height = 000;
uint8_t height_str[3] = "FFF"; // height_str[2]:MSB

uint8_t key_stat = 0;

uint8_t buff_i2c_rx[BUFF_LEN], buff_i2c_tx[BUFF_LEN + 3], buff_test[8];
volatile uint8_t ptr_i2c_rx_buff = 0, ptr_i2c_tx_buff = 0; // number of the received data	

bit b_ee_data_ready = 0;

void Init_System(void);
void Init_Vars(void);

void Led_Current_Set(uint8_t level);
void Digit_Set(uint8_t led);
void Com_Sel(uint8_t sel);
void Led_scan(void);

uint8_t Key_Scan(void);
void Clear_Rx(void);

void Command_Parse(void);

void Power_On_Reset(void);

void main()
{	
	uint8_t  i;
	volatile uint32_t temp;
	
	Init_Vars();
	Init_System();			

//	buff_i2c_tx[0] = '1';
//	buff_i2c_tx[1] = '2';
//	buff_i2c_tx[2] = '3';
//	buff_i2c_tx[3] = '4';
//	buff_i2c_tx[4] = '5';
//	buff_i2c_tx[5] = '\0';

/*==== EEPROM write test ====*/	
//	for (i = 0; i < 7; i++)
//	{
//		EE_Write_Byte(i, 0xFF);
//	}
//	
//	for (i = 0; i < 7; i++)
//	{
//		buff_i2c_tx[i] = EE_Read_Byte(i);
//	}
	
	EMI_ON();
					
	while (1)
	{	
		FeedWatchDog();		
		Command_Parse();				
													
/* wait for TX¡@i2c command and write to UART  */
//		if ((buff_i2c_rx[0] == 0x27) && (buff_i2c_rx[1] == 0x0A) && (ptr_i2c_rx_buff == 0x0C))
//		{
//			if (buff_i2c_rx[10] != 0xA5)
//			{
//				buff_i2c_rx[0] = 0x00;
//				buff_uart_tx[0] = 'E';
//				buff_uart_tx[1] = 'R';
//				buff_uart_tx[2] = 'R';
//				buff_uart_tx[3] = '\r';
//				buff_uart_tx[4] = '\n';	
//				Uart_SendBuf(buff_uart_tx, 5 );								
//			}
//			else
//			{
//				buff_i2c_rx[0] = 0x00;
//				buff_uart_tx[0] = Hex2char(buff_i2c_rx[10] >> 4);
//				buff_uart_tx[1] = Hex2char(buff_i2c_rx[10]);
//				buff_uart_tx[2] = Hex2char(buff_i2c_rx[11] >> 4);
//				buff_uart_tx[3] = Hex2char(buff_i2c_rx[11]);
//				buff_uart_tx[4] = '\r';
//				buff_uart_tx[5] = '\n';
//				Uart_SendBuf(buff_uart_tx, 6 );	
//			}
//		}		
	}
}

void Command_Parse(void)
{
	volatile uint8_t ee_addr, ee_data, i, n;
	//uint16_t dlycnt;
	static uint8_t u8temp = 0x00;

	if ((buff_i2c_rx[0] == 0x00) && (ptr_i2c_rx_buff >= 1)) // Read firmware version
	{
		my_strcpy1(8, buff_i2c_tx, date_code);
		Clear_Rx();
	}	
	else if ((buff_i2c_rx[0] == 0x01) && (ptr_i2c_rx_buff >= 4)) // 01: set height(string)
	{
		height_str[2] = buff_i2c_rx[1];
		height_str[1] = buff_i2c_rx[2];
		height_str[0] = buff_i2c_rx[3];
		Clear_Rx();
	}
	else if ((buff_i2c_rx[0] == 0x02) && (ptr_i2c_rx_buff >= 3)) // 02: set height(number)
	{
		height = (buff_i2c_rx[1] << 8) | buff_i2c_rx[2];
		utoa(height, height_str);
		buff_i2c_tx[0] = height_str[2];
		buff_i2c_tx[1] = height_str[1];
		buff_i2c_tx[2] = height_str[0];
		Clear_Rx();
	}	
	else if ((buff_i2c_rx[0] == 0x03) && (ptr_i2c_rx_buff >= 3)) // read EEPROM
	{
		
		b_ee_data_ready = 0;
		n = buff_i2c_rx[1];
		ee_addr = buff_i2c_rx[2];	
				
		for (i = 0; i < n; i++)
		{
			buff_i2c_tx[i] = EE_Read_Byte(i);			
		}
		b_ee_data_ready = 1;	
		
		Clear_Rx();			
	}
	else if ((buff_i2c_rx[0] == 0x04) && (ptr_i2c_rx_buff >= 3)) // write EEPROM byte
	{	
		ee_addr = buff_i2c_rx[1];
		ee_data = buff_i2c_rx[2];
		EE_Write_Byte(ee_addr, ee_data);

		Clear_Rx();			
	}		
	else if ((buff_i2c_rx[0] == 0x05) && (ptr_i2c_rx_buff >= (buff_i2c_rx[1] + 3))) // write EEPROM bytes
	{	
		n = buff_i2c_rx[1];
		ee_addr = buff_i2c_rx[2];
		EE_Write(n, ee_addr, buff_i2c_rx + 3);

		Clear_Rx();		
	}
	else if ((buff_i2c_rx[0] == 0x06) && (ptr_i2c_rx_buff >= 1)) // read key status
	{					
		buff_i2c_tx[0] = key_stat;
		Clear_Rx();
	}	
	else if ((buff_i2c_rx[0] == 0x20) && (ptr_i2c_rx_buff >= 2)) // SET LED Current Level
	{
		Led_Current_Set(buff_i2c_rx[1]);
		Clear_Rx();			
	}
	else if ((buff_i2c_rx[0] == 0x21) && (ptr_i2c_rx_buff >= 5)) // SET LED flash period and duty
	{
		led_duty= (buff_i2c_rx[1] << 8) | buff_i2c_rx[2];
		led_period= (buff_i2c_rx[3] << 8) | buff_i2c_rx[4];
		Clear_Rx();			
	}				
	else if ((buff_i2c_rx[0] == 0x10) && (ptr_i2c_rx_buff >= 3)) // read test buffer
	{
		n = buff_i2c_rx[1];
		ee_addr = buff_i2c_rx[2];	
//		if ((n + ee_addr) > 8)
//			n = 8 - ee_addr;
				
		for (i = 0; i < n; i++)
		{
			buff_i2c_tx[i] = buff_test[ee_addr + i];			
		}	
		
		Clear_Rx();			
	}
	else if ((buff_i2c_rx[0] == 0x11) && (ptr_i2c_rx_buff >= 3)) // write test buffer
	{
		ee_addr = buff_i2c_rx[1];
		ee_data = buff_i2c_rx[2];
		buff_test[ee_addr] = ee_data;

		Clear_Rx();			
	}	
//	else if ((buff_i2c_rx[0] == 0xF0) && (ptr_i2c_rx_buff >= 1)) // write test buffer
//	{
//		Power_On_Reset();
//		Clear_Rx();			
//	}		
//	else if ((buff_i2c_rx[0] == 0x06) && (ptr_i2c_rx_buff == 1)) // Read EEPROM status
//	{
//		//buff_i2c_tx[0] = b_ee_data_ready;		
//		buff_i2c_rx[0] = 0xff;
//		ptr_i2c_rx_buff = 0 ;	
//		GCC_NOP();		
//	}	
	else
	{
		FeedWatchDog();
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
			
	_pcc3 = 0;
	_pcc4 = 0;


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
//	_uartf = 0; 	//interrupt enable
//	_uarte = 1;
//	_rie = 1; 
//	
//	_brgh = 1; 		// high speed
//	_brg = 77; 		// 9600 bps
//	_uarten = 1; 	// pins switched to TX,RX
//	_txen = 0;	
//	_rxen = 0;

/* slave I2C */
	_iicen = 1; // enable I2C
	_i2cdbnc0 = 1; //debounce
	_i2cdbnc1 = 1;
	_pcc5 = 1; //input
	_pcc6 = 1;
	_pcps3 = 1; // max. I/O driving current 
	_pcps2 = 1;
	SDA = 1;  // set High level
	SCL = 1;
	_pcpu5 = 1; //I2C bus pull high
	_pcpu6 = 1;

	_iica = I2C_ADDR; //set slave address
//	_iichtx = 0;  // receive mode
//	_iictxak = 0; // send ack
//	_i2ctoc = 0b10111111;  // time out is enabled
	
	_iicf = 0;
	_iice = 1; // I2C interrupt
	
/* Time base 
Bit    7    6    5     4   3     2    1    0
Name TBON TBCK TB11 TB10 LXTLP TB02 TB01 TB00
TB0==
000: 256/fTB
001: 512/fTB
010: 1024/fTB
011: 2048/fTB
100: 4096/fTB
101: 8192/fTB
110: 16384/fTB
111: 32768/fTB 
TB1==
00: 4096/fTB
01: 8192/fTB
10: 16384/fTB
11: 32768/fTB
*/
	_tbc = 0b11110110; //fsys(12MHz)/4, tb1=32768/ftb , tb0=16384/ftb 
	_tb0f = 0;
	_tb0e = 1;
	_tb1f = 0;
	_tb1e = 1;	
}

void Init_Vars(void)
{
	uint8_t i;

	ptr_i2c_rx_buff = 0;
	ptr_i2c_tx_buff = 0;

	for (i = 0; i < BUFF_LEN ; i++)
	{
		buff_i2c_rx[i] = 0;
		buff_i2c_tx[i] = 0;
	}
}


void Led_Current_Set(uint8_t level)
{
	if (level == 1) //7mA
	{
		_pcps1 = 0;
		_pcps0 = 1;
		_paps3 = 0;
		_paps2 = 1;
		_pbps3 = 0;
		_pbps2 = 1;
		_pbps1 = 0;
		_pbps0 = 1;
		//_sledc0 =0b01010101;	
		//_sledc1 =0b01010101;	
	}
	else if (level == 2) // 10mA
	{
		_pcps1 = 1;
		_pcps0 = 0;
		_paps3 = 1;
		_paps2 = 0;
		_pbps3 = 1;
		_pbps2 = 0;
		_pbps1 = 1;
		_pbps0 = 0;		
		//_sledc0 =0b10101010;	
		//_sledc1 =0b10101010;	
	}	
	else if (level == 3) // 22mA
	{
		_pcps1 = 1;
		_pcps0 = 1;
		_paps3 = 1;
		_paps2 = 1;
		_pbps3 = 1;
		_pbps2 = 1;
		_pbps1 = 1;
		_pbps0 = 1;			
		//_sledc0 =0b11111111;	
		//_sledc1 =0b11111111;	
	}
	else	// 4mA
	{
		_pcps1 = 0;
		_pcps0 = 0;
		_paps3 = 0;
		_paps2 = 0;
		_pbps3 = 0;
		_pbps2 = 0;
		_pbps1 = 0;
		_pbps0 = 0;			
		//_sledc0	= 0x00;
		//_sledc1 = 0x00;		
	}		
}

/* led: B7  B6 B5 B4 B3 B2 B1 B0
        =========================
        DOT SG SF SE SD SC SB SA 
*/
void Digit_Set(uint8_t led)
{
	SA = (led >> 0) & 0x01;
	SB = (led >> 1) & 0x01;
	SC = (led >> 2) & 0x01;
	SD = (led >> 3) & 0x01;
	SE = (led >> 4) & 0x01;
	SF = (led >> 5) & 0x01;
	SG = (led >> 6) & 0x01;
	//DOT = (led >> 7) & 0x01;	
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

uint8_t Key_Scan(void)
{
	return ~(0x80 | (KS7 << 6) | (KS6 << 5) | (KS5 << 4) | (KS4 << 3) | (KS3 << 2) | (KS2 << 1) | (KS1 << 0));
}

void Clear_Rx(void)
{
	buff_i2c_rx[0] = 0xff;
	ptr_i2c_rx_buff = 0 ;
}			
/*---------------------------------------------------------------
	Interrupt Subroutines
-----------------------------------------------------------------
*/
DEFINE_ISR(Int_Tb0, 0x1C) //
{			
	
	FeedWatchDog();
	Led_Scan();	
	_tb0f = 0;
}

DEFINE_ISR(Int_Tb1, 0x20) //
{		
	static uint8_t cnt = 0;
	FeedWatchDog();	
/* Read Key */	
	key_stat = Key_Scan();	
	
	if (cnt++ > 10)
	{
		cnt = 0;
		
		if (BUZZER == 1)
			BUZZER = 0;
		else
			BUZZER = 1;
	}
			
	_tb1f = 0;
}

DEFINE_ISR(Int_UART, 0x2C)
{
	//volatile uint8_t temp, i;
	
	FeedWatchDog();
	
//	if (_rxif == 1)
//	{
//		temp = _txr_rxr;
//		if (temp == '@')
//		{
//			ptr_uart_rx_buff = 0;
//			uart_rx_received = 0;				
//			for (i = 0; i < BUFF_LEN ; i++)
//			{
//				buff_uart_rx[i] = 0;
//			}			
//		}
//		else if ((temp == 0) || (temp == '#'))
//		{
//			uart_rx_received = 1;				
//		}
//		else
//		{
//			if (ptr_uart_rx_buff < BUFF_LEN)
//			{
//				buff_uart_rx[ptr_uart_rx_buff] =	temp;
//				ptr_uart_rx_buff ++;
//			}
//		}
//	}
	
	_uartf = 0;
}

DEFINE_ISR(Int_I2C, 0x28)
{
	volatile uint8_t u8temp;
	EMI_OFF();
	
	FeedWatchDog();
	if (I2C_TIMEOUT == 1)
	{
		I2C_TIMEOUT_EN = 1;
		I2C_TIMEOUT = 0;
	}
	else 
	{
		if (I2C_ADDR_MATCH == 1)	// address match
		{
			ptr_i2c_tx_buff = 0;
			if (I2C_READ == 1) // master request data
			{
				I2C_TRANSMITTER = 1;
				//ptr_i2c_tx_buff = 0;
				I2C_DATA = buff_i2c_tx[ptr_i2c_tx_buff];
				ptr_i2c_tx_buff++;			
			}	
			else // master feed data
			{
				I2C_TRANSMITTER = 0;
				I2C_TX_NOACK = 0;				
				//ptr_i2c_rx_buff = 0;
				u8temp = I2C_DATA; //dummy read
			}
		}
		else  // data transfer
		{
			if (I2C_TRANSMITTER == 0) // master feed data
			{
				if (ptr_i2c_rx_buff < (BUFF_LEN - 1))
				{	
					buff_i2c_rx[ptr_i2c_rx_buff] = I2C_DATA;				
					GCC_NOP();						
					ptr_i2c_rx_buff ++;	 // ptr_i2c_rx_buff is the next saved buffer address	
				}

			}
			else // master request data
			{
				if (I2C_RX_NOACK == 0) // ack
				{
					I2C_DATA = buff_i2c_tx[ptr_i2c_tx_buff];
					ptr_i2c_tx_buff++;	
				}
				else // no ack, return to RX mode
				{
					I2C_TRANSMITTER = 0;
					I2C_TX_NOACK = 0;
					u8temp = I2C_DATA; //dummy read
				}
			}
		}
	}

	I2C_INT_FLAG = 0;	
	EMI_ON();
}

void Power_On_Reset(void)
{
	_lvrc = 0;	
}