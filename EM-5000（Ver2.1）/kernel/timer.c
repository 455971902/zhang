

#include "timer.h"

#include "semaphore.h"

#include "port.h"

#include "tick.h"

#if  OS_TIMER_EN == 1

static struct TIMER *timerList = 0;          //��ʱ���б�ͷָ��

static struct TIMER *curRunTimer = 0;        //��ǰ��ִ�е�TIMER����ֹͬһ��TIMER�ľ��������Ƕ��

static struct SEMAPHORE timerSem;            //��ʱ�����ź���

void setupTimer(struct TIMER *timer,void(*timerFun)(void),UINT16 type,UINT16 ticks)
{
  struct TIMER *tmpTimer = 0;
  
  if(timer && timerFun)   //��ʱ���Ϸ����Ҷ�ʱ�������Ϸ�
  {
    
    if(ticks == 0)
    {
      ticks = 10;
    }
    
    timer->timerFun = timerFun;        //���ö�ʱ�����
    timer->timerType = type;           //���ö�ʱ������
    timer->tickCnt = 0;                //��ʼ����ֵ
    timer->ticks = ticks;              //������ֵ
    timer->state = 0;                  //��ʱ��δ����
    timer->nextTimer = 0;
    
    enterCritical();                   //�����ٽ���
    
    if(timerList == 0)
    {
      timerList = timer;               //�˶�ʱ��Ϊ�׶�ʱ��
    }else
    {
      tmpTimer = timerList;
      while(tmpTimer->nextTimer)
      {
        tmpTimer = tmpTimer->nextTimer;
      }
      
      tmpTimer->nextTimer = timer;
    }

    exitCritical();                    //�˳��ٽ���
  }
}

void setTimerType(struct TIMER *timer,UINT16 type)
{
  
  
}


void setTimerTicks(struct TIMER *timer,UINT16 ticks)
{
  
  
}



void startTimer(struct TIMER *timer)
{
  if(timer)
  {
    enterCritical();                 //�����ٽ���
    
    timer->state = TIMER_START;      //������ʱ��
    
    exitCritical();                  //�˳��ٽ���
  }
  
}



void timerCnt(void)     //��ʱ������
{
  struct TIMER *tmpTimer;
  
  enterCritical();
  
  if(timerList)
  {
    tmpTimer = timerList;
    
    while(tmpTimer)
    {
      if(tmpTimer->state == TIMER_START && tmpTimer->tickCnt < tmpTimer->ticks)   //�������Ķ�ʱ��
      {
        tmpTimer->tickCnt ++;
        
        if(tmpTimer->tickCnt == tmpTimer->ticks)
        {
          upSema(&timerSem);                           //�����ź���
        }
      }
      
      tmpTimer = tmpTimer->nextTimer;
    }
    
  }
  
  exitCritical();
}





void timerTask(void)
{
  void (*fun)(void);
  
  struct TIMER *tmpTimer = 0;
  
  initSema(&timerSem,0);                    //��ʼ���ź���
  
  setTimerCntFun(timerCnt);
  
  while(1)
  {
    downSema(&timerSem);                    //��ȡ�ź���
    
    fun = 0;
    
    enterCritical();                        //�����ٽ���
    if(timerList)                           //�ж�ʱ������
    {
      if(curRunTimer == 0)
      {
        curRunTimer = timerList;
        
        while(curRunTimer)
        {
          if(curRunTimer->tickCnt == curRunTimer->ticks)  //�жϵ���ʱ�����
          {
            curRunTimer->tickCnt = 0;                     //����ֵ��0
            
            if(curRunTimer->timerType == ONE_SHORT_TIMER)
            {
              curRunTimer->state = TIMER_STOP;            //ֹͣ����
            }
            
            fun = curRunTimer->timerFun;
            
            break;
          }
          
          curRunTimer = curRunTimer->nextTimer;
        }
        
      }else
      {
        
        tmpTimer = curRunTimer->nextTimer;
        
        while(tmpTimer)
        {
          if(tmpTimer->tickCnt == tmpTimer->ticks)     //�жϵ���ʱ�����
          {
            tmpTimer->tickCnt = 0;                     //����ֵ��0
            
            if(tmpTimer->timerType == ONE_SHORT_TIMER)
            {
              tmpTimer->state = TIMER_STOP;            //ֹͣ����
            }
            
            fun = tmpTimer->timerFun;
            
            curRunTimer = tmpTimer;
            
            break;
          }
          
          tmpTimer = tmpTimer->nextTimer;
        }
        
        tmpTimer = timerList;
        
        while(tmpTimer && tmpTimer != curRunTimer->nextTimer && fun == 0)
        {
          if(tmpTimer->tickCnt == tmpTimer->ticks)     //�жϵ���ʱ�����
          {
            tmpTimer->tickCnt = 0;                     //����ֵ��0
            
            if(tmpTimer->timerType == ONE_SHORT_TIMER)
            {
              tmpTimer->state = TIMER_STOP;            //ֹͣ����
            }
            
            fun = tmpTimer->timerFun;
            
            curRunTimer = tmpTimer;
            
            break;
          }
          
          tmpTimer = tmpTimer->nextTimer;
        } 
      }
    }
    
    exitCritical();

    if(fun)
    {
      fun();   //ִ�ж�ʱ������
    }
  }
}

#endif