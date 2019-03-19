//*****************************Copyright(c)************************************* 
//** 公      司：         AMAE 
//** 项      目：         ADS130    
//** 硬件版本号：       
//**================================================================== 
//**------------------------------------ 文件信息-------------------------------
//** 文  件 名:      iic.c
//** 文件 描述:      iic通讯接口
//** 版     本:      v0.1
//** 创  建 人:      苏琴
//** 创建 日期:      2012.8.29
//============================================================================
//头文件
//=============================================================================
#include "iic.h"
#include "msp430f5438.h"
#include "tick.h"
#include "chip.h"
//============================================================================
//宏定义
//=============================================================================

#define SDA_IN    P9DIR &= ~BIT1          //P3.1输入
#define SDA_OUT   P9DIR |= BIT1           //P3.1输出
#define SCL_OUT   P9DIR |= BIT2           //P3.2输出


#define SDA_H     P9OUT |= BIT1           //P3.1 = 1
#define SDA_L     P9OUT &=~ BIT1          //P3.1 = 0
#define SCL_H     P9OUT |= BIT2           //P3.2 = 1
#define SCL_L     P9OUT &= ~BIT2          //P3.2 = 0

#define EP_WP_OUT	P9DIR |= BIT0           //P3.1输出	  
#define EP_WP_L		P9OUT &= ~BIT0           //P3.1输出	

#define IIC_READ_CMD      0xa1                //读命令
#define IIC_WRITE_CMD     0xa0                //写命令

#define SDA_DATA  (P9IN & BIT1)             //P3.1的电平
//============================================================================
//函数声明
//=============================================================================

//==============================================================================
//函数名称：void init_IIC(void)
//输入参数：void
//输出参数：void
//功能说明：iic初始化
//调用位置：main
//备   注：
//============================================================================
void init_IIC(void)
{

	EP_WP_OUT;
	SDA_OUT;
	SCL_OUT;
	delay(2);
	
	SDA_H;
	SCL_H;  
	EP_WP_L;
}
//==============================================================================
//函数名称：void iic_Start(void)
//输入参数：void
//输出参数：void
//功能说明：iic启动条件
//调用位置：main
//备   注：
//============================================================================
void iic_Start(void)
{
  SDA_OUT; 
  SDA_H;
  SCL_H;
  delay(10);
  SDA_L;
  delay(10);
  SCL_L;
  
}



//==============================================================================
//函数名称：void iic_Stop(void)
//输入参数：void
//输出参数：void
//功能说明：iic停止条件
//调用位置：main
//备   注：
//============================================================================
void iic_Stop(void)
{
  SDA_OUT;
  SCL_L;
  SDA_L;
  delay(10);
  SCL_H;
  delay(10);
  SDA_H;  
  delay(10);
}
//==============================================================================
//函数名称：void iic_Stop(void)
//输入参数：void
//输出参数：void
//功能说明：iic停止条件
//调用位置：main
//备   注：
//============================================================================
void iic_Ask(void)
{
  SDA_OUT;delay(10);
  
  SCL_L;delay(10);
  SDA_L;delay(10);
  SCL_H;delay(10);
  SCL_L; delay(10);
}

//==============================================================================
//函数名称：void iic_Stop(void)
//输入参数：void
//输出参数：void
//功能说明：iic停止条件
//调用位置：main
//备   注：
//============================================================================
void iic_NoAsk(void)
{
  SDA_OUT;delay(10);
  SCL_L;delay(10);
  SDA_H;delay(10);
  SCL_H;delay(10);
  SCL_L; delay(10);
}

//==============================================================================
//函数名称：int iic_GetAsk(void)
//输入参数：void
//输出参数：
//功能说明：
//调用位置：
//备   注：
//============================================================================
unsigned char iic_GetAsk(void)
{
  unsigned char temp = 0;

  SCL_L;
  SDA_H;
  SCL_H;
  SDA_IN; 
  delay(10);  
  
  temp = SDA_DATA;
  
  SCL_L;  
  
  return(temp & BIT1);
}

