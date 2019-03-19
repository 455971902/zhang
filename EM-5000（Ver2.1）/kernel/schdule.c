
#include "schdule.h"
#include "port.h"
#include "task.h"
#include "isr.h"


EXTERN OS_TCB *curTskTcbPtr;         //��ǰ������ƿ�ָ��
EXTERN OS_TCB *higPriTskTcbPtr;      //������ȼ�������ƿ�ָ��

EXTERN OS_TCB *priTskTcbTbl[MAX_PRIOR];   //�����ȼ������������ƿ�ָ���


void schdule(void)                        //������Ⱥ���
{
  higPriTskTcbPtr = getHighPriorTsk();
  
  if(higPriTskTcbPtr != curTskTcbPtr && intNest == 0)
  {
    swContex();                  //ִ�������Ĳ���
  }
  
  higPriTskTcbPtr = curTskTcbPtr;
}


OS_TCB* getHighPriorTsk(void)           //��ȡ����������ȼ�����
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




