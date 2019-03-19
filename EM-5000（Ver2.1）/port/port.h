

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

volatile EXTERN   void swCtx(void);                            //�������л���������


volatile EXTERN   void runIsrPort(void(*tskFun)(UINT16),UINT16 *intNestCnt,UINT16 arg,void *getHighPriFun);           //ִ���жϴ���ӿں���


volatile EXTERN   void startCurTask(void);                     //��ǰ������������

volatile EXTERN   void ctxSave(void);                          //����������


volatile EXTERN   void ctxRestore(void);                      //�ָ�������

extern    void tskCli(void);                           //���񼶵Ĺ��жϺ���
extern    void tskCliEx(void);

extern    void tskSti(void);                           //���񼶵Ŀ��жϺ���
void tskStiEx(void);

extern    UINT16 getSR(void);
extern    void   setSR(UINT16 SR);



//EXTERN   void intCli();                              //�жϼ��Ĺ��жϺ���
//EXTERN   void intSti();                              //�жϼ��Ŀ��жϺ���


volatile void initTskStk(OSSTK **tskSP,void(*tskFun)(void));                       //��ʼ�������ջ����



#endif