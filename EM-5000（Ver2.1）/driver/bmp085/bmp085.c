//*****************************Copyright(c)************************************* 
//** 公      司：         AMAE 
//** 项      目：         ADS130    
//** 硬件版本号：       
//**================================================================== 
//**------------------------------------ 文件信息-------------------------------
//** 文  件 名:      bmp085.c
//** 文件 描述:      接收数字压力传感器的温度
//** 版     本:      v0.1
//** 创  建 人:      苏琴
//** 创建 日期:      2012.9.17
//**============================================================================
//==========================================================================
//头文件
//==========================================================================
#include "msp430f5438.h"
#include "bmp085.h"
#include "parameter.h"
#include "tick.h"
//==========================================================================
//宏定义
//==========================================================================
#define BMP085_SDA_OUT      P3DIR |= BIT1         //SDA输出
#define BMP085_SDA_IN       P3DIR &= ~BIT1        //SDA输入
#define BMP085_SDA          (P3IN&BIT1)             //SDA数据

#define BMP085_EOC_IN       P3DIR &= ~BIT5        //EOC输入
#define BMP085_SCL_OUT      P3DIR |= BIT2         //SCL输出
#define BMP085_XCLK_OUT     P3DIR |= BIT4         //复位输出

#define BMP085_XCLK_H       P3OUT |= BIT4       //复位引脚
#define BMP085_XCLK_L       P3OUT &= ~BIT4

#define BMP085_SDA_H        P3OUT |= BIT1       //数据输出电平
#define BMP085_SDA_L        P3OUT &= ~BIT1

#define BMP085_SCL_H        P3OUT |= BIT2       //时钟输出电平
#define BMP085_SCL_L        P3OUT &= ~BIT2

//#define BMP085_EOC          (P3IN&BIT5)           //转换完成标志位

#define BMP085_WRITE_CMD    0xee                //bmp085写指令
#define BMP085_READ_CMD     0xEF                //bmp085读指令

//==========================================================================
//变量定义
//==========================================================================
static BMP085_PARAMETER bmp085_parameter;

float curPressure = 0.0;                        //当前大气压强值
//float curTemperature = 0.0;                     //085得到当前室温
//unsigned char Bmp085_State = 0;                 //芯片当前的状态


//==========================================================================
//函数声明
//==========================================================================
long read_bmp085_pressure();                    //未校准的压力值
long read_bmp085_temperture();                  //未校准的温度值
void bmp085_start();                            //重启BMP085芯片

//==========================================================================
//函数描述
//==========================================================================
//===========================================================================
//函数名称：void init_Bmp085(void)
//输入参数：void
//输出参数：void
//功能说明：初始化bmp085芯片
//调用位置：main
//备   注：
//============================================================================
void init_bmp085(void)
{
	BMP085_SCL_OUT;
	
	bmp085_parameter.ac1 = bmp085_read_word(0xAA);
	bmp085_parameter.ac2 = bmp085_read_word(0xAC);
	bmp085_parameter.ac3 = bmp085_read_word(0xAE);
	bmp085_parameter.ac4 = bmp085_read_word(0xB0);
	bmp085_parameter.ac5 = bmp085_read_word(0xB2);
	bmp085_parameter.ac6 = bmp085_read_word(0xB4);
	bmp085_parameter.b1 =  bmp085_read_word(0xB6);
	bmp085_parameter.b2 =  bmp085_read_word(0xB8);
	bmp085_parameter.mb =  bmp085_read_word(0xBA);
	bmp085_parameter.mc =  bmp085_read_word(0xBC);
	bmp085_parameter.md =  bmp085_read_word(0xBE);
	
	
}

//==============================================================================
//函数名称：void bmp085_start(void)
//输入参数：void
//输出参数：void
//功能说明：iic启动信号
//调用位置：
//备   注：
//============================================================================
void bmp085_start(void)
{
	BMP085_SDA_OUT;
	
    BMP085_SDA_H;                    //拉高数据线
    BMP085_SCL_H;                    //拉高时钟线
	
    delay(2);                 		 //延时
	
    BMP085_SDA_L;                    //产生下降沿
  //  delay(2);                 		 //延时
    BMP085_SCL_L;                    //拉低时钟线
}

//==============================================================================
//函数名称：void bmp085_stop(void)
//输入参数：void
//输出参数：void
//功能说明：iic停止信号
//调用位置：
//备   注：
//============================================================================
void bmp085_stop(void)
{
  BMP085_SDA_OUT;    //BMP085停止，SCL高时，SDA由低到高跳变
  BMP085_SCL_L;
  BMP085_SDA_L;
  delay(2);
  BMP085_SCL_H;
 // delay(2);
  BMP085_SDA_H;
  delay(2); 
}

