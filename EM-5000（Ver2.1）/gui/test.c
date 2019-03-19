#include<msp430x54x.h>
#include "TEST.H"
#include "DS1302.H"
#include  "pump.h"
#include "lcd.h"
#include "key.H"
#include "display.H"
#include "adc12.h"
#include "charger.h"
#include "ccd.h"
#include "bmp085.h"
#include "ds18b20.h"

#define SHUA_DELY 10
#define TEST_TS 300
#define OUT_DY  60

#define SHI_X 15

#define SHI_Y 18
#define QI_Y   33
#define DIAN_Y 48
#define OFF_DELY 100


unsigned char  test_flag=1;



volatile void delay_ms(unsigned int mm)
{
 unsigned int i,j;
 for(i=0;i<mm;i++)
     for(j=0;j<18000;j++)
     {
       _NOP();
     }

}





void powerOn(void)     //开机函数
{
     P1DIR |= BIT3;
     P1OUT &= ~BIT3;  
}

void powerOff(void)
{
 unsigned int i=0;
 static unsigned long delay;
 
 clr_lcd();
 draw_rect(99,37,50,2);
 draw_symbol(0,0,128,64,off,0);

 while(1)
 {
  
   i++;

   if(i==100+OFF_DELY)
   {
     clr_lcd();
     init_Lcd();
	 P1DIR |= BIT3;
     P1OUT |= BIT3;
     delay = 100000;
     while(delay)
     {
         delay --;
         P1OUT |= BIT3;
     }
     P1OUT &= ~BIT3;

     break;
   }

   delay_ms(1);
 

   WDTCTL = WDTPW + WDTCNTCL;  //喂狗
   P6OUT ^= BIT0 ;             //外看门狗

 }
}
void x_L_mov(unsigned int tick_up,unsigned char s_x,unsigned char s_y,unsigned char LONGS,unsigned char M_LONG,unsigned char S_LONG)
{
   static unsigned int i=0;
   

         for(i=0;i<LONGS;i++)
           { 
               
               
               
               if((tick_up+i)%M_LONG<S_LONG)
             {
               draw_point(i+s_x,s_y);
             }
             else 
               clr_point(i+s_x,s_y);
               
               
               
           }
         
}

void x_R_mov(unsigned int tick_up,unsigned char s_x,unsigned char s_y,unsigned char LONGS,unsigned char M_LONG,unsigned char S_LONG)
{
    static unsigned int i=0;
         for(i=0;i<LONGS;i++)
           { 
   
               if((tick_up+i)%M_LONG<S_LONG)
             {
               draw_point(s_x-i,s_y);
             }
             else 
               clr_point(s_x-i,s_y);
  
           }
}

void y_U_mov(unsigned int tick_up,unsigned char s_x,unsigned char s_y,unsigned char LONGS,unsigned char M_LONG,unsigned char S_LONG)
{   
   static unsigned int i=0;
         for(i=0;i<LONGS;i++)
           { 
               if((tick_up+i)%M_LONG<S_LONG)
              {
                draw_point(s_x,s_y+i);
              }
             else 
                clr_point(s_x,s_y+i);
  
           }
}

void y_D_mov(unsigned int tick_up,unsigned char s_x,unsigned char s_y,unsigned char LONGS,unsigned char M_LONG,unsigned char S_LONG)
{   
   static unsigned int i=0;
         for(i=0;i<LONGS;i++)
           { 
               if((tick_up+i)%M_LONG<S_LONG)
              {
                draw_point(s_x,s_y-i);
              }
             else 
                clr_point(s_x,s_y-i);
           }
}

char batterySupply(void)
{
	return 1;
}



void check(void)
{ 

    int ff=-2;
	float battery = 0.0;
	unsigned char temp = 0;
    
   P6DIR |= BIT0;
   WDTCTL = WDTPW + WDTCNTCL;  //喂狗
   P6OUT ^= BIT0 ;             //外看门狗

   P9DIR |= BIT6;
   P9OUT |= BIT6;

   test_flag = 1; 
   clr_lcd();
   
   draw_symbol(0 ,0 ,64,64,op ,0);
   draw_symbol(64,0 ,64,64,op1,0);        
  
   delay_ms(150);   
   
   while(test_flag)
   {        
       if(ff<64) 
      {          
           ff=ff+2;
           draw_symbol(0     ,0 ,64-ff,64,op+8*ff ,0); 
           draw_symbol(64+ff ,0 ,64-ff,64,op1     ,0);       
          if(ff>0)
          {
            draw_symbol(64-ff,0 ,2,64,HYSY+8*(64-ff),0);
            draw_symbol(62+ff,0 ,2,64,HYSY+8*(62+ff),0);
          }
          _NOP();_NOP();_NOP();_NOP();_NOP();_NOP();
        
             /*  if(ff==64)
       {
        draw_rect(15,43,90,5);
       }  */          
     }
     else        
     {        
       ff++;
       if(ff<80)
       {
           /* x_R_mov (ff,15+88,42+2,88,22,10);
            x_R_mov (ff,15+88,43+2,88,22,10);
            x_R_mov (ff,15+88,44+2,88,22,10);
            delay_ms(15);*/
		 	//draw_symbol(22,40 ,77,14,banben,0);
			battery = (float)(SOFTWARE+0.1) / 100;
    				
		 	print_str(30,42,"EM5000",0);
		 	print_float(70,42,battery,1,0);	
			
			delay_ms(22);
       }	   
	   else if(ff <=80 )
	   {
		   clr_lcd();
	   }
	   else if(ff <=89)
	   {
			print_ch_str(10,2,"自检",0); 		
			putchar(38,2,'.',0);
			putchar(44,2,'.',0);
			putchar(50,2,'.',0);
    		
			print_ch_str(20,28,"电池电量",0); 
			putchar(68,28,':',0);
			
			battery = (float)(getBattery1()-2700)*100/623;
			
			if(battery >=100)
			{
				battery = 100;
			}
			print_int(74,28,(int)battery,0);
			putchar(92,28,'%',0);
			delay_ms(22);
	   }
	   else if(ff <= 90)
	   {
		    clr_rect(18,20,80,12);
			delay_ms(2);
	   }
	   else if(ff <= 99)
	   {
			temp = check_temp_trans();
			print_ch_str(14+4,28,"温度传感器",0); 
			putchar(74+4,28,':',0);
			
			if(temp)
			{
				print_ch_str(80+4,28,"正常",0); 
			}
			else
			{
				print_ch_str(80+4,28,"异常",0); 
			}
			delay_ms(22);
	   }
	   else if(ff <=100)
	   {
		    clr_rect(18,28,80,12);
			delay_ms(2);
	   }
	   else if(ff <= 109)
	   {
		   	temp = check_press_trans();
			print_ch_str(14+3,28,"气压传感器",0); 
			putchar(74+3,28,':',0);
			
			if(temp)
			{
				print_ch_str(80+3,28,"正常",0); 
			}
			else
			{
				print_ch_str(80+3,28,"异常",0); 
			}		
			
			delay_ms(22);
	   }
	   else if(ff<=110)
	   {
	   	clr_lcd();
		delay_ms(2);
	   }
       else      
       {
        test_flag=0;
       }
       
       
     }
      
       WDTCTL = WDTPW + WDTCNTCL;  //喂狗
       P6OUT ^= BIT0 ;             //外看门狗

   }
   
   
}
