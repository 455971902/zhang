
/*
file:           porttype.h
date:           2012-05-06
author:         asadcheng

       ���ļ�Ϊ�������Ͷ��������������ϵͳ�ڲ�ʹƽ̨�ϵ���ֲ������ͬʱ������������
   ���ֳ��ȵ����ã�һ��Ĵ������л��õ��������е���������

      unsigned char
      signed   char
      unsigned int
      signed   int
      unsigned long
      signed   long

      һ������£�32bit�Ĵ�������unsigned int,signed intΪ32λ��unsigned long
   signed long�ĳ���Ϊ64λ����16bit��������unsigned int,signed intΪ16λ����
   unsigned long ,signed longΪ32λ���������Ͼ��崦��������   
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
       OSSTKΪ��task��h�ж���������ƿ�ṹ�����������ջ���ͣ������͸��ݲ�ͬ��
   ��������ͬ����ͬ������������ѹջ������1byteʱ��ʹ��unsigned char���ͣ�������
   ����ѹջ������2byteʱ����ʹ��16λ���������ͣ�����������ѹջ������4byteʱ����
   ʹ��32λ����������

    (2)
        OSSR Ϊ��������״̬�Ĵ������ͣ���״̬�Ĵ���Ϊ8λʱ��ʹ��8λ�������ͣ�ͬ
    ���ơ�

*/
typedef  UINT32      OSSTK;           //ջ��������
typedef  UINT16       OSSR;            //״̬�Ĵ�������


#endif