//==============================================================================
//函数名称：void bmp085_ask(void)
//输入参数：void
//输出参数：void
//功能说明：主机响应确认信息
//调用位置：
//备   注：
//============================================================================
void bmp085_ask(void)
{
  BMP085_SDA_OUT;         //确认时，SDA低
  BMP085_SCL_L;
  BMP085_SDA_L;
  delay(2);
  BMP085_SCL_H;
  delay(2);
  BMP085_SCL_L; 
 // delay(3);
}

//==============================================================================
//函数名称：void bmp085_ask(void)
//输入参数：void
//输出参数：void
//功能说明：主机不响应确认信息
//调用位置：
//备   注：
//============================================================================
void bmp085_no_ask(void)
{
  BMP085_SDA_OUT;         //不确认时，SDA高
  BMP085_SCL_L;
  BMP085_SDA_H;
  delay(2);
  BMP085_SCL_H;
  delay(2);
  BMP085_SCL_L; 
  //delay(3);
}

//==============================================================================
//函数名称：unsigned char bmp085_get_ask(void)
//输入参数：void
//输出参数：应答信号 0：失败 1：成功
//功能说明：从机回复应答信号
//调用位置：
//备   注：
//============================================================================
unsigned char bmp085_get_ask(void)
{
  unsigned char temp = 0;                       
  BMP085_SCL_L; 
  delay(2);
  BMP085_SCL_H;
  delay(2);
  BMP085_SDA_IN;   
  temp = BMP085_SDA;
  delay(2);
  BMP085_SCL_L;  
  delay(2);
  return(temp & BIT1);
}

//==============================================================================
//函数名称：void bmp085_send_byte(unsigned char data)
//输入参数：data:发送数据
//输出参数：void
//功能说明：IIC总线发送数据
//调用位置：
//备   注：
//============================================================================
void bmp085_send_byte(unsigned char data)
{
  unsigned char i = 0;
  
  BMP085_SDA_OUT;

  for(i = 0; i < 8; i++)
  {
      
    BMP085_SCL_L;
    if(data &0x80)                                  //高位先发，高位有效
    {
      BMP085_SDA_H;
    }
    else
    {
      BMP085_SDA_L;
    }
    BMP085_SCL_H;
    data <<= 1;
  }
  BMP085_SCL_L;
}

//==============================================================================
//函数名称：unsigned char bmp085_recv_byte(void)
//输入参数：void
//输出参数：data:接收数据
//功能说明：IIC总线接收数据
//调用位置：
//备   注：
//============================================================================
unsigned char bmp085_recv_byte(void)
{
  unsigned char i = 0;
  unsigned char temp = 0;  
  
  BMP085_SDA_IN;
  
  for(i = 0; i < 8; i++)
  {
      BMP085_SCL_H;      
      if(BMP085_SDA)    
      {
        temp |= (0x80 >> i);      
      }
      BMP085_SCL_L;
   }    
   return temp;  
}


