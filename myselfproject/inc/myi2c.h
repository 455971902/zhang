//*****************************Copyright(c)*************************** 
//** company：         AMAE 
//** project：         016    
//** hardware version：       
//**================================================================== 
//**-------------------------- file information-----------------------
//** file name:      myi2c.h
//** function :      
//** version  :      v0.1
//** author		:      zhangxu
//** data			:      2018.12.8
//**==================================================================
//====================================================================
//头文件
//====================================================================
#include "stm32f10x.h"
//====================================================================
#ifndef __MYI2C_H__
#define __MYI2C_H__
/* 宏定义------------------------------------------------------------*/
#if 1
#define I2C_SCL_HIGH(i2c)      GPIO_SetBits(i2c->gpio_Scl_Port,i2c->gpio_Scl_Pin)    // 输出高电平
#define I2C_SCL_LOW(i2c)       GPIO_ResetBits(i2c->gpio_Scl_Port,i2c->gpio_Scl_Pin)  // 输出低电平
#define I2C_SDA_HIGH(i2c)      GPIO_SetBits(i2c->gpio_Sda_Port,i2c->gpio_Sda_Pin)    // 输出高电平
#define I2C_SDA_LOW(i2c)       GPIO_ResetBits(i2c->gpio_Sda_Port,i2c->gpio_Sda_Pin)  // 输出低电平
#define I2C_SDA_READ(i2c)      GPIO_ReadInputDataBit(i2c->gpio_Sda_Port,i2c->gpio_Sda_Pin)//设置为输入
#endif

/* 类型变量 ---------------------------------------------------------*/

typedef struct I2C_Bus
{
    GPIO_TypeDef * gpio_Scl_Port;
    GPIO_TypeDef * gpio_Sda_Port;
    uint16_t gpio_Scl_Pin;
    uint16_t gpio_Sda_Pin;
    uint8_t delayTime;
}I2c_Bus;
/* 函数说明 ---------------------------------------------------------*/
void        I2C_Start(I2c_Bus *i2c);
void        I2C_Stop(I2c_Bus *i2c);
void        I2C_SendByte(uint8_t Byte,I2c_Bus *i2c);
void        Oled_I2C_SendByte(uint8_t Byte,I2c_Bus *i2c);
uint8_t  	  I2C_ReadByte(I2c_Bus *i2c);
uint8_t   	I2C_WaitAck(I2c_Bus *i2c);
void        I2C_Ack(I2c_Bus *i2c);
void        I2C_NAck(I2c_Bus *i2c);
void        I2C_SDA_OUT(I2c_Bus *i2c);
void        I2C_SDA_IN(I2c_Bus *i2c);


#endif


