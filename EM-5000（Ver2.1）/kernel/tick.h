
#ifndef _TICK_H_

#define _TICK_H_

#include "task.h"

#include "os_cfg.h"

void osTick(UINT16 arg);                        //ϵͳtick����

void delayTick(UINT16 arg);                     //��ʱms����

#if OS_TIMER_EN == 1

void setTimerCntFun(void (*timerCnt)(void));

#endif


#endif