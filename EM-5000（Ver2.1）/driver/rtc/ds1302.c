

#include "ds1302.h"
#include  <msp430x54x.h>


void  DS1302_charging(void);
void DS1302_Reset(void) ;
void Init_time_io()
{
   P1DIR |= 0XE0;
   
   DS1302_Reset();
   DS1302_charging();
}

//��ʱ���� 
void DS1302_Delay(unsigned int dtime) 
{ 
 unsigned int temp; 
 for(temp=0;temp<dtime;temp++); 
} 


//DS1302��λ 
void DS1302_Reset(void) 
{ 
 ds1302_sclk_clr; 
 ds1302_ce_clr; 
 //DS1302_Delay(10); 
 ds1302_sclk_set; 
} 


//��DS1302д��һ���ֽ� 
void DS1302_Write(unsigned int w_dat) 
{ 
  unsigned int i; 
  
  ds1302_ce_set; 
  for(i=8;i>0;i--) 
  { 
    ds1302_sda_clr; 
    if(w_dat&BIT0) ds1302_sda_set; 
    ds1302_sclk_clr; 
    //DS1302_Delay(10); 
    ds1302_sclk_set; 
    //DS1302_Delay(10); 
    w_dat >>= 1; 
  }  
} 


//��DS1302�ж�ȡһ���ֽ� 
unsigned int DS1302_Read(void) 
{  
unsigned int i=0,rdata=0; 
  
  ds1302_sda_r;

  ds1302_ce_set; 
  for(i=0;i<8;i++)
  {
    ds1302_sclk_set; 
    //DS1302_Delay(10); 
    ds1302_sclk_clr; 
    //DS1302_Delay(10); 
    rdata >>= 1;//����λ�ŵ�ǰ�棬������������һλ 
    if(ds1302_read_hi)rdata |= 0x80; 
  } 
  ds1302_sda_w;
  
  return  rdata;
} 


//��DS1302��д���ַ��д������ 
void DS1302_WriteData(unsigned int addr,unsigned int w_dat) 
{ 
  ds1302_ce_clr; 
  ds1302_sclk_clr; 
  ds1302_ce_set; 
  
  DS1302_Write(addr);  //д���ַ 
  DS1302_Write(w_dat);  //д������ 
  
  ds1302_sclk_set; 
  ds1302_ce_clr; 
}  


//��DS1302д���ַ��,��DS1302�ж�ȡ���� 
unsigned int DS1302_ReadData(unsigned int addr) 
{ 
  unsigned int r_dat=0; 
  
  ds1302_ce_clr; 
  ds1302_sclk_clr; 
  ds1302_ce_set; 
  
  DS1302_Write(addr);  //д���ַ 
  r_dat = DS1302_Read();  //�������� 
  
  ds1302_sclk_clr; 
  ds1302_ce_clr; 
  
  return(r_dat); 
} 


///*************************************////


//����SettingData����������DS1302��ʱ�� 


void Set_time(unsigned char shi,unsigned char min,unsigned char sec,unsigned char date,unsigned char month,unsigned char year) 
{ 
  DS1302_WriteData(0x8E,0x00);  //д���������,����д���� 
 
  shi=(shi/10)*16 + shi%10;          //10����תBCD��
  DS1302_WriteData(0x84,shi);
  min=(min/10)*16 + min%10;
  DS1302_WriteData(0x82,min);
  sec=(sec/10)*16 + sec%10;
  DS1302_WriteData(0x80,sec);
 
  date=(date/10)*16 + date%10;
  DS1302_WriteData(0x86,date);
  
  month=(month/10)*16 + month%10;
  DS1302_WriteData(0x88,month);
  
  
  year=(year/10)*16 + year%10;
  DS1302_WriteData(0x8c,year);
  
  DS1302_WriteData(0x8E,0x80);  //д���������,����д���� 
} 
//��ȡDS1302ʱ�䵽ReadingData�� 

void Get_time(TIME *tim) 
{ 
    _DINT();
    tim->date = DS1302_ReadData(0x87);
	tim->date = (tim->date / 16)*10 + (tim->date%16);
    
    tim->month = DS1302_ReadData(0x89);
	tim->month = (tim->month / 16)*10 + (tim->month%16);
   
    tim->year = DS1302_ReadData(0x8d);
	tim->year = (tim->year / 16)*10 + (tim->year%16);
	
    tim->hour = DS1302_ReadData(0x85);//
    tim->hour = (tim->hour / 16)*10 + (tim->hour%16);//BCD��ת��������,��ʽ1
	
    tim->hour = DS1302_ReadData(0x85);//
    tim->hour = (tim->hour / 16)*10 + (tim->hour%16);//BCD��ת��������,��ʽ1
    //str[i] = (str[i]>>4)*10 + (str[i] & 0x0f);//BCD��ת��������,��ʽ2
    tim->min  = DS1302_ReadData(0x83);
	tim->min = (tim->min / 16)*10 + (tim->min%16);
	tim->sec = DS1302_ReadData(0x81);
	tim->sec = (tim->sec / 16)*10 + (tim->sec%16);
   _EINT();
} 

void DS1302_charging(void) 
{ 
    
  DS1302_WriteData(0x8E,0x00);  //д���������,����д���� 
  
  DS1302_WriteData(0x90,0xA6);  //���ó��,��ʽ:һ��������,4K����,����Լ1mA

  DS1302_WriteData(0x8E,0x80);  //д���������,����д���� 
} 