
#ifndef _TIMER_H_
#define _TIMER_H_

#include "porttype.h"
#include "os_cfg.h"

#if  OS_TIMER_EN == 1


#define  TIMER_START          1
#define  TIMER_STOP           0
#define  ONE_SHORT_TIMER      0
#define  PERIOD_TIMER         1


struct TIMER
{
  void (*timerFun)(void);  //定时器要执行的句柄
  
  UINT16 timerType;        //定时器类型
  
  UINT16 tickCnt;          //定时的当前计数值
  
  UINT16 ticks;            //定时器的ticks数值
  
  UINT16 state;            //只是定时器的启动状态的标志
  
  struct TIMER *nextTimer;
};




void setupTimer(struct TIMER *timer,void(*timerFun)(void),UINT16 type,UINT16 ticks);

void setTimerType(struct TIMER *timer,UINT16 type);

void setTimerTicks(struct TIMER *timer,UINT16 ticks);

void startTimer(struct TIMER *timer);

void timerCnt(void);    //定时器计数

void timerTask(void);   //定时器任务

#endif



#endif