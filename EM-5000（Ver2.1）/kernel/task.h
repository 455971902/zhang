


#ifndef _TASK_H_

#define _TASK_H_


#include "porttype.h"




//  MAX_PRIO 表示任务的最高优先级
#define  MAX_PRIOR                     16


/*
   一下的宏定义表示任务的几种状态
*/
#define      RUNNING      0            //任务正在运行
#define      READY        1            //任务就绪
#define      WAITE        2            //任务在等待时间
#define      BLOCK        3            //任务被阻塞
#define      BLOCK_WAITE  4            //任务阻塞并等待超时



struct OSTCB          //任务控制块结构体
{
  OSSTK *SP;                          //任务的栈指针，本成员的值是任务的栈在RAM中的地址
  void (*taskFun)(void);             //任务主函数
  
  UINT16 tskState;                    //任务状态
  
  UINT32 waitTick;                    //任务需要等待的tick数
  
  UINT16 prior;                       //任务优先级
  
  UINT16 eventFlag;                    //任务事件标志
  
  struct OSTCB  *preTCB;              //前一个任务的TCB
  struct OSTCB  *nextTCB;             //后一个任务的TCB
};


typedef  struct OSTCB                 OS_TCB;


EXTERN OS_TCB *curTskTcbPtr;         //当前任务控制块指针
EXTERN OS_TCB *higPriTskTcbPtr;      //最高优先级任务控制块指针

EXTERN OS_TCB *priTskTcbTbl[MAX_PRIOR];   //按优先级排序的任务控制块指针表



/*
    声明createTask函数,本函数用于建立一个任务，第1个参数是任务的任务控制块，
第2个参数是任务的执行函数，第3个参数是任务的优先级，第4个函数是任务的栈大小
*/
void     createTask(OS_TCB *tskTcb,void(*tskFun)(void),UINT16 prior,UINT16 stkSize);




#endif