//==============================================================================
//函数名称：void iic_SendByte(unsigned int data)
//输入参数：void
//输出参数：void
//功能说明：iic发送数据
//调用位置：main
//备   注：高位先发
//============================================================================
void iic_SendByte(unsigned char data)
{
    int i = 7;
	
    SDA_OUT; 	    
    
    for(i = 0; i < 8; i++)
    {   
        SCL_L;
        if(data & 0x80)
        {
          SDA_H;          
        }
        else
        {
          SDA_L;
        }
        delay(10);
        SCL_H;      
        data <<= 1;
    }    
    SCL_L;  
    delay(20);
}
//==============================================================================
//函数名称：void iic_SendByte(unsigned int data)
//输入参数：void
//输出参数：void
//功能说明：iic发送数据
//调用位置：main
//备   注：
//============================================================================
 unsigned char  iic_RcvByte(void)
{
  int i = 0;
  unsigned char temp = 0;  
  
  SDA_IN;
  delay(10);
  
  for(i = 0; i < 8; i++)
  {
      temp <<= 1;
      SCL_H;      
      delay(2);
      if(SDA_DATA)
      {
        temp |= 1;      
      }
      SCL_L;
     delay(20);
   }    
   return temp;   
}


//==============================================================================
//函数名称：int writeByte(int addr, char data)
//输入参数：addr：地址 data：写入数据
//输出参数：0：写失败  非0：写成功
//功能说明：写单个字节数据
//调用位置：main
//备   注：
//============================================================================
int writeByte(int addr,unsigned char data)
{
  unsigned char temp = 0;
  
  iic_Start();
  
  iic_SendByte(IIC_WRITE_CMD);               //发送写命令字
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr >> 8);
  iic_SendByte(temp);          //发送高地址  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr);
  iic_SendByte(temp);        //发送低地址    
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  iic_SendByte(data);             //发送数据  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  iic_Stop();
  delay(200);
  return 1;
}

//==============================================================================
//函数名称：int writeByte(int addr, char data)
//输入参数：addr：地址 data：写入数据
//输出参数：0：写失败  非0：写成功
//功能说明：写单个字数据
//调用位置：main
//备   注：
//============================================================================
int writeWord(int addr,unsigned int data)
{
  unsigned char temp = 0;
  
  iic_Start();
  
  iic_SendByte(IIC_WRITE_CMD);               //发送写命令字
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr >> 8);
  iic_SendByte(temp);          //发送高地址  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr);
  iic_SendByte(temp);        //发送低地址    
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  temp = data >> 8;               //数据高位存放
  iic_SendByte(temp);             //发送数据  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = data;  
  iic_SendByte(temp);             //发送数据  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  iic_Stop();
  delay(500);
  return 1;
}
//==============================================================================
//函数名称：int writeByte(int addr, char data)
//输入参数：addr：地址 data：写入数据
//输出参数：0：写失败  非0：写成功
//功能说明：写单个字数据
//调用位置：main
//备   注：
//============================================================================
int writeDWord(int addr,unsigned long data)
{
  unsigned char temp = 0;
  
  iic_Start();
  
  iic_SendByte(IIC_WRITE_CMD);               //发送写命令字
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr >> 8);
  iic_SendByte(temp);          //发送高地址  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr);
  iic_SendByte(temp);        //发送低地址    
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  temp = data >> 24;               //数据高位存放
  iic_SendByte(temp);             //发送数据  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = data >> 16;  
  iic_SendByte(temp);             //发送数据  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = data >> 8;               //数据高位存放
  iic_SendByte(temp);             //发送数据  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = data;  
  iic_SendByte(temp);             //发送数据  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  iic_Stop();
  delay(500);
  return 1;
}
//==============================================================================
//函数名称：int writeByte(int addr, char data)
//输入参数：addr：地址 data：写入数据
//输出参数：0：写失败  非0：写成功
//功能说明：写单个字节数据
//调用位置：main
//备   注：
//============================================================================
int writeArrayData(int addr,unsigned char *data,int length)
{
  unsigned char temp = 0;
  int i = 0;
  iic_Start();  
  iic_SendByte(IIC_WRITE_CMD);           //发送写命令字
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr >> 8);
  iic_SendByte(temp);      //发送高地址  
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr);
  iic_SendByte(temp);  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }

   for(i = 0; i < length; i++)
   {
      iic_SendByte(*(data++));             //发送数据     
      temp = iic_GetAsk();  
      if(temp & BIT1)
      {
        return 0;
      }
   }
  iic_Stop();
  return 1;
}

