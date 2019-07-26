
//*****************************Copyright(c)************************************* 
//** company:         AMAE 
//** project:         016    
//** hardware version:       
//**================================================================== 
//**------------------------------------ file information-------------------------------
//** file name:      ds18b20.c
//** function	:      接受两路ds18b20传感器的温度
//** version	:      v0.1
//** author 	:      zhangxu
//** data			:      2018.12.5
//**============================================================================

/* 包含头文件 ----------------------------------------------------------------*/
#include "ds18b20.h"
#include "delay.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/

/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
static void DS18B20_Mode_IPU(void);
static void DS18B20_Mode_Out_PP(void);
static void DS18B20_Rst(void);
static uint8_t DS18B20_Presence(void);
static uint8_t DS18B20_ReadBit(void);
static uint8_t DS18B20_ReadByte(void);
static void DS18B20_SkipRom(void);
static void DS18B20B_Mode_IPU(void);
static void DS18B20B_Mode_Out_PP(void);
static void DS18B20B_Rst(void);
static uint8_t DS18B20B_Presence(void);
static uint8_t DS18B20B_ReadBit(void);
static uint8_t DS18B20B_ReadByte(void);
static void DS18B20B_SkipRom(void);


/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: 做延时使用
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void DS18B20_Delay(uint16_t time)
{
        uint8_t i;

  while(time)
  {    
          for (i = 0; i < 7; i++)
    {
      
    }
    time--;
  }
}

/**
  * 函数功能: DS18B20 初始化函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
uint8_t DS18B20_Init(void)
{
  DS18B20_Dout_GPIO_CLK_ENABLE();
  
  DS18B20_Mode_Out_PP();
        
  DS18B20_Dout_HIGH();
  DS18B20_Rst();
	
  
  return DS18B20_Presence ();
}
//---------------------------------------------------------------------------------
uint8_t DS18B20B_Init(void)
{
  DS18B20_Dout_GPIO_CLK_ENABLE();
  
  DS18B20_Mode_Out_PP();
        
  DS18B20B_Dout_HIGH();
  DS18B20_Rst();
	
  
  return DS18B20_Presence ();
}
//-------------------------------------------------------------------------------------

/**
  * 函数功能: 使DS18B20-DATA引脚变为上拉输入模式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void DS18B20_Mode_IPU(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* 串口外设功能GPIO配置 */
  GPIO_InitStruct.GPIO_Pin   = DS18B20_Dout_PIN;
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(DS18B20_Dout_PORT, &GPIO_InitStruct);      
}

//--------------------------------------------------------------------------------------------------
static void DS18B20B_Mode_IPU(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* 串口外设功能GPIO配置 */
  GPIO_InitStruct.GPIO_Pin   = DS18B20B_Dout_PIN;
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(DS18B20_Dout_PORT, &GPIO_InitStruct);
        
}
//---------------------------------------------------------------------------------------------------
/**
  * 函数功能: 使DS18B20-DATA引脚变为推挽输出模式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void DS18B20_Mode_Out_PP(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* 串口外设功能GPIO配置 */
  GPIO_InitStruct.GPIO_Pin = DS18B20_Dout_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(DS18B20_Dout_PORT, &GPIO_InitStruct);          
}
//-----------------------------------------------------------------------------------------------------------
static void DS18B20B_Mode_Out_PP(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* 串口外设功能GPIO配置 */
	GPIO_InitStruct.GPIO_Pin = DS18B20B_Dout_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(DS18B20_Dout_PORT, &GPIO_InitStruct);         
}
//-----------------------------------------------------------------------------------------------------------
/**
  * 函数功能: 主机给从机发送复位脉冲
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void DS18B20_Rst(void)
{
        /* 主机设置为推挽输出 */
        DS18B20_Mode_Out_PP();
        
        DS18B20_Dout_LOW();
  
        /* 主机至少产生480us的低电平复位信号 */
        DS18B20_Delay(600);
        
        /* 主机在产生复位信号后，需将总线拉高 */
        DS18B20_Dout_HIGH();
				DS18B20_Delay(20);
	
        
        /*从机接收到主机的复位信号后，会在15~60us后给主机发一个存在脉冲*/
      
}
//----------------------------------------------------------------------------------------------------
static void DS18B20B_Rst(void)
{
        /* 主机设置为推挽输出 */
        DS18B20B_Mode_Out_PP();
        
        DS18B20B_Dout_LOW();
  
        /* 主机至少产生480us的低电平复位信号 */
        DS18B20_Delay(600);
        
        /* 主机在产生复位信号后，需将总线拉高 */
        DS18B20B_Dout_HIGH();
				DS18B20_Delay(20);
	
        
        /*从机接收到主机的复位信号后，会在15~60us后给主机发一个存在脉冲*/
      
}
//----------------------------------------------------------------------------------------------------------
/**
  * 函数功能: 检测从机给主机返回的存在脉冲
  * 输入参数: 无
  * 返 回 值: 0：成功，1：失败
  * 说    明：无
  */
