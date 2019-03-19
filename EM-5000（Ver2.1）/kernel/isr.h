

#ifndef  _ISR_H_
#define  _ISR_H_

#include "porttype.h"


EXTERN UINT16 intNest;

/*
      执行中断处理函数API
*/
void runIsr(void(*isrFun)(UINT16),UINT16 arg);




#endif