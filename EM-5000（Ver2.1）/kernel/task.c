

#include "task.h"
#include "stack.h"
#include "port.h"

OS_TCB *curTskTcbPtr;         //当前任务控制块指针
OS_TCB *higPriTskTcbPtr;      //最高优先级任务控制块指针

OS_TCB *priTskTcbTbl[MAX_PRIOR];   //按优先级排序的任务控制块指针表



void     createTask(OS_TCB *tskTcb,void(*tskFun)(void),UINT16 prior,UINT16 stkSize)
{
  if(tskTcb && tskFun && prior < MAX_PRIOR )     //判断任务控制块指针，函数指针是否合法
  {   //任务控制块，任务函数合法
    
    tskTcb->SP = allocTskStk(stkSize);                  //为任务分配栈空间
    
    if(tskTcb->SP)                                      //栈分配成功
    {
      tskTcb->taskFun = tskFun;
      tskTcb->tskState = READY;                         //任务就绪
      tskTcb->prior = prior;                            //设置优先级
      tskTcb->waitTick = 0;                             //等待时间为0
      tskTcb->eventFlag = 0;                            //事件标志为0
      tskTcb->preTCB = 0;
      tskTcb->nextTCB = 0;
      
      initTskStk(&tskTcb->SP,tskFun);                    //初始化任务
      
      //enterCritical();
      curTskTcbPtr = tskTcb;
      higPriTskTcbPtr = curTskTcbPtr;
      priTskTcbTbl[prior] = curTskTcbPtr;
      //exitCritical();
    }
  }
}






