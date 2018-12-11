#ifndef _I2C_MASTER_H_
#define _I2C_MASTER_H_


#define I2c_BITDLY() Delay(1)//DLY(1)

/* I2C master*/
#define SCL0 _pb0
#define SDA0 _pb1
#define I2c_SCL_0() {_pb0 = 0; _pbpu0 = 0; _pbc0 = 0;}
#define I2c_SCL_1() {_pb0 = 1; _pbpu0 = 1; _pbc0 = 1;}
#define I2c_SDA_0() {_pb1 = 0; _pbpu1 = 0; _pbc1 = 0;}
#define I2c_SDA_1() {_pb1 = 1; _pbpu1 = 1; _pbc1 = 1;}
//void Init_I2c(void);
void I2c_SendStart(void);
void I2c_SendStop(void);
uint8_t Wait_Ack(void);
void SendNotAck(void);
void SendAck(void);
uint8_t I2c_SendByte(uint8_t bt);
uint8_t I2c_Write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t len, uint8_t *buff);
uint8_t I2c_ReadByte(uint8_t ack);
uint8_t I2c_Read(uint8_t mode, uint8_t i2c_addr, uint8_t reg_addr, uint8_t len, uint8_t *buff); 

#endif //I2C_MASTER
