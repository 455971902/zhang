
#include "os.h"

#include "task.h"

#include "timer.h"

#include "port.h"

static OS_TCB  idelTcb;


#if OS_TIMER_EN
static OS_TCB  timerTskTcb;
#endif

void idelTask(void)                      //idelTask
{
  while(1)
  {
    
  }
}


void osStart(void)                        //ϵͳ��������
{
  curTskTcbPtr = priTskTcbTbl[MAX_PRIOR - 1];
  higPriTskTcbPtr = curTskTcbPtr;
  startCurrentTsk();                         //������ǰ������
}

void osInit(void)                         //ϵͳ��ʼ������
{
  createTask(&idelTcb,idelTask,MAX_PRIOR -1,128);
  
#if OS_TIMER_EN == 1
  
  createTask(&timerTskTcb,timerTask,0,128);
#endif
  
}