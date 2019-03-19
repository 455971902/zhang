
#include "isr.h"

#include "port.h"

#include "schdule.h"

UINT16 intNest = 0;

EXTERN OS_TCB *curTskTcbPtr;         //当前任务控制块指针
EXTERN OS_TCB *higPriTskTcbPtr;      //最高优先级任务控制块指针


/*
      执行中断处理函数API
*/

void runIsr(void(*isrFun)(UINT16),UINT16 arg)
{
  if(isrFun)
  {
    if(intNest == 0)
    {
      intNest ++;
      runIsrPort(isrFun,&intNest,arg,(void*)getHighPriorTsk);                 //获取最高任务优先级函数);
    }else if(intNest < 5)
    {
      intNest ++;
      enableInt();            ///允许中断
      isrFun(arg);
      disableInt();           //关中断
      intNest --;
    }else
    {
      _NOP();
    }
  }
}