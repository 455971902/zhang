
#include "schdule.h"
#include "port.h"
#include "task.h"
#include "isr.h"


EXTERN OS_TCB *curTskTcbPtr;         //当前任务控制块指针
EXTERN OS_TCB *higPriTskTcbPtr;      //最高优先级任务控制块指针

EXTERN OS_TCB *priTskTcbTbl[MAX_PRIOR];   //按优先级排序的任务控制块指针表


void schdule(void)                        //任务调度函数
{
  higPriTskTcbPtr = getHighPriorTsk();
  
  if(higPriTskTcbPtr != curTskTcbPtr && intNest == 0)
  {
    swContex();                  //执行上下文操作
  }
  
  higPriTskTcbPtr = curTskTcbPtr;
}


OS_TCB* getHighPriorTsk(void)           //获取最高任务优先级函数
{
  int i;
  OS_TCB *osTcb = 0;
  
  for(i = 0; i < MAX_PRIOR; i++)
  {
    if(priTskTcbTbl[i])
    {
      if(priTskTcbTbl[i]->tskState == READY)
      {
        osTcb = priTskTcbTbl[i];
        break;
      }
    }
  }
  
  return osTcb;
}




