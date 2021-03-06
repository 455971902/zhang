//*****************************Copyright(c)*************************** 
//** company：         AMAE 
//** project：         016    
//** hardware version：       
//**================================================================== 
//**-------------------------- file information-----------------------
//** file name:      bmp280.c
//** function :      接收数字压力传感器的温温度和压力
//** version  :      v0.1
//** author		:      zhangxu
//** data			:      2018.12.8
//**==================================================================
//====================================================================
//头文件
//====================================================================
#include "bmp280.h"
#include "delay.h"
#include "myi2c.h"
#include <string.h>
//====================================================================

/* 宏定义------------------------------------------------------------*/
/* 类型变量 ---------------------------------------------------------*/
//extern I2C_HandleTypeDef hi2c1;
//setMaintainData_t g_setMaintainData;
static bmp280_t 	p_bmp280;
static  long bmp280_Pressure = 0; 												//当前的气压
static long bmp280_Temperature = 0;												//当前气温

/* 私有变量 ---------------------------------------------------------*/
/* 扩展变量 ---------------------------------------------------------*/
/* 私有函数 ---------------------------------------------------------*/
//=====================================================================

//=====================================================================
//变量定义
//==========================================================================

//==========================================================================
//函数声明
//==========================================================================
//==========================================================================
//函数描述
//==========================================================================
#if 0
/**
  * 函数功能: 
  * 输入参数: 
  * 返 回 值:
  * 说    明�
  */： 
void Bmp280_IIC_Init(void)
{
    I2c_Bus *bmp280Temp = &g_Bmp280;
    I2C_SDA_OUT(bmp280Temp);
    I2C_SCL_HIGH(bmp280Temp); 
    I2C_SDA_HIGH(bmp280Temp);
}
/**
  * 函数功能: 
  * 输入参数: 
  * 返 回 值: 
  * 说    明：
  */
static unsigned int bmp280_write_byte( unsigned char addr,unsigned char reg,unsigned char data)
{
  unsigned char temp = 0;
  
  I2C_Start(&g_Bmp280);
  
  I2C_SendByte((addr<<1)|0,&g_Bmp280);                   //发送写控制命令字
  temp = I2C_WaitAck(&g_Bmp280); 
  if(temp == 1)
  {
    return 0;
  }
	I2C_SendByte(reg,&g_Bmp280);             							//发送读控制命令字 
  temp = I2C_WaitAck(&g_Bmp280);  
  if(temp == 1)
  {
    return 0;
  }
  
  I2C_SendByte(data,&g_Bmp280);                   			//发送写控制命令字
  temp = I2C_WaitAck(&g_Bmp280); 
  if(temp == 1)
  {
    return 0;
  }
  I2C_Stop(&g_Bmp280);
  return 1;
}
static unsigned int bmp280_read_byte( unsigned char addr,unsigned char reg)
{
  unsigned data = 0;
  unsigned char temp = 0;
  
  I2C_Start(&g_Bmp280);
  //temp = (addr<<1)|0;
  I2C_SendByte((addr<<1)|0,&g_Bmp280);                   //发送写控制命令字
  temp = I2C_WaitAck(&g_Bmp280); 
  if(temp == 1)
  {
    return 0;
  }

  
  I2C_SendByte(reg,&g_Bmp280);             //发送读控制命令字 
  temp = I2C_WaitAck(&g_Bmp280);  
  if(temp == 1)
  {
    return 0;
  }
  I2C_Start(&g_Bmp280);
  I2C_SendByte((addr<<1)|1,&g_Bmp280);             //发送读控制命令字 
  temp = I2C_WaitAck(&g_Bmp280);  
  if(temp == 1)
  {
    return 0;
  }
  
  data = I2C_ReadByte(&g_Bmp280);                      //获取低8位数据
  I2C_NAck(&g_Bmp280);
  I2C_Stop(&g_Bmp280);
  delay_us(6);

  return data;
	
}

/**
  * 函数功能: 
  * 输入参数: 
  * 返 回 值: 
  * 说    明：
  */static unsigned int bmp280_read_Len( unsigned char addr,unsigned char reg,unsigned char len,void *buf)
{
   unsigned char temp = 0;
  unsigned char *str;
  str = buf;
  
  I2C_Start(&g_Bmp280);
  
  I2C_SendByte((addr<<1)|0,&g_Bmp280);                   //发送写控制命令字
  temp = I2C_WaitAck(&g_Bmp280); 
  if(temp == 1)
  {
    return 0;
  }

  
  I2C_SendByte(reg,&g_Bmp280);             //发送读控制命令字 
  temp = I2C_WaitAck(&g_Bmp280);  
  if(temp == 1)
  {
    return 0;
  }

  I2C_Start(&g_Bmp280);
  I2C_SendByte((addr<<1)|1,&g_Bmp280);             //发送读控制命令字 
  temp = I2C_WaitAck(&g_Bmp280);  
  if(temp == 1)
  {
    return 0;
  }
  
    for(char i = 0;i < len;++i)
    {
        if(i ==(len - 1))
        {
            str[i] = I2C_ReadByte(&g_Bmp280);   //读数据,发送nACK 
            I2C_NAck(&g_Bmp280);

        }
        else
        {
            str[i] = I2C_ReadByte(&g_Bmp280);		//读数据,发送ACK 
            I2C_Ack(&g_Bmp280);
        }
    }                   //获取低8位数据
  I2C_Stop(&g_Bmp280);
  return 1;
}
#endif
