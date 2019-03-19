//*****************************Copyright(c)************************************* 
//** ��      ˾��         AMAE 
//** ��      Ŀ��         ADS130    
//** Ӳ���汾�ţ�       
//**================================================================== 
//**------------------------------------ �ļ���Ϣ-------------------------------
//** ��  �� ��:      eeprom.c
//** �ļ� ����:      ���ⲿeepromͨѶ�Ĵ������ӿ�
//** ��     ��:      v0.1
//** ��  �� ��:      ����
//** ���� ����:      2012.8.29
//**============================================================================
//ͷ�ļ�
//=============================================================================
#include "eeprom.h"
#include "parameter.h"
#include "MSP430x54x.h"
#include "iic.h"


unsigned char setMod = 0;

MANU setManuParameter;					//�ֶ����ò���
__no_init GROUP setGroupParameter[10];	//��ʱ���ò���
CYL   setCylsParameter;              	// ѭ�������ṹ��
CLV	  setCapParameter;				  	//����ģʽ�ṹ��

__no_init REVIEW_DATA storeData;

unsigned char lastStoreNum = 0;

//==============================================================================
//�������ƣ�void eeprom_ReadMod(void)
//���������void
//���������void
//����˵������ȡ����ģʽ
//����λ�ã�
//��   ע��
//============================================================================
void eeprom_ReadMod(void)
{
	readByte(SET_MOD_ADDR,&setMod);
}
//==============================================================================
//�������ƣ�void eeprom_WriteMod(void)
//���������void
//���������void
//����˵����д����ģʽ
//����λ�ã�
//��   ע��
//============================================================================
void eeprom_WriteMod(unsigned char mod)
{
	writeByte(SET_MOD_ADDR,mod);
}
//==============================================================================
//�������ƣ�void eeprom_ReadManuSetPara(void)
//���������void
//���������void
//����˵������ȡ�ֶ�����
//����λ�ã�
//��   ע��
//============================================================================
void eeprom_ReadManuSetPara(void)
{
	unsigned char flag = 0;
	
	readByte(SET_MANU_PARA_FLAG_ADDR,&flag);
	
	if(flag == CRC_NUM)
	{
		readWord(SET_MANU_FLOW_ADDR, &setManuParameter.flow);
		readWord(SET_MANU_SMP_TIME_ADDR,&setManuParameter.smp_time);
		readByte(SET_MANU_TEMP_FLAG_ADDR,&setManuParameter.temp_flag);
		readWord(SET_MANU_TEMP_ADDR, &setManuParameter.temperature);
	}
	else
	{
		setManuParameter.flow = ORIGIN_FLOW;
		setManuParameter.smp_time = ORIGIN_SMP_TIME;
		setManuParameter.temp_flag = 0;
		setManuParameter.temperature = ORIGN_TEMP;			
	}
	
}
//==============================================================================
//�������ƣ�void eeprom_WriteManuSetPara()
//���������void
//���������void
//����˵����д�ֶ�����
//����λ�ã�
//��   ע��
//============================================================================
void eeprom_WriteManuSetPara(MANU val)
{
    writeWord(SET_MANU_FLOW_ADDR, val.flow);
    writeWord(SET_MANU_SMP_TIME_ADDR,val.smp_time);
    writeByte(SET_MANU_TEMP_FLAG_ADDR,val.temp_flag);
	writeWord(SET_MANU_TEMP_ADDR, val.temperature);	
	writeByte(SET_MANU_PARA_FLAG_ADDR,CRC_NUM);
}
//==============================================================================
//�������ƣ�void eeprom_ReadGroupSetPara(char num)
//���������num:���
//���������void
//����˵������ȡ��ʱ��������
//����λ�ã�main
//��   ע��
//============================================================================
void eeprom_ReadGroupSetPara( char num)
{
	unsigned char flag = 0;
	
	readByte(SET_GROUP_PARA_FLAG_ADDR+num*SET_PARA_NUM,&flag);
	
	if(flag == CRC_NUM)
	{
		readByte(SET_GROUP_START_TIME_ADDR+num*SET_PARA_NUM, &setGroupParameter[num].start_time.hour);
		readByte(SET_GROUP_START_TIME_ADDR+num*SET_PARA_NUM+1, &setGroupParameter[num].start_time.min);
		
		readWord(SET_GROUP_FLOW_ADDR+num*SET_PARA_NUM,&setGroupParameter[num].flow);    
		readWord(SET_GROUP_SMP_TIME_ADDR+num*SET_PARA_NUM, &setGroupParameter[num].smp_time);
		readByte(SET_GROUP_SRB_ADDR+num*SET_PARA_NUM,&setGroupParameter[num].srb);
	}
	else
	{
		setGroupParameter[num].start_time.hour = 0;
		setGroupParameter[num].start_time.min = 0;
		setGroupParameter[num].flow = ORIGIN_FLOW;
		setGroupParameter[num].smp_time = ORIGIN_SMP_TIME;
		setGroupParameter[num].srb = 0;
	}
}

