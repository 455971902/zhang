
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
  void (*timerFun)(void);  //��ʱ��Ҫִ�еľ��
  
  UINT16 timerType;        //��ʱ������
  
  UINT16 tickCnt;          //��ʱ�ĵ�ǰ����ֵ
  
  UINT16 ticks;            //��ʱ����ticks��ֵ
  
  UINT16 state;            //ֻ�Ƕ�ʱ��������״̬�ı�־
  
  struct TIMER *nextTimer;
};




void setupTimer(struct TIMER *timer,void(*timerFun)(void),UINT16 type,UINT16 ticks);

void setTimerType(struct TIMER *timer,UINT16 type);

void setTimerTicks(struct TIMER *timer,UINT16 ticks);

void startTimer(struct TIMER *timer);

void timerCnt(void);    //��ʱ������

void timerTask(void);   //��ʱ������

#endif



#endif