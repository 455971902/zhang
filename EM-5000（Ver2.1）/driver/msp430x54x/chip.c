


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
 
    

  
 // UCSCTL6|=XCAP_3;          //ʹXT1ģ�����Ϊ12pF
  P7SEL|=0x03;              //ʹXIN,XOUT���ſ�ͨ
   UCSCTL6|=XT2DRIVE_3;      //ʹXT2�����ⲿ�����Ƶ��Ϊ20Mhz
   UCSCTL6&=~XT2OFF;         //����XT2ģ��
 
  UCSCTL4|=SELA__XT1CLK;             //ʹACLKΪXT1
  UCSCTL3|=SELREF__XT1CLK;           //ʹPLL�Ĳο�ʱ��ΪXT1   FLLD__32
  
  UCSCTL4|=SELM__XT2CLK;             //ѡ��XT2CLKΪMCLK             
  UCSCTL4|=SELS__XT2CLK;
  
 // UCSCTL5|=DIVM_0 + DIVS_0;           //MCLK=MCLK���÷�Ƶϵ��
  

  P5SEL|=0x0c;              //��ͨXT2IN,XT2OUT����
  P5DIR=0x08;               //���ø�ͨ��I/O�ڵķ���
 
  
  do{
    
    UCSCTL7&=~XT1LFOFFG;    //��λXT1��ʧЧ��־
    UCSCTL7&=~DCOFFG;       //��λDCO��ʧЧ��־
    UCSCTL7&=~XT2OFFG;      //��λXT2��ʧЧ��־
    SFRIFG1&=~OFIFG;        //��λʱ�Ӵ����־OFIFG

    for(i=0;i<0xff;i++)     //��ʱһ��ʱ��
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
       }               //�⿴�Ź�
     
    }
    else
    {
     ct=0;
     DCO=1;
     goto DCO;
    }
   
    
  }while(XT1LFOFFG&UCSCTL7 || DCOFFG&UCSCTL7 || XT2OFFG&UCSCTL7); //�ж��Ƿ�����ʧЧ��־�Ѹ�λ

  DCO=0;
  goto end; 
    
    
    
    
    
    
    
  
DCO:
       //unsigned int i;
       
       UCSCTL4=SELM__DCOCLKDIV+SELS__DCOCLKDIV;             //ѡ��XT2CLKΪMCLK             

       UCSCTL6|=XT2OFF;             //�ر� XT2ģ��
       
       UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
       UCSCTL1 = DCORSEL_7;                      // Select DCO range 24MHz operation
       UCSCTL2 = FLLD_1 + 548;//548;             // Set DCO Multiplier for 
       
       UCSCTL6 &= ~(XT1OFF);                     // XT1 On
       UCSCTL6 |= XCAP_3;                        // Internal load cap
      do{
    
         UCSCTL7&=~XT1LFOFFG;    //��λXT1��ʧЧ��־
         UCSCTL7&=~DCOFFG;       //��λDCO��ʧЧ��־
         UCSCTL7&=~XT2OFFG;      //��λXT2��ʧЧ��־
         SFRIFG1&=~OFIFG;        //��λʱ�Ӵ����־OFIFG

          for(i=0;i<0xff;i++)     //��ʱһ��ʱ��
          {
            _NOP();
          }
    
        }while(XT1LFOFFG&UCSCTL7 || DCOFFG&UCSCTL7 || XT2OFFG&UCSCTL7); //�ж��Ƿ�����ʧЧ��־�Ѹ�λ  
     
       
   
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