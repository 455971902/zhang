
#include "bsp.h"
#include "chip.h"
#include "MSP430x54x.h"
#include "tick.h"
#include "isr.h"
#include "ds1302.h"

void initClock(void)      //初始化定时器函数
{
    TA0CCR0 = 16;
    
    TA0CTL |= TASSEL__ACLK;
    
    TA0CCTL0 |= CCIE;
    
    TA0CTL |= TACLR + MC__UP;
}


void initBsp(void)
{
    // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  Init_time_io();
  init_Ucs();                        //初始化时钟
  initClock();
}

#pragma vector=TIMER0_A0_VECTOR


__interrupt void timerA0(void)
{
   runIsr(osTick,0);
}