//==============================================================================
//函数名称：int readByte(int addr, char *data)
//输入参数：addr :读取的地址  *data 读取数据存放地址
//输出参数：0：失败 1：成功
//功能说明：读取单个字节数据
//调用位置：main
//备   注：先写从机地址 
//============================================================================
int readByte(int addr,unsigned  char *data)
{
  unsigned char temp = 0;

  iic_Start();  
  iic_SendByte(IIC_WRITE_CMD);           //发送写命令字
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr >> 8);
  iic_SendByte(temp);      //发送高地址  
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr);
  iic_SendByte(temp);  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  
  iic_Start();
  iic_SendByte(IIC_READ_CMD);           //发送接收命令字  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }

  *data = (unsigned char)iic_RcvByte();
  iic_Stop();

  return 1;
}

//==============================================================================
//函数名称：int readByte(int addr, char *data)
//输入参数：addr :读取的地址  *data 读取数据存放地址
//输出参数：0：失败 1：成功
//功能说明：读取单个字节数据
//调用位置：main
//备   注：先写从机地址 
//============================================================================
int readWord(int addr,unsigned  int *data)
{
  unsigned char temp = 0;
  unsigned int readword = 0;
  
  iic_Start();  
  iic_SendByte(IIC_WRITE_CMD);           //发送写命令字
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr >> 8);
  iic_SendByte(temp);      //发送高地址  
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr);
  iic_SendByte(temp);  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  
  iic_Start();
  iic_SendByte(IIC_READ_CMD);           //发送接收命令字  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }

  temp = iic_RcvByte();  
  readword = (unsigned int)temp << 8;
  iic_Ask();  
  
  temp = iic_RcvByte();
  readword |= temp;
  iic_NoAsk();
  *data = readword;
  iic_Stop();
  return 1;
}

//==============================================================================
//函数名称：int readByte(int addr, char *data)
//输入参数：addr :读取的地址  *data 读取数据存放地址
//输出参数：0：失败 1：成功
//功能说明：读取单个字节数据
//调用位置：main
//备   注：先写从机地址 
//============================================================================
int readDWord(int addr,unsigned  long *data)
{
  unsigned char temp = 0;
  unsigned long readword = 0;
  
  iic_Start();  
  iic_SendByte(IIC_WRITE_CMD);           //发送写命令字
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr >> 8);
  iic_SendByte(temp);      //发送高地址  
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr);
  iic_SendByte(temp);  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  iic_Start();
  iic_SendByte(IIC_READ_CMD);           //发送接收命令字  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }

  temp = iic_RcvByte();
  iic_Ask();
  readword = (unsigned long)temp << 24;
  
  temp = iic_RcvByte();
  iic_Ask();
  readword |= (unsigned long)temp << 16;
  
  temp = iic_RcvByte();
  iic_Ask();
  readword |= (unsigned long)temp << 8;
  
  temp = iic_RcvByte();  
  iic_NoAsk();
  readword |= temp;
  
  *data = readword;
  iic_Stop();
  return 1;
}

//==============================================================================
//函数名称：int readByte(int addr, char *data)
//输入参数：addr :读取的地址  *data 读取数据存放地址
//输出参数：0：失败 1：成功
//功能说明：读取单个字节数据
//调用位置：main
//备   注：先写从机地址 
//============================================================================
int readArrayData(int addr,unsigned  char *data,unsigned int length)
{
  unsigned char temp  = 0;          //中间变量        
  int i= 0;                         //循环变量
  
  iic_Start();  
  iic_SendByte(IIC_WRITE_CMD);
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr >> 8);
  iic_SendByte(temp);      //发送高地址  
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr);     //发送低地址
  iic_SendByte(temp);  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  } 
    
  iic_Start();
  iic_SendByte(IIC_READ_CMD);           //发送接收命令字  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }

  for(i = 0; i < length-1; i++)
  {
    *(data+i) = iic_RcvByte();
    iic_Ask();
  }
 *(data+length-1) = iic_RcvByte();
 
  iic_NoAsk();  
  iic_Stop();
  return 1;
}
//==============================================================================
//函数名称：int readByte(int addr, char *data)
//输入参数：addr :读取的地址  *data 读取数据存放地址
//输出参数：0：失败 1：成功
//功能说明：读取单个字节数据
//调用位置：main
//备   注：先写从机地址 
//============================================================================
void clr_FM24C64(void)
{
    unsigned int i = 0;
    
    for(i = 0; i < 0x1fff; i++)
    {
       writeByte(i, 0xff); 
    } 
}