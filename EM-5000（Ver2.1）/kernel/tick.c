
#include "tick.h"

#include "port.h"

#include "task.h"

#include "schdule.h"

#include "isr.h"

#include "os_cfg.h"


#include "msp430x54x.h"


EXTERN OS_TCB *curTskTcbPtr;         //��ǰ������ƿ�ָ��
EXTERN OS_TCB *higPriTskTcbPtr;      //������ȼ�������ƿ�ָ��

EXTERN OS_TCB *priTskTcbTbl[MAX_PRIOR];   //�����ȼ������������ƿ�ָ���


#if OS_TIMER_EN == 1

void (*timerCntFun)(void) = 0;

#endif


void osTick(UINT16 arg)                         //ϵͳtick����
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




void delayTick(UINT16 arg)                        //��ʱms����
{
  enterCritical();                              //�����ٽ���
  
  curTskTcbPtr->waitTick = arg;                 //������ʱʱ��
  curTskTcbPtr->tskState = WAITE;               //��ǰ��������ΪWAIT״̬
  schdule();                                    //����ִ�е���
  exitCritical();                               //�뿪�ٽ���
}


