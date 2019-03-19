
//*****************************Copyright(c)************************************* 
//** 公      司：         AMAE 
//** 项      目：         EM-1500    
//** 硬件版本号：       
//**================================================================== 
//**------------------------------------ 文件信息-------------------------------
//** 文  件 名:      ds18b20.c
//** 文件 描述:      环境温度检测
//** 版     本:      v0.1
//** 创  建 人:      苏琴
//** 创建 日期:      2014.1.20
//**============================================================================
#include "MSP430x54x.h"
#include "ds18b20.h"
#include "parameter.h"
#include "tick.h"

#include "port.h"

#define  READ_DS18B20       0XCC         //读取操作命令
#define  TRANS_DS18B20      0X44         //转换命令
#define  DATA_DS18B20       0XBE         //温度寄存器地址
#define  CONFIG_DS18B20     0x4e


unsigned char temperatureBuf[2] = {0};        //温度缓存区

unsigned char sn[4] = {0};                    //序列号缓存区

float env_temp = 25;

//==============================================================================
//函数名称：void start_ds18b20(void)
//输入参数：void
//输出参数：回复标志
//功能说明：复位DS18B20
//调用位置：命令变化时调用
//备   注：
//============================================================================
unsigned int start_ds18b20(void)
{
  unsigned int flag = 0xff;
  
  DS18B20_OUT_1;  
  DS18B20_L_1;
       
  delay(900);       			//等待480~960us  
  
  DS18B20_H_1; 
 
  delay(70);        			//等待15~60us
  DS18B20_IN_1;
  flag = DS18B20_DATA_1;    	//持续时间为60us~240us     

  delay(200);   
  return flag;
}
//==============================================================================
//函数名称：void writeDs18b20Byte(unsigned char data)
//输入参数：data:写入数据
//输出参数：void
//功能说明：发送数据到DS18B20
//调用位置：
//备   注：
//============================================================================
void ds18b20_write_byte(unsigned char data)
{
    int i = 0;
    unsigned char temp = 0;
    DS18B20_OUT_1;
	DS18B20_H_1;
	delay(2);

    for(i = 0; i < 8; i++)
    {  
      enterCritical();

      temp = data >> i;
      temp &= 0x01;
      DS18B20_L_1;
      delay(20);
      if(temp == 1)
      {
        DS18B20_H_1;
      }
      delay(30);
      DS18B20_H_1;
      exitCritical();
      delay(2);
     } 

    delay(5);
}
//==============================================================================
//函数名称：unsigned char readDs18b20Byte(void)
//输入参数：void
//输出参数：环境温度存放数值
//功能说明：接收DS18B20的数据
//调用位置：temperature_Adc()
//备   注：
//============================================================================
unsigned char ds18b20_read_byte(void)
{
  int i = 0;
  unsigned char temp = 0;

  DS18B20_H_1;
  
  for(i = 0; i < 8; i++)
  {   enterCritical();
      temp >>= 1;
      DS18B20_L_1;      
      delay(2);
      DS18B20_H_1;
      delay(10);
      DS18B20_IN_1; 
      delay(15);
      if(DS18B20_DATA_1)
      {
        temp |= 0x80;      
      }
      delay(70);
      DS18B20_OUT_1; 
      DS18B20_H_1; 
      exitCritical();
      delay(2);
   } 
   delay(5);
   return temp;
}

//==============================================================================
//函数名称：int temperature_Adc(void)
//输入参数：void
//输出参数：采样AD值
//功能说明：获取环境温度采样值
//调用位置：tempTask()
//备   注：
//============================================================================
void temperature_Adc(void)
{
  int i = 0;  

  start_ds18b20();
  ds18b20_write_byte(READ_DS18B20);    
  ds18b20_write_byte(TRANS_DS18B20);
					 
  delayTick(2000);
 
  start_ds18b20();
  ds18b20_write_byte(READ_DS18B20);             //跳过ROM
  ds18b20_write_byte(DATA_DS18B20);             //读DS18B20温度缓存器
  
  for(i = 0; i < 2; i++)
  {
    temperatureBuf[i] = ds18b20_read_byte();  //获取采集温度    
  }  
  start_ds18b20();                //复位，
}

//==============================================================================
//函数名称：void init_Ds18b20(void)
//输入参数：void
//输出参数：void
//功能说明：初始化DS18B20
//调用位置：
//备   注：
//============================================================================
void readrom(void)          //read the serial 读取温度传感器的序列号
{                      //本程序中没有用到此函数
  start_ds18b20();
  delay(20);
  
  ds18b20_write_byte(0x33);

  sn[0] = ds18b20_read_byte();
  sn[1] = ds18b20_read_byte();  
  sn[2] = ds18b20_read_byte();
  sn[3] = ds18b20_read_byte();
}
//==============================================================================
//函数名称：unsigned char check_temp_trans(void)
//输入参数：void
//输出参数：0：异常 1：正常
//功能说明：采集环境温度及液温
//调用位置：main
//备   注：
//============================================================================
unsigned char check_temp_trans(void)
{
	unsigned char i = 0;
	
	readrom();
	
	for(i = 0; i < 4; i++)
	{
		if((sn[i] != 0)&&(sn[i]!= 0xff))
		{
			return 1;
		}
	}
	return 0;
}
//==============================================================================
//函数名称：long TempTask(void)
//输入参数：void
//输出参数：void
//功能说明：采集环境温度及液温
//调用位置：main
//备   注：
//============================================================================
void TempTask(void)
{
    float temp = 0.0;
    unsigned int tempflag = 0;
    unsigned int temp_num = 0;
    
    while(1)
    {
        temperature_Adc();                      //环境温度采样
        
        tempflag = temperatureBuf[1];           //环境温度计算
        temp_num =  (temperatureBuf[1] << 8)|temperatureBuf[0];            
        tempflag &= 0xf8;
        if((tempflag == 0xf8)&&(temp_num!= 0xffff))  //环境负温计算
        {
          temp_num = ~temp_num +1; 
          temp = 0.0625*temp_num;
          env_temp = 0.0-temp;
        }
        else
        {
          temp = 0.0625*temp_num;                //环境正温计算
          if(temp > 125)
          {
            temp = 0;
          }
          env_temp = temp;
        }
		if(check_temp_trans() == 0)
		{
			env_temp = 20.0;
		}
		delayTick(500);	
	}
}
					 