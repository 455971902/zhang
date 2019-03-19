


#ifndef _TASK_H_

#define _TASK_H_


#include "porttype.h"




//  MAX_PRIO ��ʾ�����������ȼ�
#define  MAX_PRIOR                     16


/*
   һ�µĺ궨���ʾ����ļ���״̬
*/
#define      RUNNING      0            //������������
#define      READY        1            //�������
#define      WAITE        2            //�����ڵȴ�ʱ��
#define      BLOCK        3            //��������
#define      BLOCK_WAITE  4            //�����������ȴ���ʱ



struct OSTCB          //������ƿ�ṹ��
{
  OSSTK *SP;                          //�����ջָ�룬����Ա��ֵ�������ջ��RAM�еĵ�ַ
  void (*taskFun)(void);             //����������
  
  UINT16 tskState;                    //����״̬
  
  UINT32 waitTick;                    //������Ҫ�ȴ���tick��
  
  UINT16 prior;                       //�������ȼ�
  
  UINT16 eventFlag;                    //�����¼���־
  
  struct OSTCB  *preTCB;              //ǰһ�������TCB
  struct OSTCB  *nextTCB;             //��һ�������TCB
};


typedef  struct OSTCB                 OS_TCB;


EXTERN OS_TCB *curTskTcbPtr;         //��ǰ������ƿ�ָ��
EXTERN OS_TCB *higPriTskTcbPtr;      //������ȼ�������ƿ�ָ��

EXTERN OS_TCB *priTskTcbTbl[MAX_PRIOR];   //�����ȼ������������ƿ�ָ���



/*
    ����createTask����,���������ڽ���һ�����񣬵�1�������������������ƿ飬
��2�������������ִ�к�������3����������������ȼ�����4�������������ջ��С
*/
void     createTask(OS_TCB *tskTcb,void(*tskFun)(void),UINT16 prior,UINT16 stkSize);




#endif