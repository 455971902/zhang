
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
#ifndef _DS18B20_H
#define _DS18B20_H
//环境温度
#define DS18B20_OUT_1   P6DIR |= BIT5
#define DS18B20_IN_1    P6DIR &= ~BIT5

#define DS18B20_H_1     P6OUT |= BIT5
#define DS18B20_L_1     P6OUT &= ~BIT5

#define DS18B20_DATA_1     (P6IN & BIT5)
//==============================================================================
//函数名称：long TempTask(void)
//输入参数：void
//输出参数：void
//功能说明：采集环境温度及液温
//调用位置：main
//备   注：
//============================================================================
void TempTask(void);

unsigned char check_temp_trans(void);

#endif