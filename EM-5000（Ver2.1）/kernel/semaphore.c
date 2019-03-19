

#include "semaphore.h"

#include "port.h"
#include "schdule.h"



void initSema(struct SEMAPHORE *semaPtr,UINT16 initVal)
{
  if(semaPtr)
  {
    //semaPtr->semList = 0;
    semaPtr->semCnt = initVal;
  }
}


void downSema(struct SEMAPHORE *semaPtr)       //信号量减
{
  OS_TCB *tcb;
  
  if(semaPtr)
  {
    enterCritical();
    
  enterPlus:
    if(semaPtr->semCnt > 0)
    {
      semaPtr->semCnt-- ;
    }else
    {
      curTskTcbPtr->tskState = BLOCK;     //进入BLOCK状态
      
      if(semaPtr->semList)
      {
        tcb = semaPtr->semList;
        
        while(tcb->nextTCB)
        {
          tcb = tcb->nextTCB;
        }
        
        tcb->nextTCB = curTskTcbPtr;
        tcb->nextTCB->nextTCB = 0;
        
      }else
      {
        semaPtr->semList = curTskTcbPtr;
      }
      schdule();                          //重新进行调度
      goto enterPlus; 
    }
    exitCritical();
  }
}

void upSema(struct SEMAPHORE *semaPtr)         //信号量加
{
  OS_TCB *tcb;
  
  if(semaPtr)
  {
    enterCritical();     //进入临界区
    
    semaPtr->semCnt++ ;
    
    if(semaPtr->semList)
    {
      tcb = semaPtr->semList;
      
      if(tcb->nextTCB)
      {
        semaPtr->semList = tcb->nextTCB;
      }else
      {
        semaPtr->semList = 0;
      }
      tcb->nextTCB = 0;
      
      tcb->tskState = READY;
      schdule();                      //重新执行调度
    }
    exitCritical();
  }
}