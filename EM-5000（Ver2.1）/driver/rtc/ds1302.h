
#ifndef  _ds1302_h
#define  _ds1302_h 


//硬件连接: 
////          MCU                       DS1302 
////   ------------------         ------------------ 
////   |      P1.7       | ------> |     SCLK      5 | 
////   |      P1.6       | ------> |     SDA       6 | I/O
////   |      P1.5       | ------> |     RST       7 | CE
////   ------------------         ------------------ 
////   ------------------         ------------------ 

#define ds1302_ce_set     P1OUT |=  BIT7
#define ds1302_ce_clr     P1OUT &=~ BIT7 

#define ds1302_sclk_set   P1OUT |=  BIT5
#define ds1302_sclk_clr   P1OUT &=~ BIT5 

///////////////////////

#define ds1302_sda_set    P1OUT |=  BIT6 
#define ds1302_sda_clr    P1OUT &=~ BIT6 

#define ds1302_read_hi    (P1IN & BIT6)==BIT6

#define ds1302_sda_w      P1DIR |=  BIT6 
#define ds1302_sda_r      P1DIR &=~ BIT6 

/*********************************************************/
typedef struct times
{
  unsigned char year;        //年
  unsigned char month;       //月  
  unsigned char date;        //日
  unsigned char hour;        //时
  unsigned char min;         //分
  unsigned char sec;         //秒
}TIME;


void Init_time_io();
void Set_time(unsigned char shi,unsigned char min,unsigned char sec,unsigned char date,unsigned char month,unsigned char year);
void Get_time(TIME *tim) ;

////////////////////////////////////////////////////END


/*void DS1302_Delay(unsigned int dtime); 
void DS1302_Reset(void); 

unsigned int DS1302_Read(void); 
void DS1302_Write(unsigned int w_dat); 


void DS1302_Setting(void); 
void DS1302_GetData(unsigned char *str); 

unsigned int DS1302_ReadData(unsigned int addr);
void DS1302_WriteData(unsigned int addr,unsigned int w_dat); 


void DS1302_charging(void); */

////////////////////////////////////////////////////

/*
typedef struct 
{
  Uint16 yy;  //年份
  Uint16 wk;  //星期
  Uint16 mm;  //月份    
  Uint16 dd;  //日期     
  Uint16 hor; //小时
  Uint16 min; //分钟
  Uint16 sec; //秒钟
}time_struct;

time_struct ds1302_read;

time_struct ds1302_setting;
*/

#endif