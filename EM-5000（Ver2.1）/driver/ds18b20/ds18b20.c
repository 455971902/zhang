
//*****************************Copyright(c)************************************* 
//** ��      ˾��         AMAE 
//** ��      Ŀ��         EM-1500    
//** Ӳ���汾�ţ�       
//**================================================================== 
//**------------------------------------ �ļ���Ϣ-------------------------------
//** ��  �� ��:      ds18b20.c
//** �ļ� ����:      �����¶ȼ��
//** ��     ��:      v0.1
//** ��  �� ��:      ����
//** ���� ����:      2014.1.20
//**============================================================================
#include "MSP430x54x.h"
#include "ds18b20.h"
#include "parameter.h"
#include "tick.h"

#include "port.h"

#define  READ_DS18B20       0XCC         //��ȡ��������
#define  TRANS_DS18B20      0X44         //ת������
#define  DATA_DS18B20       0XBE         //�¶ȼĴ�����ַ
#define  CONFIG_DS18B20     0x4e


unsigned char temperatureBuf[2] = {0};        //�¶Ȼ�����

unsigned char sn[4] = {0};                    //���кŻ�����

float env_temp = 25;

//==============================================================================
//�������ƣ�void start_ds18b20(void)
//���������void
//����������ظ���־
//����˵������λDS18B20
//����λ�ã�����仯ʱ����
//��   ע��
//============================================================================
unsigned int start_ds18b20(void)
{
  unsigned int flag = 0xff;
  
  DS18B20_OUT_1;  
  DS18B20_L_1;
       
  delay(900);       			//�ȴ�480~960us  
  
  DS18B20_H_1; 
 
  delay(70);        			//�ȴ�15~60us
  DS18B20_IN_1;
  flag = DS18B20_DATA_1;    	//����ʱ��Ϊ60us~240us     

  delay(200);   
  return flag;
}
//==============================================================================
//�������ƣ�void writeDs18b20Byte(unsigned char data)
//���������data:д������
//���������void
//����˵�����������ݵ�DS18B20
//����λ�ã�
//��   ע��
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
//�������ƣ�unsigned char readDs18b20Byte(void)
//���������void
//��������������¶ȴ����ֵ
//����˵��������DS18B20������
//����λ�ã�temperature_Adc()
//��   ע��
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
//�������ƣ�int temperature_Adc(void)
//���������void
//�������������ADֵ
//����˵������ȡ�����¶Ȳ���ֵ
//����λ�ã�tempTask()
//��   ע��
//============================================================================
void temperature_Adc(void)
{
  int i = 0;  

  start_ds18b20();
  ds18b20_write_byte(READ_DS18B20);    
  ds18b20_write_byte(TRANS_DS18B20);
					 
  delayTick(2000);
 
  start_ds18b20();
  ds18b20_write_byte(READ_DS18B20);             //����ROM
  ds18b20_write_byte(DATA_DS18B20);             //��DS18B20�¶Ȼ�����
  
  for(i = 0; i < 2; i++)
  {
    temperatureBuf[i] = ds18b20_read_byte();  //��ȡ�ɼ��¶�    
  }  
  start_ds18b20();                //��λ��
}

//==============================================================================
//�������ƣ�void init_Ds18b20(void)
//���������void
//���������void
//����˵������ʼ��DS18B20
//����λ�ã�
//��   ע��
//============================================================================
void readrom(void)          //read the serial ��ȡ�¶ȴ����������к�
{                      //��������û���õ��˺���
  start_ds18b20();
  delay(20);
  
  ds18b20_write_byte(0x33);

  sn[0] = ds18b20_read_byte();
  sn[1] = ds18b20_read_byte();  
  sn[2] = ds18b20_read_byte();
  sn[3] = ds18b20_read_byte();
}
//==============================================================================
//�������ƣ�unsigned char check_temp_trans(void)
//���������void
//���������0���쳣 1������
//����˵�����ɼ������¶ȼ�Һ��
//����λ�ã�main
//��   ע��
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
//�������ƣ�long TempTask(void)
//���������void
//���������void
//����˵�����ɼ������¶ȼ�Һ��
//����λ�ã�main
//��   ע��
//============================================================================
void TempTask(void)
{
    float temp = 0.0;
    unsigned int tempflag = 0;
    unsigned int temp_num = 0;
    
    while(1)
    {
        temperature_Adc();                      //�����¶Ȳ���
        
        tempflag = temperatureBuf[1];           //�����¶ȼ���
        temp_num =  (temperatureBuf[1] << 8)|temperatureBuf[0];            
        tempflag &= 0xf8;
        if((tempflag == 0xf8)&&(temp_num!= 0xffff))  //�������¼���
        {
          temp_num = ~temp_num +1; 
          temp = 0.0625*temp_num;
          env_temp = 0.0-temp;
        }
        else
        {
          temp = 0.0625*temp_num;                //�������¼���
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
					 