


#include "system.h"
#include "lcd.h"

SYSTEM* lpsys;

///////////////////////////////////////////////////////////////////////////////

SYSTEM* new_system(void)   //创建系统函数，函数将返回一个SYSTEM指针
{
  static SYSTEM sys;

  
  lpsys=&sys;
  
  
  lpsys->sys_time.hour=0;
  lpsys->sys_time.min=0;
  lpsys->sys_time.sec=0;
  
  lpsys->battery.level=0;
  lpsys->battery.level_old=0;
  lpsys->battery.vol=0;
  lpsys->battery.vol_old=0;
  lpsys->battery.change=0;
  lpsys->battery.charge=0;
  lpsys->battery.x_pos_icn=109;
  lpsys->battery.y_pos_icn=2;
  lpsys->battery.icn=battery_icn;
  //lpsys->battery.icn_big=0;
  
  lpsys->mod=SINGLE_MOD;
  lpsys->lag=ENGLISH;
  lpsys->clk1=0;
  lpsys->clk2=0;
  lpsys->clk3=0;
  
  lpsys->set_battery=set_battery;
  lpsys->bat_display=bat_display;
  lpsys->set_time=set_time;
  lpsys->dis_time=dis_time;
  
  lpsys->temp_hour=1;
  lpsys->temp_min=1;
  lpsys->temp_sec=2;
  
  lpsys->a_hour=0;
  lpsys->a_min=0;
  lpsys->a_sec=0;
  
  lpsys->a_flow=0;
  
  
  return &sys;
}

//////////////////////////////////////////////////////////////////////////////

void set_battery(unsigned int ADCMEM)               //设置电池电量函数
{
  lpsys->battery.vol=ADCMEM*2.5/4095;
  
  if(lpsys->battery.vol!=lpsys->battery.vol_old)
  {
    lpsys->battery.vol_old=lpsys->battery.vol;
    if(lpsys->battery.vol>=2.2)
    {
      lpsys->battery.level=LEVEL4;
    }else if(lpsys->battery.vol<2.2 && lpsys->battery.vol>=1.8)
    {
      lpsys->battery.level=LEVEL3;
    }else if(lpsys->battery.vol<1.8 && lpsys->battery.vol>=1.4)
    {
      lpsys->battery.level=LEVEL2;
    }else if(lpsys->battery.vol<1.4 && lpsys->battery.vol>=1.0)
    {
      lpsys->battery.level=LEVEL1;
    }else if(lpsys->battery.vol<1.0)
    {
      lpsys->battery.level=LEVEL0;
    }
    
    if(lpsys->battery.level!=lpsys->battery.level_old)
    {
      lpsys->battery.level_old=lpsys->battery.level;
      lpsys->battery.change=1;
    }
  }
}

void bat_display(int mod)                 //电量显示函数
{
  if((lpsys->battery.change && lpsys->battery.icn) || mod==1)
  {
    lpsys->battery.change=0;
    draw_symbol(lpsys->battery.x_pos_icn,
                lpsys->battery.y_pos_icn,
                18,8,
                lpsys->battery.icn+lpsys->battery.level*18,
                0);
  }
}

void set_time(unsigned long time)           //设置时间函数
{
  
}

void dis_time(int mod)                             //显示时间函数
{
  
  int unit,decase;
  
  if(lpsys->temp_hour!=lpsys->sys_time.hour || mod==1)
  {
    lpsys->temp_hour=lpsys->sys_time.hour;
    if(lpsys->temp_hour<10)
    {
      putchar(97,50,'0',0);
      putchar(103,50,lpsys->temp_hour+'0',0);
    }else
    {
      unit=lpsys->temp_hour%10;
      putchar(103,50,unit+'0',0);
      decase=lpsys->temp_hour-unit;
      decase/=10;
      putchar(97,50,decase+'0',0);
    }
  }
  
  if(lpsys->temp_sec!=lpsys->sys_time.sec || mod==1)
  {
    lpsys->temp_sec=lpsys->sys_time.sec;
    if(lpsys->temp_sec&0x01)
    {
      putchar(109,50,':',0);
    }else
    {
      putchar(109,50,' ',0);
    }
  }
  
  if(lpsys->temp_min!=lpsys->sys_time.min || mod==1)
  {
    lpsys->temp_min=lpsys->sys_time.min;
    
    if(lpsys->temp_min<10)
    {
      putchar(115,50,'0',0);
      putchar(121,50,lpsys->temp_min+'0',0);
    }else
    {
      unit=lpsys->temp_min%10;
      putchar(121,50,unit+'0',0);
      decase=lpsys->temp_min-unit;
      decase/=10;
      putchar(115,50,decase+'0',0);
    }
  }
}


