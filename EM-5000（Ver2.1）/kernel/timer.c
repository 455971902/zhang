

#include "timer.h"

#include "semaphore.h"

#include "port.h"

#include "tick.h"

#if  OS_TIMER_EN == 1

static struct TIMER *timerList = 0;          //定时器列表头指针

static struct TIMER *curRunTimer = 0;        //当前被执行的TIMER，防止同一个TIMER的句柄被自我嵌套

static struct SEMAPHORE timerSem;            //定时器用信号量

void setupTimer(struct TIMER *timer,void(*timerFun)(void),UINT16 type,UINT16 ticks)
{
  struct TIMER *tmpTimer = 0;
  
  if(timer && timerFun)   //定时器合法并且定时器函数合法
  {
    
    if(ticks == 0)
    {
      ticks = 10;
    }
    
    timer->timerFun = timerFun;        //设置定时器句柄
    timer->timerType = type;           //设置定时器类型
    timer->tickCnt = 0;                //初始计数值
    timer->ticks = ticks;              //最大计数值
    timer->state = 0;                  //定时器未启动
    timer->nextTimer = 0;
    
    enterCritical();                   //进入临界区
    
    if(timerList == 0)
    {
      timerList = timer;               //此定时器为首定时器
    }else
    {
      tmpTimer = timerList;
      while(tmpTimer->nextTimer)
      {
        tmpTimer = tmpTimer->nextTimer;
      }
      
      tmpTimer->nextTimer = timer;
    }

    exitCritical();                    //退出临界区
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
    enterCritical();                 //进入临界区
    
    timer->state = TIMER_START;      //启动定时器
    
    exitCritical();                  //退出临界区
  }
  
}



void timerCnt(void)     //定时器计数
{
  struct TIMER *tmpTimer;
  
  enterCritical();
  
  if(timerList)
  {
    tmpTimer = timerList;
    
    while(tmpTimer)
    {
      if(tmpTimer->state == TIMER_START && tmpTimer->tickCnt < tmpTimer->ticks)   //被开启的定时器
      {
        tmpTimer->tickCnt ++;
        
        if(tmpTimer->tickCnt == tmpTimer->ticks)
        {
          upSema(&timerSem);                           //发送信号量
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
  
  initSema(&timerSem,0);                    //初始化信号量
  
  setTimerCntFun(timerCnt);
  
  while(1)
  {
    downSema(&timerSem);                    //获取信号量
    
    fun = 0;
    
    enterCritical();                        //进入临界区
    if(timerList)                           //有定时器存在
    {
      if(curRunTimer == 0)
      {
        curRunTimer = timerList;
        
        while(curRunTimer)
        {
          if(curRunTimer->tickCnt == curRunTimer->ticks)  //判断到定时器溢出
          {
            curRunTimer->tickCnt = 0;                     //计数值清0
            
            if(curRunTimer->timerType == ONE_SHORT_TIMER)
            {
              curRunTimer->state = TIMER_STOP;            //停止计数
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
          if(tmpTimer->tickCnt == tmpTimer->ticks)     //判断到定时器溢出
          {
            tmpTimer->tickCnt = 0;                     //计数值清0
            
            if(tmpTimer->timerType == ONE_SHORT_TIMER)
            {
              tmpTimer->state = TIMER_STOP;            //停止计数
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
          if(tmpTimer->tickCnt == tmpTimer->ticks)     //判断到定时器溢出
          {
            tmpTimer->tickCnt = 0;                     //计数值清0
            
            if(tmpTimer->timerType == ONE_SHORT_TIMER)
            {
              tmpTimer->state = TIMER_STOP;            //停止计数
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
      fun();   //执行定时器函数
    }
  }
}

#endif