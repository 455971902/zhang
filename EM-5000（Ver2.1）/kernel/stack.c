

#include "stack.h"
#include "porttype.h"


#ifdef STK_HIGH_LOW
static INT32 length = STK_TOTAL_SIZE;
#endif

#ifdef STK_LOW_HIGH
static INT32 length = 0;
#endif

#pragma pack(4)
__no_init OSSTK  tskStack[STK_TOTAL_SIZE];


OSSTK *allocTskStk(UINT16 stkSize)                    //为任务分配栈函数
{
  OSSTK *reStkPtr = 0;
  
  if(stkSize < STK_MIN_SIZE)
  {
    stkSize = STK_MIN_SIZE;
  }

#ifdef STK_HIGH_LOW
if( length - stkSize >= 0)
  {
    reStkPtr = &tskStack[length - 1];
    length -= stkSize;
  }
#endif
    
#ifdef STK_LOW_HIGH
  if(length + stkSize <= STK_TOTAL_SIZE)
  {
    reStkPtr = &tskStack[length];
    length += stkSize;
  }
#endif
  
  return reStkPtr;
}