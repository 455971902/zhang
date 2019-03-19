

#ifndef  _SEMAPHORE_H_

#define  _SEMAPHORE_H_


#include "task.h"


struct SEMAPHORE
{
  OS_TCB *semList;        //等待本信号量的任务队列
  UINT16 semCnt;          //信号量计数值
};


void initSema(struct SEMAPHORE *semaPtr,UINT16 initVal);

void downSema(struct SEMAPHORE *semaPtr);      //信号量减

void upSema(struct SEMAPHORE *semaPtr);        //信号量加





#endif