//==============================================================================
//函数名称：unsigned char bmp085_read_word( unsigned char addr,unsigned int *data)
//输入参数：addr：寄存器地址 data：读取数据
//输出参数：0：读失败  非0：读成功
//功能说明：读单个字节数据
//调用位置：
//备   注：
//============================================================================
unsigned int bmp085_read_word( unsigned char addr)
{
  unsigned char msb = 0;
  unsigned char lsb = 0;
  unsigned data = 0;
  unsigned char temp = 0;
  
  bmp085_start();
  
  bmp085_send_byte(BMP085_WRITE_CMD);                   //发送写控制命令字
  temp = bmp085_get_ask(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  bmp085_send_byte(addr);                        //发送寄存器地址    
  temp = bmp085_get_ask();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  bmp085_start();
  bmp085_send_byte(BMP085_READ_CMD);             //发送读控制命令字 
  temp = bmp085_get_ask();  
  if(temp & BIT1)
  {
    return 0;
  }

  msb = bmp085_recv_byte();                      //获取高8位
  bmp085_ask();
  
  lsb = bmp085_recv_byte();                      //获取低8位数据
  bmp085_no_ask();
  bmp085_stop();
  delay(6);
  data = (unsigned int)msb << 8;
  data |= lsb;
  return data;
	
}
//==============================================================================
//函数名称：long read_bmp085_temp(void)
//输入参数：void
//输出参数：当前温度采样值
//功能说明：读取未校准温度值
//调用位置：
//备   注：
//============================================================================
long read_bmp085_temp(void)
{
  unsigned int temp = 0;
  
  bmp085_start();
  bmp085_send_byte(BMP085_WRITE_CMD);            //发送写控制命令字
  temp = bmp085_get_ask(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  bmp085_send_byte(0xf4);                        //发送控制寄存器地址    
  temp = bmp085_get_ask();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  bmp085_send_byte(0x2e);                        //发送压强测试    
  temp = bmp085_get_ask();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  bmp085_stop();                    
  
 /* while(!BMP085_EOC)                               //等待转换完成
  {
  	;
  } */
  delayTick(10);
  
  bmp085_start();                               //接收压力测试数据      
  temp = bmp085_read_word(0xf6);
  return ((long)temp);
}

//==============================================================================
//函数名称：long read_bmp085_pressure(void)
//输入参数：void
//输出参数：当前压强采样值
//功能说明：读取未校准压强值
//调用位置：
//备   注：
//============================================================================
long read_bmp085_pressure(void)
{
  unsigned int press = 0;
  unsigned char temp = 0;
  
  bmp085_start();
  bmp085_send_byte(BMP085_WRITE_CMD);            //发送写控制命令字
  temp = bmp085_get_ask();   
  if(temp & BIT1)
  {
    return 0;
  }
  
  bmp085_send_byte(0xf4);                        //发送控制寄存器地址    
  temp = bmp085_get_ask();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  bmp085_send_byte(0x34);                        //发送压强测试    
  temp = bmp085_get_ask();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  bmp085_stop();                    
                                //等待转换完成
 /* while(!BMP085_EOC)
  {
  	;
  }*/
  delayTick(10);
  
  bmp085_start();                               //接收压力测试数据
  
  press = bmp085_read_word(0xf6);
 
  return ((long)press & 0x0000ffff);
}

//==============================================================================
//函数名称：unsigned char check_press_trans(void)
//输入参数：void
//输出参数：传感器的状态：0：异常 1：正常
//功能说明：读取未校准温度值
//调用位置：
//备   注：
//============================================================================
unsigned char check_press_trans(void)
{
	init_bmp085();
	
	if((bmp085_parameter.ac1 == 0xff)||(bmp085_parameter.ac2 == 0xff)
	   ||(bmp085_parameter.ac3 == 0xff)||(bmp085_parameter.ac4 == 0xff)
		   ||(bmp085_parameter.ac5 == 0xff)||(bmp085_parameter.ac6 == 0xff)
			   ||(bmp085_parameter.b1 == 0xff)||(bmp085_parameter.b2 == 0xff)
				   ||(bmp085_parameter.mb == 0xff)||(bmp085_parameter.mc == 0xff)
					   ||(bmp085_parameter.md == 0xff))
	{
		return 0;
	}
	return 1;
}

//==============================================================================
//函数名称：long read_bmp085_temp(void)
//输入参数：void
//输出参数：当前温度采样值
//功能说明：读取未校准温度值
//调用位置：
//备   注：
//============================================================================
void pressTask(void)
{
  long x1 = 0;
  long x2 = 0;
  long x3 = 0;
  long b5 = 0;
  long b3 = 0;
  long b6 = 0;
  unsigned long b4 = 0;
  unsigned long b7 = 0;  
  long temper = 0;                              //存放温度
  long pressure = 0;                            //存放压强
     
  init_bmp085();
  while(1)
  {
    temper = read_bmp085_temp();                //获取温度、压强采样值
    pressure = read_bmp085_pressure();    

    x1 = ((temper - bmp085_parameter.ac6)*bmp085_parameter.ac5) >> 15;    //计算温度值
    x2 = ((long)bmp085_parameter.mc << 11)/(x1 + bmp085_parameter.md);
    b5 = x1 + x2;
     
    b6 = b5 - 4000;                                             //计算压强值  
    x1 = (b6 * b6) >> 12;
    x1 *= bmp085_parameter.b2;
    x1 >>= 11;
    x2 = bmp085_parameter.ac2 * b6;
    x2 >>= 11;
    x3 = x1 + x2;
    
    b3 = (((long)bmp085_parameter.ac1 * 4 + x3) + 2) >> 2;
    x1 = (bmp085_parameter.ac3 * b6) >> 13;
    x2 = (bmp085_parameter.b1 * ((b6 * b6) >> 12)) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    b4 = (bmp085_parameter.ac4 *(unsigned long)(x3 + 32768)) >> 15;
    b7 = (unsigned long)(pressure - b3) * 50000;
    
    if(b7 < 0x80000000)
    {
      pressure = (b7 << 1) / b4;
    }
    else
    {
      pressure = (b7 / b4) << 1;    
    }
    x1 = pressure >> 8;
    x1 *= x1;
    x1 = (x1 * 3038) >> 16;
    x2 = (pressure * (-7357)) >> 16;
    pressure += (x1 + x2 +3791) >> 4;
	
    curPressure = (float)pressure / 1000;                   //获取当前压强 KPa
	delayTick(500);
  }

}