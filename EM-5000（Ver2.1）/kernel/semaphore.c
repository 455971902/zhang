

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


void downSema(struct SEMAPHORE *semaPtr)       //�ź�����
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
      curTskTcbPtr->tskState = BLOCK;     //����BLOCK״̬
      
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
      schdule();                          //���½��е���
      goto enterPlus; 
    }
    exitCritical();
  }
}

void upSema(struct SEMAPHORE *semaPtr)         //�ź�����
{
  OS_TCB *tcb;
  
  if(semaPtr)
  {
    enterCritical();     //�����ٽ���
    
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
      schdule();                      //����ִ�е���
    }
    exitCritical();
  }
}