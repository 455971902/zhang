

#ifndef  _ISR_H_
#define  _ISR_H_

#include "porttype.h"


EXTERN UINT16 intNest;

/*
      ִ���жϴ�����API
*/
void runIsr(void(*isrFun)(UINT16),UINT16 arg);




#endif