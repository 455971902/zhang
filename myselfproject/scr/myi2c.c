//*****************************Copyright(c)*************************** 
//** company 					:         AMAE 
//** project 					:         016    
//** hardware version	:        
//**================================================================== 
//**-------------------------- file information-----------------------
//** file name:      myi2c.c
//** function :      i2c数据传输
//** version  :      v0.1
//** author	  :      zhangxu
//** data	  	:      2018.12.8
//**==================================================================
//====================================================================
//头文件
//====================================================================
#include "myi2c.h"
#include "delay.h"

//宏定义
//====================================================================

//变量定义
//====================================================================

//函数说明
//====================================================================

/**
  * 函数功能: IIC 配置引脚
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void I2C_SDA_IN(I2c_Bus *i2c)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Pin = i2c->gpio_Sda_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(i2c->gpio_Sda_Port, &GPIO_InitStruct);
}

/**
  * 函数功能: 配置I2C信号线的输出
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void I2C_SDA_OUT(I2c_Bus *i2c)
{
    GPIO_InitTypeDef GPIO_InitStruct;

		GPIO_InitStruct.GPIO_Pin = i2c->gpio_Sda_Pin;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(i2c->gpio_Sda_Port, &GPIO_InitStruct);

}


/**
  * 函数功能: IIC star
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
	
void I2C_Start(I2c_Bus *i2c)
{
    I2C_SDA_OUT(i2c);

    I2C_SCL_HIGH(i2c);
    I2C_SDA_HIGH(i2c);

    delay_us(i2c->delayTime);
    
    I2C_SDA_LOW(i2c);
		delay_us(4);
    I2C_SCL_LOW(i2c);											//准备进行发送或者接受数据
}

/**
  * 函数功能: CPU发起I2C总线停止信号
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void I2C_Stop(I2c_Bus *i2c)
{
    I2C_SDA_OUT(i2c);
    
    I2C_SCL_LOW(i2c);
    I2C_SDA_LOW(i2c);
    
    delay_us(i2c->delayTime);
    
    I2C_SCL_HIGH(i2c);
    I2C_SDA_HIGH(i2c);
    
}

/**
  * 函数功能: CPU产生一个时钟，并读取从器件的ACK应答信号
  * 输入参数: 无
  * 返 回 值: 返回0表示正确应答，1表示无从器件响应
  * 说    明：无
  */
uint8_t I2C_WaitAck(I2c_Bus *i2c)
{
    uint8_t tempTime = 0;

    I2C_SCL_LOW(i2c);
    delay_us(i2c->delayTime);
    
    I2C_SCL_HIGH(i2c);				/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
    delay_us(i2c->delayTime);
    
    I2C_SDA_IN(i2c);
    
     while(I2C_SDA_READ(i2c))	/* CPU读取SDA口线状态 */
    {
        tempTime++;
        if(tempTime > 250)
        {
            I2C_Stop(i2c);
            return 1;
        }
    	
    }
    I2C_SCL_LOW(i2c);
    
    return 0;
}

/**
  * 函数功能: CPU产生一个ACK信号
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void I2C_Ack(I2c_Bus *i2c)
{
    I2C_SDA_OUT(i2c);
    
    I2C_SCL_LOW(i2c);
    I2C_SDA_LOW(i2c);						/* CPU驱动SDA = 0 */
    
		delay_us(i2c->delayTime);

    I2C_SCL_HIGH(i2c);					/* CPU产生1个时钟 */
    delay_us(i2c->delayTime);

    I2C_SCL_LOW(i2c);
}

/**
  * 函数功能: CPU不产生应答（NACK）信号
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void I2C_NAck(I2c_Bus *i2c)
{
    I2C_SDA_OUT(i2c);
    
    I2C_SCL_LOW(i2c);
    I2C_SDA_HIGH(i2c);					/* CPU驱动SDA = 1 */
    delay_us(i2c->delayTime);
    
    I2C_SCL_HIGH(i2c);					/* CPU产生1个时钟 */
    delay_us(i2c->delayTime);
    
    I2C_SCL_LOW(i2c);
}

/**
  * 函数功能: CPU向I2C总线设备发送8bit数据
  * 输入参数: Byte ： 等待发送的字节
  * 返 回 值: 无
  * 说    明：无
  */
void I2C_SendByte(uint8_t Byte,I2c_Bus *i2c)
{
    uint8_t i;

    I2C_SDA_OUT(i2c);
    
	/* 先发送字节的高位bit7 */
    for (i = 0; i < 8; i++)
    {
	I2C_SCL_LOW(i2c);
    		
    	if (Byte & 0x80)
    	{
        	I2C_SDA_HIGH(i2c);
    	}
    	else
    	{
        	I2C_SDA_LOW(i2c);
    	}
    	
	I2C_SCL_HIGH(i2c);
    		
    	Byte <<= 1;	/* 左移一个bit */
    }
	
    I2C_SCL_LOW(i2c);
}

/**
  * 函数功能: CPU向I2C总线设备发送8bit数据
  * 输入参数: Byte ： 等待发送的字节
  * 返 回 值: 无
  * 说    明：无
  */
void Oled_I2C_SendByte(uint8_t Byte,I2c_Bus *i2c)
{
    uint8_t i;

    I2C_SDA_OUT(i2c);
    
	/* 先发送字节的高位bit7 */
    I2C_SCL_LOW(i2c);
    for (i = 0; i < 8; i++)
    {
    	if (Byte & 0x80)
    	{
        	I2C_SDA_HIGH(i2c);
    	}
    	else
    	{
        	I2C_SDA_LOW(i2c);
    	}
			Byte <<= 1;	/* 左移一个bit */
			I2C_SCL_HIGH(i2c);
      I2C_SCL_LOW(i2c);
    }
    I2C_SDA_HIGH(i2c);
    I2C_SCL_HIGH(i2c);
    I2C_SCL_LOW(i2c);
}

