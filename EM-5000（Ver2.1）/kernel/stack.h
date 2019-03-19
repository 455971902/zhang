

#ifndef _STACK_H_

#define _STACK_H_

#include "porttype.h"

#define STK_TOTAL_SIZE            1280      //总任务栈的大小

#define STK_MIN_SIZE              64       //每个任务的最小大小




#define    STK_HIGH_LOW          
//#define    STK_LOW_HIGH



OSSTK *allocTskStk(UINT16 stkSize);                   //为任务分配栈函数







#endif