//==============================================================================
//�������ƣ�void eeprom_ReadGroupSetPara(char num)
//���������num:���
//���������void
//����˵������ȡ��ʱ��������
//����λ�ã�main
//��   ע��
//============================================================================
void eeprom_WriteGroupSetPara(GROUP val,char num)
{
    writeByte(SET_GROUP_START_TIME_ADDR+num*SET_PARA_NUM, val.start_time.hour);
	writeByte(SET_GROUP_START_TIME_ADDR+num*SET_PARA_NUM+1, val.start_time.min);
	
    writeWord(SET_GROUP_FLOW_ADDR+num*SET_PARA_NUM,val.flow);    
	writeWord(SET_GROUP_SMP_TIME_ADDR+num*SET_PARA_NUM, val.smp_time);
	writeByte(SET_GROUP_SRB_ADDR+num*SET_PARA_NUM,val.srb);
	
	writeByte(SET_GROUP_PARA_FLAG_ADDR+num*SET_PARA_NUM,CRC_NUM);
}
//==============================================================================
//�������ƣ�void eeprom_ReadCylsSetPara(void)
//���������void
//���������void
//����˵������ȡѭ����������
//����λ�ã�main
//��   ע��
//============================================================================
void eeprom_ReadCylsSetPara(void)
{
	unsigned char flag = 0;
	
	readByte(SET_CLV_PARA_FLAG_ADDR,&flag);
	
	if(flag == CRC_NUM)
	{
		readWord(SET_CLV_FLOW_ADDR,&setCylsParameter.flow);    
		readWord(SET_CLV_MID_TIME_ADDR, &setCylsParameter.mid_time);
		readWord(SET_CLV_SMP_TIME_ADDR,&setCylsParameter.smp_time);    
		readByte(SET_CLV_SMP_NUM_ADDR,&setCylsParameter.cyl_num);
	}
	else
	{
		setCylsParameter.flow = ORIGIN_FLOW;
		setCylsParameter.mid_time = ORIGN_MID_TIME;
		setCylsParameter.smp_time = ORIGIN_SMP_TIME;
		setCylsParameter.cyl_num = ORIGN_NUM;
		
	}
}
//==============================================================================
//�������ƣ�void eeprom_ReadCylsSetPara(void)
//���������void
//���������void
//����˵����дѭ����������
//����λ�ã�main
//��   ע��
//============================================================================
void eeprom_WriteCylsSetPara(CYL var)
{
    writeWord(SET_CLV_FLOW_ADDR,var.flow);    
	writeWord(SET_CLV_MID_TIME_ADDR, var.mid_time);
    writeWord(SET_CLV_SMP_TIME_ADDR,var.smp_time);    
	writeByte(SET_CLV_SMP_NUM_ADDR,var.cyl_num);
	
	writeByte(SET_CLV_PARA_FLAG_ADDR,CRC_NUM);
}
//==============================================================================
//�������ƣ�void eeprom_ReadCapSetPara(void)
//���������void
//���������void
//����˵������ȡ���ݲ�������
//����λ�ã�main
//��   ע��
//============================================================================
void eeprom_ReadCapSetPara(void)
{
	unsigned char flag = 0;
	
	readByte(SET_CLV_PARA_FLAG_ADDR,&flag);
	
	if(flag == CRC_NUM)
	{
		readArrayData(SET_CAP_VOL_ADDR,setCapParameter.smp_volume.data_D,8);
		readWord(SET_CAP_FLOW_ADDR,&setCapParameter.flow);    
		readWord(SET_CAP_MID_TIME_ADDR, &setCapParameter.mid_time);
		readByte(SET_CAP_SMP_NUM_ADDR,&setCapParameter.cyl_num);
	}
	else
	{
		setCapParameter.smp_volume.d = ORIGN_VOL;
		setCapParameter.flow = ORIGIN_FLOW;
		setCapParameter.mid_time = ORIGN_MID_TIME;
		setCapParameter.cyl_num = ORIGN_NUM;
	}
}
//==============================================================================
//�������ƣ�void eeprom_WriteCapSetPara(void)
//���������void
//���������void
//����˵����д���ݲ�������
//����λ�ã�main
//��   ע��
//============================================================================
void eeprom_WriteCapSetPara(CLV var)
{
	writeArrayData(SET_CAP_VOL_ADDR,var.smp_volume.data_D,8);
    writeWord(SET_CAP_FLOW_ADDR,var.flow);    
	writeWord(SET_CAP_MID_TIME_ADDR, var.mid_time);
	writeByte(SET_CAP_SMP_NUM_ADDR,var.cyl_num);
	
	writeByte(SET_CAP_PARA_FLAG_ADDR,CRC_NUM);
}
//==============================================================================
//�������ƣ�void readFM28C64()
//���������void
//���������void
//����˵����
//����λ�ã�
//��   ע��
//============================================================================
void readAllReviewData(unsigned char num)
{
    unsigned char flag = 0;

	readByte(STORE_FLAG_ADDR+num*STORE_PARA_NUM,&flag);
	
	if(flag == CRC_NUM)
	{
		readByte(START_TIME_ADDR+num*STORE_PARA_NUM, &storeData.curTime.year);
		readByte(START_TIME_ADDR+num*STORE_PARA_NUM+1, &storeData.curTime.month);
		readByte(START_TIME_ADDR+num*STORE_PARA_NUM+2, &storeData.curTime.date);
		readByte(START_TIME_ADDR+num*STORE_PARA_NUM+3, &storeData.curTime.hour);
		readByte(START_TIME_ADDR+num*STORE_PARA_NUM+4, &storeData.curTime.min);
		readByte(START_TIME_ADDR+num*STORE_PARA_NUM+5, &storeData.curTime.sec);
		
		
		readWord(FLOW_ADDR+num*STORE_PARA_NUM,&storeData.flow);    
		readArrayData(TEMP_ADDR + num*STORE_PARA_NUM, storeData.envT.data_F,4); 
		readArrayData(PREV_ADDR + num*STORE_PARA_NUM, storeData.envP.data_F,4); 
		readDWord(SUM_TIME_ADDR + num*STORE_PARA_NUM, &storeData.sumTime);
		readArrayData(SUM_ENTITY_VOL_ADDR + num*STORE_PARA_NUM, storeData.entity_V.data_D,8);
		readArrayData(SUM_CAIL_VOL_ADDR + num*STORE_PARA_NUM, storeData.standard_V.data_D,8);
	}
	else
	{
		storeData.curTime.year = 0;
		storeData.curTime.month = 0;
		storeData.curTime.date = 0;
		storeData.curTime.hour = 0;
		storeData.curTime.min = 0;
		storeData.curTime.sec = 0;
		storeData.flow = 0;
		storeData.envT.f = 0;
		storeData.envP.f = 0;
		storeData.sumTime = 0;
		storeData.entity_V.d = 0;
		storeData.standard_V.d = 0;	
		
	}
}

