

#include "port.h"
#include "porttype.h"

UINT32 isrStk[128];
UINT32 *isrStrPtr = &isrStk[127];

volatile void initTskStk(OSSTK **tskSP,void(*tskFun)(void))                        //初始化任务的栈函数
{
  OSSTK *sp;
  
  sp = *tskSP;
  
  if(sp)
  {
    *((UINT32*)sp) = (UINT32)tskFun;
    
    sp --;
    *((UINT32*)sp) = 0x0008;
    
    sp --;
    *((UINT32*)sp) = 0x0404;
    
    sp --;
    *((UINT32*)sp) = 0x0505;
    
    sp --;
    *((UINT32*)sp) = 0x0606;
    
    sp --;
    *((UINT32*)sp) = 0x0707;
    
    sp --;
    *((UINT32*)sp) = 0x0808;
    
    sp --;
    *((UINT32*)sp) = 0x0909;
    
    sp --;
    *((UINT32*)sp) = 0x1010;
    
    sp --;
    *((UINT32*)sp) = 0x1111;
    
    sp --;
    *((UINT32*)sp) = 0x1212;
    
    sp --;
    *((UINT32*)sp) = 0x1313;
    
    sp --;
    *((UINT32*)sp) = 0x1414;
    
    sp --;
    *((UINT32*)sp) = 0x1515;
    
    *tskSP = sp;
  }
}




