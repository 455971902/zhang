
#include "tick.h"

#include "port.h"

#include "task.h"

#include "schdule.h"

#include "isr.h"

#include "os_cfg.h"


#include "msp430x54x.h"


EXTERN OS_TCB *curTskTcbPtr;         //当前任务控制块指针
EXTERN OS_TCB *higPriTskTcbPtr;      //最高优先级任务控制块指针

EXTERN OS_TCB *priTskTcbTbl[MAX_PRIOR];   //按优先级排序的任务控制块指针表


#if OS_TIMER_EN == 1

void (*timerCntFun)(void) = 0;

#endif


void osTick(UINT16 arg)                         //系统tick函数
{
  OS_TCB *tcb;
  UINT16 i;
  
  enterCritical();
  
  for(i = 0; i < MAX_PRIOR; i++)
  {
    tcb = priTskTcbTbl[i];
    
    if(tcb && (tcb->tskState == WAITE || tcb->tskState == BLOCK_WAITE))
    {
      tcb->waitTick --;
      
      if(tcb->waitTick == 0)
      {
        tcb->tskState = READY;
      }
    }
  }
  
  exitCritical();

#if  OS_TIMER_EN == 1
  
  if(timerCntFun)
  {
    timerCntFun();
  }
  
#endif
  
}



#if OS_TIMER_EN == 1

void setTimerCntFun(void (*timerCnt)(void))
{
  enterCritical();
  
  if(timerCnt)
  {
    timerCntFun = timerCnt;
  }
  exitCritical();
}

#endif




void delayTick(UINT16 arg)                        //延时ms函数
{
  enterCritical();                              //进入临界区
  
  curTskTcbPtr->waitTick = arg;                 //设置延时时间
  curTskTcbPtr->tskState = WAITE;               //当前任务设置为WAIT状态
  schdule();                                    //重新执行调度
  exitCritical();                               //离开临界区
}


