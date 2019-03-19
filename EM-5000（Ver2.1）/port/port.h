

#ifndef  _PORT_H_

#define  _PORT_H_

#include "porttype.h"
#include "MSP430x54x.h"
#include "task.h"

#define enterCritical()     do { _DINT(); }while(0)
#define exitCritical()      do { _EINT(); }while(0)

#define swContex()          do { swCtx(); _NOP();           }while(0)

#define runIsrFun(x,y,z)    do { runIsrPort(x,y,z); _NOP(); }while(0)

#define startCurrentTsk()   do { startCurTask(); _NOP();    }while(0)

#define intCtxSave()        do { ctxSave();    _NOP();      }while(0)
#define intCtxRest()        do { ctxRestore();  _NOP();     }while(0)

#define disableInt()        do { _DINT(); _NOP(); }while(0)
#define enableInt()         do { _EINT(); _NOP(); }while(0)


extern  UINT32 isrStk[128];

volatile EXTERN   void swCtx(void);                            //上下文切换操作函数


volatile EXTERN   void runIsrPort(void(*tskFun)(UINT16),UINT16 *intNestCnt,UINT16 arg,void *getHighPriFun);           //执行中断处理接口函数


volatile EXTERN   void startCurTask(void);                     //当前任务启动函数

volatile EXTERN   void ctxSave(void);                          //保存上下文


volatile EXTERN   void ctxRestore(void);                      //恢复上下文

extern    void tskCli(void);                           //任务级的关中断函数
extern    void tskCliEx(void);

extern    void tskSti(void);                           //任务级的开中断函数
void tskStiEx(void);

extern    UINT16 getSR(void);
extern    void   setSR(UINT16 SR);



//EXTERN   void intCli();                              //中断级的关中断函数
//EXTERN   void intSti();                              //中断级的开中断函数


volatile void initTskStk(OSSTK **tskSP,void(*tskFun)(void));                       //初始化任务的栈函数



#endif