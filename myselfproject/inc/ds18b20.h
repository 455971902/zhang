//*****************************Copyright(c)*************************** 
//** company :         AMAE 
//** project :         016    
//** hardware version:        
//**================================================================== 
//**-------------------------- file information-----------------------
//** file name:      ds18b20.h
//** function :      
//** version  :      v0.1
//** author	  :      zhangxu
//** data	  :      2018.12.5
//**==================================================================

#ifndef _DS18B20_H__
#define _DS18B20_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f10x.h"

/* 宏定义 -------------------------------------------------------------------*/
/***********************   DS18B20 连接引脚定义  **************************/
#define DS18B20_Dout_GPIO_CLK_ENABLE()              	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE) 
#define DS18B20_Dout_PORT                           	GPIOB
#define DS18B20_Dout_PIN                            	GPIO_Pin_4
#define DS18B20B_Dout_PIN                            	GPIO_Pin_5

/***********************   DS18B20 函数宏定义  ****************************/
#define DS18B20_Dout_LOW()                          	GPIO_ResetBits(DS18B20_Dout_PORT,DS18B20_Dout_PIN)
#define DS18B20_Dout_HIGH()                        	 	GPIO_SetBits(DS18B20_Dout_PORT,DS18B20_Dout_PIN)
#define DS18B20_Data_IN()                          		GPIO_ReadInputDataBit(DS18B20_Dout_PORT,DS18B20_Dout_PIN)



#define DS18B20B_Dout_LOW()                          	GPIO_ResetBits(DS18B20_Dout_PORT,DS18B20B_Dout_PIN)
#define DS18B20B_Dout_HIGH()                         	GPIO_SetBits(DS18B20_Dout_PORT,DS18B20B_Dout_PIN)
#define DS18B20B_Data_IN()                           	GPIO_ReadInputDataBit(DS18B20_Dout_PORT,DS18B20B_Dout_PIN)
/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
uint8_t DS18B20_Init(void);
uint8_t DS18B20B_Init(void);

float DS18B20_GetTemp_SkipRom(void);
void 	DS18B20_WriteByte(uint8_t dat);
float DS18B20B_GetTemp_SkipRom(void);
void 	DS18B20B_WriteByte(uint8_t dat);

#endif 

