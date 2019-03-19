


#include "MSP430x54x.h"

#include "chip.h"
#include "lcd.h"
static unsigned int DCO=0;

unsigned int get_DCO(void)
{
  return DCO;
}
unsigned int ct=0;
extern void init_Ucs(void)
{ 
 unsigned int i;
   DCO=0;
  P6DIR |= BIT0; 
  P6OUT ^= BIT0;
  init_Lcd();
  clr_lcd();
 
    

  
 // UCSCTL6|=XCAP_3;          //使XT1模块电容为12pF
  P7SEL|=0x03;              //使XIN,XOUT引脚开通
   UCSCTL6|=XT2DRIVE_3;      //使XT2驱动外部晶振的频率为20Mhz
   UCSCTL6&=~XT2OFF;         //开启XT2模块
 
  UCSCTL4|=SELA__XT1CLK;             //使ACLK为XT1
  UCSCTL3|=SELREF__XT1CLK;           //使PLL的参考时钟为XT1   FLLD__32
  
  UCSCTL4|=SELM__XT2CLK;             //选择XT2CLK为MCLK             
  UCSCTL4|=SELS__XT2CLK;
  
 // UCSCTL5|=DIVM_0 + DIVS_0;           //MCLK=MCLK配置分频系数
  

  P5SEL|=0x0c;              //开通XT2IN,XT2OUT引脚
  P5DIR=0x08;               //设置该通用I/O口的方向
 
  
  do{
    
    UCSCTL7&=~XT1LFOFFG;    //复位XT1的失效标志
    UCSCTL7&=~DCOFFG;       //复位DCO的失效标志
    UCSCTL7&=~XT2OFFG;      //复位XT2的失效标志
    SFRIFG1&=~OFIFG;        //复位时钟错误标志OFIFG

    for(i=0;i<0xff;i++)     //延时一段时间
    {
      _NOP();
        
    }
    ct++;
     if(ct<500)
    {
       print_int(20,20,ct,0);
       // _NOP();
        //delay(1800);
       
        if(ct%100==0)
       {
        P6OUT ^= BIT0;
       }               //外看门狗
     
    }
    else
    {
     ct=0;
     DCO=1;
     goto DCO;
    }
   
    
  }while(XT1LFOFFG&UCSCTL7 || DCOFFG&UCSCTL7 || XT2OFFG&UCSCTL7); //判断是否三个失效标志已复位

  DCO=0;
  goto end; 
    
    
    
    
    
    
    
  
DCO:
       //unsigned int i;
       
       UCSCTL4=SELM__DCOCLKDIV+SELS__DCOCLKDIV;             //选择XT2CLK为MCLK             

       UCSCTL6|=XT2OFF;             //关闭 XT2模块
       
       UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
       UCSCTL1 = DCORSEL_7;                      // Select DCO range 24MHz operation
       UCSCTL2 = FLLD_1 + 548;//548;             // Set DCO Multiplier for 
       
       UCSCTL6 &= ~(XT1OFF);                     // XT1 On
       UCSCTL6 |= XCAP_3;                        // Internal load cap
      do{
    
         UCSCTL7&=~XT1LFOFFG;    //复位XT1的失效标志
         UCSCTL7&=~DCOFFG;       //复位DCO的失效标志
         UCSCTL7&=~XT2OFFG;      //复位XT2的失效标志
         SFRIFG1&=~OFIFG;        //复位时钟错误标志OFIFG

          for(i=0;i<0xff;i++)     //延时一段时间
          {
            _NOP();
          }
    
        }while(XT1LFOFFG&UCSCTL7 || DCOFFG&UCSCTL7 || XT2OFFG&UCSCTL7); //判断是否三个失效标志已复位  
     
       
   
end:;
  
  

}

extern void delay(long int time)
{
  int i; 

  for(i=0;i<time;i++)
  {
    _NOP();
  }
}