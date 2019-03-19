

#include "key.h"
#include "port.h"
#include "tick.h"
#include "MSP430x54x.h"


#define BUFSIZE 2

static char keyBuf[BUFSIZE];

static char index = 0;

void initKey(void)
{
    
    P2DIR = 0x00;
    
}


void initLcdBG(void)
{
    P9DIR |= BIT6; 
    
    P9OUT |= BIT6;
}



void scanKey(void)     //读取键值函数
{   
    unsigned long i = 0;
    
    static char keyVal = 0;
    
    static char key = 0;
    
    static char P2INS = 0;
    
    char oldKey = 0;
    
    int lcdDelay = 0;
    
    initKey();
    
    initLcdBG();   //启动背光灯
    
    
    for(i=0;i<BUFSIZE;i++)
    {
        keyBuf[i] = 0;
    }
    
    i = 0;
    
    while(1)
    {
    
       P2INS = P2IN;
       
       P2INS ^= BIT6;
        
       keyVal = P2INS & 0x7e;
    
       keyVal |= 0x81;
       
       keyVal = ~keyVal;
       
       key = keyVal;  //获得键值
       i = 0;
       
       
       while(key)
       {
           
           lcdDelay = 0;
           
           if(!(P9OUT & BIT6))
           {
               keyVal = 0;
               P9OUT |= BIT6;
               delayTick(400);
               break;
           }
           
           if(i<1)
           {
               delayTick(80);   //任务休眠30ms
           }else
           {
               delayTick(40);
           }
           
           i++;
           
           if(i>20)
           {
               if(keyVal & CENTERKEY)
               {
                   if(i > 100)
                   {
                       keyVal = LENTERKEY;
                       
                       break;
                   }
                   
               }else if(keyVal & CANCELKEY)
               {
                   if(i > 80)
                   {
                       keyVal = KEY_POWER;
                       break;
                   }
               }else if(!(key & CENTERKEY))
               {
                   
                   break;
               }
           }
           
           if(oldKey == LENTERKEY)
           {
                 keyVal = 0;
           }
           
           if(oldKey == keyVal && oldKey != CENTERKEY )
           {
               break;
           }
           
           P2INS = P2IN;
           
          P2INS ^= BIT6;
           
           key = ~((P2INS&0x7e)|0x81);
       }
       
       if(i > 20 && i< 100 && keyVal == CENTERKEY)
       {
           keyVal = 0;
       }
       
       oldKey = keyVal;

       if(keyVal != 0)
       {
           enterCritical();    //进入临街区
               
           if(index < BUFSIZE)
           {
                 index ++;
                 keyBuf[index - 1] = keyVal;
                 
           } 
           exitCritical();      //退出临界区
       }
       
       if(lcdDelay < 350)  //对背光灯点亮时间进行计数
       {
           lcdDelay ++;
       }else
       {
           P9OUT &= ~BIT6;
       }
       
       delayTick(100);
       WDTCTL = WDTPW + WDTCNTCL;  //喂狗
       P6OUT ^= BIT0;  
    }        
}




char getKey(void)
{
    char key = 0;
    
    int offset = 0;
    
    enterCritical();    //进入临街区           
    if(index>0)
    {
        key = keyBuf[0];
        
        for(offset=0;offset<index - 1;offset++)
        {
            keyBuf[offset] = keyBuf[offset+1];
        }
        keyBuf[index - 1] = 0;
        index --;
    }         
    exitCritical();      //释放信号量，退出临界区
    return key;

}


void clrKeyBuf(void)
{
  int i;
  enterCritical();
  
  for(i = 0; i < BUFSIZE; i++)
  {
    keyBuf[i] = 0;
  }
  
  index = 0;
  
  exitCritical();
}


#pragma vector = PORT2_VECTOR

__interrupt void port2Isr(void)
{
    
}