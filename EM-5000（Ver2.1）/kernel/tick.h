
#ifndef _TICK_H_

#define _TICK_H_

#include "task.h"

#include "os_cfg.h"

void osTick(UINT16 arg);                        //系统tick函数

void delayTick(UINT16 arg);                     //延时ms函数

#if OS_TIMER_EN == 1

void setTimerCntFun(void (*timerCnt)(void));

#endif


#endif