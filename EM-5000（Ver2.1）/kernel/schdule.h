

#ifndef  _SCHDULE_H_
#define  _SCHDULE_H_

#include "task.h"
#include "porttype.h"

void schdule(void);                       //任务调度函数

OS_TCB*  getHighPriorTsk(void);                 //获取最高任务优先级函数


#endif