static uint8_t DS18B20_Presence(void)
{
        uint16_t pulse_time = 0;
        
        /* 主机设置为上拉输入 */
        DS18B20_Mode_IPU();
        
        /* 等待存在脉冲的到来，存在脉冲为一个60~240us的低电平信号 
         * 如果存在脉冲没有来则做超时处理，从机接收到主机的复位信号后，会在15~60us后给主机发一个存在脉冲
         */
        while( DS18B20_Data_IN() && pulse_time<100 )
        {
                pulse_time++;
                DS18B20_Delay(2);
        }        
        /* 经过100us后，存在脉冲都还没有到来*/
        if( pulse_time >=100 )
                return 1;
        else
                pulse_time = 0;
        
        /* 存在脉冲到来，且存在的时间不能超过240us */
        while( !DS18B20_Data_IN() && pulse_time<240 )
        {
                pulse_time++;
                DS18B20_Delay(2);
        }        
        if( pulse_time >=240 )
                return 1;
        else
                return 0;
}

//-------------------------------------------------------------------------------------
static uint8_t DS18B20B_Presence(void)
{
        uint16_t pulse_time = 0;
        
        /* 主机设置为上拉输入 */
        DS18B20B_Mode_IPU();
        
        /* 等待存在脉冲的到来，存在脉冲为一个60~240us的低电平信号 
         * 如果存在脉冲没有来则做超时处理，从机接收到主机的复位信号后，会在15~60us后给主机发一个存在脉冲
         */
        while( DS18B20B_Data_IN() && pulse_time<100 )
        {
                pulse_time++;
                DS18B20_Delay(2);
        }        
        /* 经过100us后，存在脉冲都还没有到来*/
        if( pulse_time >=100 )
                return 1;
        else
                pulse_time = 0;
        
        /* 存在脉冲到来，且存在的时间不能超过240us */
        while( !DS18B20B_Data_IN() && pulse_time<240 )
        {
                pulse_time++;
                DS18B20_Delay(2);
        }        
        if( pulse_time >=240 )
                return 1;
        else
                return 0;
}
//----------------------------------------------------------------------------------------



/**
  * 函数功能: 从DS18B20读取一个bit
  * 输入参数: 无
  * 返 回 值: 读取到的数据
  * 说    明：无
  */
static uint8_t DS18B20_ReadBit(void)
{
        uint8_t dat;
        
        /* 读0和读1的时间至少要大于60us */        
        DS18B20_Mode_Out_PP();
        /* 读时间的起始：必须由主机产生 >1us <15us 的低电平信号 */
        DS18B20_Dout_LOW();
        DS18B20_Delay(2);
        //DS18B20_Dout_HIGH();
        /* 设置成输入，释放总线，由外部上拉电阻将总线拉高 */
        DS18B20_Mode_IPU();
				DS18B20_Delay(12);
        
        if( DS18B20_Data_IN() == Bit_SET )
                dat = 1;
        else
                dat = 0;
        
        /* 这个延时参数请参考时序图 */
        DS18B20_Delay(50);
        
        return dat;
}

//-------------------------------------------------------------------
static uint8_t DS18B20B_ReadBit(void)
{
        uint8_t dat;
        
        /* 读0和读1的时间至少要大于60us */        
        DS18B20B_Mode_Out_PP();
        /* 读时间的起始：必须由主机产生 >1us <15us 的低电平信号 */
        DS18B20B_Dout_LOW();
        DS18B20_Delay(2);
        //DS18B20_Dout_HIGH();
        /* 设置成输入，释放总线，由外部上拉电阻将总线拉高 */
        DS18B20B_Mode_IPU();
				DS18B20_Delay(12);
        
        if( DS18B20B_Data_IN() == Bit_SET )
                dat = 1;
        else
                dat = 0;
        
        /* 这个延时参数请参考时序图 */
        DS18B20_Delay(50);
        
        return dat;
}
//--------------------------------------------------------------------

/**
  * 函数功能: 从DS18B20读一个字节，低位先行
  * 输入参数: 无
  * 返 回 值: 读到的数据
  * 说    明：无
  */
static uint8_t DS18B20_ReadByte(void)
{
        uint8_t i, j, dat = 0;        
        
        for(i=0; i<8; i++) 
        {
                j = DS18B20_ReadBit();                
                dat=(j<<7)|(dat>>1);
        }
        
        return dat;
}
//----------------------------------------------------------
static uint8_t DS18B20B_ReadByte(void)
{
        uint8_t i, j, dat = 0;        
        
        for(i=0; i<8; i++) 
        {
                j = DS18B20B_ReadBit();                
                dat=(j<<7)|(dat>>1);
        }
        
        return dat;
}
//------------------------------------------------------------