//==============================================================================
//�������ƣ�void writeReviewData(unsigned char num)
//���������num:�洢�±�
//���������void
//����˵�����洢��������
//����λ�ã�
//��   ע��
//============================================================================
void writeReviewData(unsigned char num)
{	
	writeByte(START_TIME_ADDR+num*STORE_PARA_NUM, storeData.curTime.year);
	writeByte(START_TIME_ADDR+num*STORE_PARA_NUM+1, storeData.curTime.month);
	writeByte(START_TIME_ADDR+num*STORE_PARA_NUM+2, storeData.curTime.date);
	writeByte(START_TIME_ADDR+num*STORE_PARA_NUM+3, storeData.curTime.hour);
	writeByte(START_TIME_ADDR+num*STORE_PARA_NUM+4, storeData.curTime.min);
	writeByte(START_TIME_ADDR+num*STORE_PARA_NUM+5, storeData.curTime.sec);	
	
	writeWord(FLOW_ADDR+num*STORE_PARA_NUM,storeData.flow);    
	writeArrayData(TEMP_ADDR + num*STORE_PARA_NUM, storeData.envT.data_F,4); 
	writeArrayData(PREV_ADDR + num*STORE_PARA_NUM, storeData.envP.data_F,4); 
	writeDWord(SUM_TIME_ADDR + num*STORE_PARA_NUM, storeData.sumTime);
	writeArrayData(SUM_ENTITY_VOL_ADDR + num*STORE_PARA_NUM, storeData.entity_V.data_D,8);
	writeArrayData(SUM_CAIL_VOL_ADDR + num*STORE_PARA_NUM, storeData.standard_V.data_D,8);
	
	writeByte(STORE_FLAG_ADDR+num*STORE_PARA_NUM,CRC_NUM);
		
}

void writeReviewNum(void)
{	
	writeByte(STORE_NUM_ADDR,lastStoreNum);
	writeByte(STORE_NUM_FLAG_ADDR,CRC_NUM);
}

void readReviewNum(void)
{
	unsigned char temp = 0;
	
	readByte(STORE_NUM_FLAG_ADDR,&temp);
	
	if(temp == CRC_NUM)
	{
		readByte(STORE_NUM_ADDR,&temp);	
	}
	
	if(temp < 20)
	{
		lastStoreNum = temp;
	}
	else
	{
		lastStoreNum = 0;		
	}
}
