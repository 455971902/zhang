
#include "isr.h"

#include "port.h"

#include "schdule.h"

UINT16 intNest = 0;

EXTERN OS_TCB *curTskTcbPtr;         //��ǰ������ƿ�ָ��
EXTERN OS_TCB *higPriTskTcbPtr;      //������ȼ�������ƿ�ָ��


/*
      ִ���жϴ�����API
*/

void runIsr(void(*isrFun)(UINT16),UINT16 arg)
{
  if(isrFun)
  {
    if(intNest == 0)
    {
      intNest ++;
      runIsrPort(isrFun,&intNest,arg,(void*)getHighPriorTsk);                 //��ȡ����������ȼ�����);
    }else if(intNest < 5)
    {
      intNest ++;
      enableInt();            ///�����ж�
      isrFun(arg);
      disableInt();           //���ж�
      intNest --;
    }else
    {
      _NOP();
    }
  }
}