/**
  * 函数功能: 写一个字节到DS18B20，低位先行
  * 输入参数: dat：待写入数据
  * 返 回 值: 无
  * 说    明：无
  */
void DS18B20_WriteByte(uint8_t dat)
{
        uint8_t i, testb;
        DS18B20_Mode_Out_PP();
        
        for( i=0; i<8; i++ )
        {
                testb = dat&0x01;
                dat = dat>>1;                
                /* 写0和写1的时间至少要大于60us */
                if (testb)
                {                        
                        DS18B20_Dout_LOW();
                        /* 1us < 这个延时 < 15us */
                        DS18B20_Delay(2);
                        
                        DS18B20_Dout_HIGH();
                        DS18B20_Delay(60);
                }                
                else
                {                        
                        DS18B20_Dout_LOW();
                        /* 60us < Tx 0 < 120us */
                        DS18B20_Delay(60);
                        
                        DS18B20_Dout_HIGH();                
                        /* 1us < Trec(恢复时间) < 无穷大*/
                        DS18B20_Delay(2);
                }
        }
}

//----------------------------------------------------------------------------------------------
void DS18B20B_WriteByte(uint8_t dat)
{
        uint8_t i, testb;
        DS18B20B_Mode_Out_PP();
        
        for( i=0; i<8; i++ )
        {
                testb = dat&0x01;
                dat = dat>>1;                
                /* 写0和写1的时间至少要大于60us */
                if (testb)
                {                        
                        DS18B20B_Dout_LOW();
                        /* 1us < 这个延时 < 15us */
                        DS18B20_Delay(2);
                        
                        DS18B20B_Dout_HIGH();
                        DS18B20_Delay(60);
                }                
                else
                {                        
                        DS18B20B_Dout_LOW();
                        /* 60us < Tx 0 < 120us */
                        DS18B20_Delay(60);
                        
                        DS18B20B_Dout_HIGH();                
                        /* 1us < Trec(恢复时间) < 无穷大*/
                        DS18B20_Delay(2);
                }
        }
}
//----------------------------------------------------------------------------------------

static void DS18B20_SkipRom ( void )
{
        DS18B20_Rst();                   
        DS18B20_Presence();                 
        DS18B20_WriteByte(0XCC);
				DS18B20_WriteByte(0X44);			/* 跳过 ROM */        
}

//--------------------------------------------------------------------------------------
static void DS18B20B_SkipRom ( void )
{
        DS18B20B_Rst();                   
        DS18B20B_Presence();                 
        DS18B20B_WriteByte(0XCC);
				DS18B20B_WriteByte(0X44);			/* 跳过 ROM */        
}
//---------------------------------------------------------------------------------------
float DS18B20_GetTemp_SkipRom ( void )
{
        uint8_t tpmsb, tplsb;
        short s_tem;
        float f_tem;
        
        
        DS18B20_SkipRom ();
	      DS18B20_Rst();                  
        DS18B20_Presence ();                               /* 开始转换 */
        DS18B20_Delay(2);
				DS18B20_WriteByte(0xcc);
				DS18B20_WriteByte(0xbe);                            /* 读温度值 */
				DS18B20_Delay(5);
        tplsb = DS18B20_ReadByte();                 
        tpmsb = DS18B20_ReadByte(); 
        
        
        s_tem = tpmsb<<8;
        s_tem = s_tem | tplsb;
        
        if( s_tem < 0 )                /* 负温度 */
                f_tem = (~s_tem+1) * 0.0625;        
        else
                f_tem = (float)s_tem * 0.0625;
        
        return f_tem;         
}

//另外一路
float DS18B20B_GetTemp_SkipRom( void )
{
        uint8_t tpmsb, tplsb;
        short s_tem;
        float f_tem;
        DS18B20B_SkipRom ();
	      DS18B20B_Rst();                  
        DS18B20B_Presence ();                               /* 开始转换 */
        DS18B20_Delay(2);
				DS18B20B_WriteByte(0xcc);
				DS18B20B_WriteByte(0xbe);                            /* 读温度值 */
				DS18B20_Delay(5);
        tplsb = DS18B20B_ReadByte();                 
        tpmsb = DS18B20B_ReadByte(); 
        
        
        s_tem = tpmsb<<8;
        s_tem = s_tem | tplsb;
        
        if( s_tem < 0 )                /* 负温度 */
                f_tem = (~s_tem+1) * 0.0625;        
        else
                f_tem = (float)s_tem * 0.0625;
        
        return f_tem;         
}

