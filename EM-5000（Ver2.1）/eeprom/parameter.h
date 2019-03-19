/************************************************************
** ��Ȩ:   AMAE
** �ļ���: parameter.h
** �汾��  
** ��������: IAR 5.10
** ����:   ����
** ��������: 2014.02.10
** �ļ�����: ���������ļ�
** ����ļ�: 
** �޸���־��         
*************************************************************/
#ifndef _PARAMETER_H_
#define _PARAMETER_H_

#include "ds1302.h"
#include "chip.h"

//-----------------��������Ÿ�ʽ-----------------------
//
typedef union
{
    float f;
    unsigned char data_F[4];
}float_x;  

//-----------------˫���ȸ�������Ÿ�ʽ-----------------------
//
typedef union
{
    double d;
    unsigned char data_D[8];
}double_x; 

//-----------------�������ô洢��ַ-----------------------
#define SET_START_ADDR        		1                  //���ò����洢����ʼ��ַ
#define SET_MOD_ADDR				SET_START_ADDR

#define CRC_NUM						0xfe

//�ֶ�������ַ
#define	SET_MANU_PARA_ADDR			SET_START_ADDR+1

#define	SET_MANU_FLOW_ADDR			SET_MANU_PARA_ADDR
#define	SET_MANU_SMP_TIME_ADDR		SET_MANU_PARA_ADDR+2
#define	SET_MANU_TEMP_FLAG_ADDR		SET_MANU_PARA_ADDR+4
#define	SET_MANU_TEMP_ADDR			SET_MANU_PARA_ADDR+5

#define SET_MANU_PARA_FLAG_ADDR		SET_MANU_PARA_ADDR+7

//��ʱ������ַ							
#define	SET_GROUP_PARA_ADDR			SET_START_ADDR+10

#define SET_GROUP_START_TIME_ADDR   SET_GROUP_PARA_ADDR  
#define SET_GROUP_FLOW_ADDR			SET_GROUP_PARA_ADDR+2  
#define SET_GROUP_SMP_TIME_ADDR   	SET_GROUP_PARA_ADDR+4
#define SET_GROUP_SRB_ADDR			SET_GROUP_PARA_ADDR+6  

#define SET_GROUP_PARA_FLAG_ADDR	SET_GROUP_PARA_ADDR+7

#define	SET_PARA_NUM				8

//ѭ��������ַ							
#define	SET_CLV_PARA_ADDR			SET_START_ADDR+100

#define SET_CLV_FLOW_ADDR   		SET_CLV_PARA_ADDR  
#define SET_CLV_MID_TIME_ADDR		SET_CLV_PARA_ADDR+2  
#define SET_CLV_SMP_TIME_ADDR   	SET_CLV_PARA_ADDR+4
#define SET_CLV_SMP_NUM_ADDR		SET_CLV_PARA_ADDR+6

#define SET_CLV_PARA_FLAG_ADDR		SET_CLV_PARA_ADDR+7

//���ݲ�����ַ							
#define	SET_CAP_PARA_ADDR			SET_START_ADDR+110

#define SET_CAP_VOL_ADDR   			SET_CAP_PARA_ADDR  
#define SET_CAP_FLOW_ADDR			SET_CAP_PARA_ADDR+8 
#define SET_CAP_MID_TIME_ADDR		SET_CAP_PARA_ADDR+10  
#define SET_CAP_SMP_NUM_ADDR		SET_CAP_PARA_ADDR+12

#define SET_CAP_PARA_FLAG_ADDR		SET_CAP_PARA_ADDR+13





//-----------------�����ݲ�ѯ�洢��ַ-----------------------
#define STORE_START_ADDR      150
#define STORE_PARA_NUM		  38


#define STORE_FLAG_ADDR         STORE_START_ADDR           		//д���־
#define START_TIME_ADDR       	STORE_START_ADDR+1            	//����ʱ��
#define FLOW_ADDR             	STORE_START_ADDR+7            	//��������
#define TEMP_ADDR             	STORE_START_ADDR+9           	//�¶�
#define PREV_ADDR             	STORE_START_ADDR+13           	//ѹǿ

#define SUM_TIME_ADDR         	STORE_START_ADDR+17           	//�ۼ�ʱ��
#define SUM_ENTITY_VOL_ADDR		STORE_START_ADDR+21           	//ʵ�����
#define SUM_CAIL_VOL_ADDR    	STORE_START_ADDR+29           	//ʵ�����




#define STORE_NUM				20

#define STORE_NUM_ADDR        	1000            	//���Ĵ洢��ŵ�ַ
#define STORE_NUM_FLAG_ADDR     1001            	//���Ĵ洢��ŵ�ַ


typedef struct REVIEW_DATA
{
  // unsigned char    smpNum;     //�洢�ı��   
   TIME             curTime;    //����ʱ�� 
   unsigned int     flow;       //�������� 
   
   float_x          envT;       //����
   float_x          envP;       //��ѹ
   
   unsigned long    sumTime;      //�ۼ�ʱ�䣨s��
   double_x         entity_V;    //ʵ������ ml
   double_x         standard_V; //��׼����       
   
}REVIEW_DATA;

extern void delay(long int time);
#endif