

#ifndef  _SEMAPHORE_H_

#define  _SEMAPHORE_H_


#include "task.h"


struct SEMAPHORE
{
  OS_TCB *semList;        //�ȴ����ź������������
  UINT16 semCnt;          //�ź�������ֵ
};


void initSema(struct SEMAPHORE *semaPtr,UINT16 initVal);

void downSema(struct SEMAPHORE *semaPtr);      //�ź�����

void upSema(struct SEMAPHORE *semaPtr);        //�ź�����





#endif