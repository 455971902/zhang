

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

//延时函数 
void DS1302_Delay(unsigned int dtime) 
{ 
 unsigned int temp; 
 for(temp=0;temp<dtime;temp++); 
} 


//DS1302复位 
void DS1302_Reset(void) 
{ 
 ds1302_sclk_clr; 
 ds1302_ce_clr; 
 //DS1302_Delay(10); 
 ds1302_sclk_set; 
} 


//向DS1302写入一个字节 
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


//从DS1302中读取一个字节 
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
    rdata >>= 1;//将移位放到前面，否则读不到最高一位 
    if(ds1302_read_hi)rdata |= 0x80; 
  } 
  ds1302_sda_w;
  
  return  rdata;
} 


//向DS1302中写入地址后写入数据 
void DS1302_WriteData(unsigned int addr,unsigned int w_dat) 
{ 
  ds1302_ce_clr; 
  ds1302_sclk_clr; 
  ds1302_ce_set; 
  
  DS1302_Write(addr);  //写入地址 
  DS1302_Write(w_dat);  //写入数据 
  
  ds1302_sclk_set; 
  ds1302_ce_clr; 
}  


//向DS1302写入地址后,从DS1302中读取数据 
unsigned int DS1302_ReadData(unsigned int addr) 
{ 
  unsigned int r_dat=0; 
  
  ds1302_ce_clr; 
  ds1302_sclk_clr; 
  ds1302_ce_set; 
  
  DS1302_Write(addr);  //写入地址 
  r_dat = DS1302_Read();  //读出数据 
  
  ds1302_sclk_clr; 
  ds1302_ce_clr; 
  
  return(r_dat); 
} 


///*************************************////


//按照SettingData的设置设置DS1302的时间 


void Set_time(unsigned char shi,unsigned char min,unsigned char sec,unsigned char date,unsigned char month,unsigned char year) 
{ 
  DS1302_WriteData(0x8E,0x00);  //写入控制命令,禁用写保护 
 
  shi=(shi/10)*16 + shi%10;          //10进制转BCD码
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
  
  DS1302_WriteData(0x8E,0x80);  //写入控制命令,启用写保护 
} 
//读取DS1302时间到ReadingData中 

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
    tim->hour = (tim->hour / 16)*10 + (tim->hour%16);//BCD码转换二进制,方式1
	
    tim->hour = DS1302_ReadData(0x85);//
    tim->hour = (tim->hour / 16)*10 + (tim->hour%16);//BCD码转换二进制,方式1
    //str[i] = (str[i]>>4)*10 + (str[i] & 0x0f);//BCD码转换二进制,方式2
    tim->min  = DS1302_ReadData(0x83);
	tim->min = (tim->min / 16)*10 + (tim->min%16);
	tim->sec = DS1302_ReadData(0x81);
	tim->sec = (tim->sec / 16)*10 + (tim->sec%16);
   _EINT();
} 

void DS1302_charging(void) 
{ 
    
  DS1302_WriteData(0x8E,0x00);  //写入控制命令,禁用写保护 
  
  DS1302_WriteData(0x90,0xA6);  //起用充电,方式:一个二极管,4K电阻,电流约1mA

  DS1302_WriteData(0x8E,0x80);  //写入控制命令,启用写保护 
} 