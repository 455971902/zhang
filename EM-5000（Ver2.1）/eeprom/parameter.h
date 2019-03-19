/************************************************************
** 版权:   AMAE
** 文件名: parameter.h
** 版本：  
** 工作环境: IAR 5.10
** 作者:   苏琴
** 生成日期: 2014.02.10
** 文件描述: 参数配置文件
** 相关文件: 
** 修改日志：         
*************************************************************/
#ifndef _PARAMETER_H_
#define _PARAMETER_H_

#include "ds1302.h"
#include "chip.h"

//-----------------浮点数存放格式-----------------------
//
typedef union
{
    float f;
    unsigned char data_F[4];
}float_x;  

//-----------------双精度浮点数存放格式-----------------------
//
typedef union
{
    double d;
    unsigned char data_D[8];
}double_x; 

//-----------------参数设置存储地址-----------------------
#define SET_START_ADDR        		1                  //设置参数存储的起始地址
#define SET_MOD_ADDR				SET_START_ADDR

#define CRC_NUM						0xfe

//手动参数地址
#define	SET_MANU_PARA_ADDR			SET_START_ADDR+1

#define	SET_MANU_FLOW_ADDR			SET_MANU_PARA_ADDR
#define	SET_MANU_SMP_TIME_ADDR		SET_MANU_PARA_ADDR+2
#define	SET_MANU_TEMP_FLAG_ADDR		SET_MANU_PARA_ADDR+4
#define	SET_MANU_TEMP_ADDR			SET_MANU_PARA_ADDR+5

#define SET_MANU_PARA_FLAG_ADDR		SET_MANU_PARA_ADDR+7

//定时参数地址							
#define	SET_GROUP_PARA_ADDR			SET_START_ADDR+10

#define SET_GROUP_START_TIME_ADDR   SET_GROUP_PARA_ADDR  
#define SET_GROUP_FLOW_ADDR			SET_GROUP_PARA_ADDR+2  
#define SET_GROUP_SMP_TIME_ADDR   	SET_GROUP_PARA_ADDR+4
#define SET_GROUP_SRB_ADDR			SET_GROUP_PARA_ADDR+6  

#define SET_GROUP_PARA_FLAG_ADDR	SET_GROUP_PARA_ADDR+7

#define	SET_PARA_NUM				8

//循环参数地址							
#define	SET_CLV_PARA_ADDR			SET_START_ADDR+100

#define SET_CLV_FLOW_ADDR   		SET_CLV_PARA_ADDR  
#define SET_CLV_MID_TIME_ADDR		SET_CLV_PARA_ADDR+2  
#define SET_CLV_SMP_TIME_ADDR   	SET_CLV_PARA_ADDR+4
#define SET_CLV_SMP_NUM_ADDR		SET_CLV_PARA_ADDR+6

#define SET_CLV_PARA_FLAG_ADDR		SET_CLV_PARA_ADDR+7

//定容参数地址							
#define	SET_CAP_PARA_ADDR			SET_START_ADDR+110

#define SET_CAP_VOL_ADDR   			SET_CAP_PARA_ADDR  
#define SET_CAP_FLOW_ADDR			SET_CAP_PARA_ADDR+8 
#define SET_CAP_MID_TIME_ADDR		SET_CAP_PARA_ADDR+10  
#define SET_CAP_SMP_NUM_ADDR		SET_CAP_PARA_ADDR+12

#define SET_CAP_PARA_FLAG_ADDR		SET_CAP_PARA_ADDR+13





//-----------------泵数据查询存储地址-----------------------
#define STORE_START_ADDR      150
#define STORE_PARA_NUM		  38


#define STORE_FLAG_ADDR         STORE_START_ADDR           		//写入标志
#define START_TIME_ADDR       	STORE_START_ADDR+1            	//采样时刻
#define FLOW_ADDR             	STORE_START_ADDR+7            	//采样流量
#define TEMP_ADDR             	STORE_START_ADDR+9           	//温度
#define PREV_ADDR             	STORE_START_ADDR+13           	//压强

#define SUM_TIME_ADDR         	STORE_START_ADDR+17           	//累计时间
#define SUM_ENTITY_VOL_ADDR		STORE_START_ADDR+21           	//实际体积
#define SUM_CAIL_VOL_ADDR    	STORE_START_ADDR+29           	//实际体积




#define STORE_NUM				20

#define STORE_NUM_ADDR        	1000            	//最后的存储编号地址
#define STORE_NUM_FLAG_ADDR     1001            	//最后的存储编号地址


typedef struct REVIEW_DATA
{
  // unsigned char    smpNum;     //存储的编号   
   TIME             curTime;    //采样时刻 
   unsigned int     flow;       //设置流量 
   
   float_x          envT;       //计温
   float_x          envP;       //计压
   
   unsigned long    sumTime;      //累计时间（s）
   double_x         entity_V;    //实际容量 ml
   double_x         standard_V; //标准容量       
   
}REVIEW_DATA;

extern void delay(long int time);
#endif