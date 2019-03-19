

#include "task.h"
#include "stack.h"
#include "port.h"

OS_TCB *curTskTcbPtr;         //��ǰ������ƿ�ָ��
OS_TCB *higPriTskTcbPtr;      //������ȼ�������ƿ�ָ��

OS_TCB *priTskTcbTbl[MAX_PRIOR];   //�����ȼ������������ƿ�ָ���



void     createTask(OS_TCB *tskTcb,void(*tskFun)(void),UINT16 prior,UINT16 stkSize)
{
  if(tskTcb && tskFun && prior < MAX_PRIOR )     //�ж�������ƿ�ָ�룬����ָ���Ƿ�Ϸ�
  {   //������ƿ飬�������Ϸ�
    
    tskTcb->SP = allocTskStk(stkSize);                  //Ϊ�������ջ�ռ�
    
    if(tskTcb->SP)                                      //ջ����ɹ�
    {
      tskTcb->taskFun = tskFun;
      tskTcb->tskState = READY;                         //�������
      tskTcb->prior = prior;                            //�������ȼ�
      tskTcb->waitTick = 0;                             //�ȴ�ʱ��Ϊ0
      tskTcb->eventFlag = 0;                            //�¼���־Ϊ0
      tskTcb->preTCB = 0;
      tskTcb->nextTCB = 0;
      
      initTskStk(&tskTcb->SP,tskFun);                    //��ʼ������
      
      //enterCritical();
      curTskTcbPtr = tskTcb;
      higPriTskTcbPtr = curTskTcbPtr;
      priTskTcbTbl[prior] = curTskTcbPtr;
      //exitCritical();
    }
  }
}






