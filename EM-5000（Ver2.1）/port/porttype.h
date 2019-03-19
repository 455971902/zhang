
/*
file:           porttype.h
date:           2012-05-06
author:         asadcheng

       本文件为数据类型定义别名，以利于系统在不痛平台上的移植操作，同时减短数据类型
   名字长度的作用，一般的处理器中会用到以下所列的数据类型

      unsigned char
      signed   char
      unsigned int
      signed   int
      unsigned long
      signed   long

      一边情况下，32bit的处理器的unsigned int,signed int为32位，unsigned long
   signed long的长度为64位。而16bit处理器中unsigned int,signed int为16位，而
   unsigned long ,signed long为32位，具体请结合具体处理器而定   
*/



#ifndef _PORT_TYPE_H_
#define _PORT_TYPE_H_



typedef  unsigned char      UCHAR8;
typedef  signed   char      CHAR8;
typedef  unsigned int       UINT16;
typedef  signed   int       INT16;
typedef  unsigned long      UINT32;
typedef  signed   long      INT32;

#define EXTERN  extern


/*
    (1)  
       OSSTK为在task。h中定义任务控制块结构体里面的任务栈类型，此类型根据不同的
   处理器不同而不同，当处理器的压栈操作是1byte时请使用unsigned char类型，当处理
   器的压栈操作是2byte时，请使用16位的数据类型，当处理器的压栈操作是4byte时，请
   使用32位的数据类型

    (2)
        OSSR 为处理器的状态寄存器类型，当状态寄存器为8位时请使用8位数据类型，同
    类推。

*/
typedef  UINT32      OSSTK;           //栈数据类型
typedef  UINT16       OSSR;            //状态寄存器类型


#endif