//*****************************Copyright(c)*************************** 
//** company 					:         AMAE 
//** project 					:         016    
//** hardware version	:        
//**================================================================== 
//**-------------------------- file information-----------------------
//** file name:      myi2c.c
//** function :      i2c���ݴ���
//** version  :      v0.1
//** author	  :      zhangxu
//** data	  	:      2018.12.8
//**==================================================================
//====================================================================
//ͷ�ļ�
//====================================================================
#include "myi2c.h"
#include "delay.h"

//�궨��
//====================================================================

//��������
//====================================================================

//����˵��
//====================================================================

/**
  * ��������: IIC ��������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void I2C_SDA_IN(I2c_Bus *i2c)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Pin = i2c->gpio_Sda_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(i2c->gpio_Sda_Port, &GPIO_InitStruct);
}

/**
  * ��������: ����I2C�ź��ߵ����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
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
  * ��������: IIC star
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
	
void I2C_Start(I2c_Bus *i2c)
{
    I2C_SDA_OUT(i2c);

    I2C_SCL_HIGH(i2c);
    I2C_SDA_HIGH(i2c);

    delay_us(i2c->delayTime);
    
    I2C_SDA_LOW(i2c);
		delay_us(4);
    I2C_SCL_LOW(i2c);											//׼�����з��ͻ��߽�������
}

/**
  * ��������: CPU����I2C����ֹͣ�ź�
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
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
  * ��������: CPU����һ��ʱ�ӣ�����ȡ��������ACKӦ���ź�
  * �������: ��
  * �� �� ֵ: ����0��ʾ��ȷӦ��1��ʾ�޴�������Ӧ
  * ˵    ������
  */
uint8_t I2C_WaitAck(I2c_Bus *i2c)
{
    uint8_t tempTime = 0;

    I2C_SCL_LOW(i2c);
    delay_us(i2c->delayTime);
    
    I2C_SCL_HIGH(i2c);				/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
    delay_us(i2c->delayTime);
    
    I2C_SDA_IN(i2c);
    
     while(I2C_SDA_READ(i2c))	/* CPU��ȡSDA����״̬ */
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
  * ��������: CPU����һ��ACK�ź�
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void I2C_Ack(I2c_Bus *i2c)
{
    I2C_SDA_OUT(i2c);
    
    I2C_SCL_LOW(i2c);
    I2C_SDA_LOW(i2c);						/* CPU����SDA = 0 */
    
		delay_us(i2c->delayTime);

    I2C_SCL_HIGH(i2c);					/* CPU����1��ʱ�� */
    delay_us(i2c->delayTime);

    I2C_SCL_LOW(i2c);
}

/**
  * ��������: CPU������Ӧ��NACK���ź�
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void I2C_NAck(I2c_Bus *i2c)
{
    I2C_SDA_OUT(i2c);
    
    I2C_SCL_LOW(i2c);
    I2C_SDA_HIGH(i2c);					/* CPU����SDA = 1 */
    delay_us(i2c->delayTime);
    
    I2C_SCL_HIGH(i2c);					/* CPU����1��ʱ�� */
    delay_us(i2c->delayTime);
    
    I2C_SCL_LOW(i2c);
}

/**
  * ��������: CPU��I2C�����豸����8bit����
  * �������: Byte �� �ȴ����͵��ֽ�
  * �� �� ֵ: ��
  * ˵    ������
  */
void I2C_SendByte(uint8_t Byte,I2c_Bus *i2c)
{
    uint8_t i;

    I2C_SDA_OUT(i2c);
    
	/* �ȷ����ֽڵĸ�λbit7 */
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
    		
    	Byte <<= 1;	/* ����һ��bit */
    }
	
    I2C_SCL_LOW(i2c);
}

/**
  * ��������: CPU��I2C�����豸����8bit����
  * �������: Byte �� �ȴ����͵��ֽ�
  * �� �� ֵ: ��
  * ˵    ������
  */
void Oled_I2C_SendByte(uint8_t Byte,I2c_Bus *i2c)
{
    uint8_t i;

    I2C_SDA_OUT(i2c);
    
	/* �ȷ����ֽڵĸ�λbit7 */
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
			Byte <<= 1;	/* ����һ��bit */
			I2C_SCL_HIGH(i2c);
      I2C_SCL_LOW(i2c);
    }
    I2C_SDA_HIGH(i2c);
    I2C_SCL_HIGH(i2c);
    I2C_SCL_LOW(i2c);
}

