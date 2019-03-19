
////////////////////////////////////////////////////////////////////////
//
//       windows.h
//       zuozhe: Asad
//       date:2010.09.20
//
//     ���ļ���Ҫ������menu�˵��ṹ�壬���ڽṹ�壬�������������Ĭ��
//  ���������������Ӱ����������ĺ������½��˵������ڳ�ʼ���˵��ĺ���
//  �˵���˵�֮�����ӵĺ�����
//
////////////////////////////////////////////////////////////////////////
#ifndef _WINDOWS_H_

#define _WINDOWS_H_

#include "system.h"

/////////////////////////////////////////////////////////////////////////////

typedef unsigned int KEY;

/*

/////////////////////////////////////////////////////////////////////////////

#define UPKEY             4           //���ϼ�
#define DOWNKEY           2           //���¼�
#define RIGHTKEY          8           //�Ҽ�
#define LEFTKEY           16          //���
#define CENTERKEY         1           //���ļ�

/////////////////////////////////////////////////////////////////////////////

*/
typedef struct sysmenu   //�˵��ṹ��
{
  struct sysmenu *top_menu;      //�ϲ�˵�ָ��
  struct sysmenu *sub_menu;      //�²��Ӳ˵�ָ��
  struct sysmenu *right;         //�һ��˵�ָ��
  struct sysmenu *left;          //����˵�ָ��
  
  //unsigned int ID;               //�˵���ID�ţ����ڱ�ʶ�˵�
  const char *text;                    //�˵����ı�
  const char *text_chn;       //�˵�������ʾ����ָ��
  const unsigned char *icn;      //�˵�ͼ����������ָ��
  unsigned char x_pos;           //�˵�ͼ���xλ��
  unsigned char y_pos;           //�˵�ͼ���yλ��
  unsigned char status1;         //���ڲ˵�����ʱ�ĸ�����
  int status2;                   //�˵�����ʱ������2 
  void (*onUp)();                //���ϼ����´�����
  void (*onDown)();              //���¼����´�����
  void (*onLeft)();              //������´�����
  void (*onRight)();             //�Ҽ����´�����
  void (*onCenter)();            //���ļ����´�����
  void (*onClk)();               //��ʱ���źŴ�����
  void (*show)();                //��ʾ����  
}MENU,*LpMENU;

typedef struct window   //���ڽṹ��
{
  MENU *curfous;                      //��ǰ����˵�ָ��
  MENU *menu;                         //�˵�ָ��
  unsigned char enter_menu;           //�����жϴ����Ƿ����˵�����
  void (*messagefun)(unsigned int);   //��Ϣ������
  void (*onCenter)();                 //���ļ����´�����
  void (*onUp)();                     //���ϼ����´�����
  void (*onDown)();                   //���¼����´�����
  void (*onRight)();                  //�Ҽ����´�����
  void (*onLeft)();                   //������´�����
  void (*onClk)();                    //��ʱ���źŴ�����
  
  void (*show)();                     //��ʾ����

  SYSTEM* system;                       //ϵͳָ�룬���ڶ�ϵͳ��Ҫ����������в���
  
  unsigned char    curpos;            //��������˵����õ�ʱ���¼�����λ��
  
  unsigned char    set_g_curpos;      //����������������ʱ��ȸ������Ľ���λ��
  
}WINDOW,*LpWINDOW;

//////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////

//WINDOW* new_window(void* system);     //�������ں�����systemΪϵͳ�ṹ��ָ��

void  init_menu(MENU* menu);                //��ʼ��һ���˵�

void  add_sub_menu(MENU* menu,MENU* sub_menu);
//ʹsub_menu���ϲ�˵�Ϊmenu
//���sub_menu��menu�ĵ�һ���Ӳ˵�����menu���²�˵���sub_menu
//���sub_menu����menu�ĵ�һ���Ӳ˵�����sub_menu�������ӵ�menu�����һ���˵���
//����menu���е��µ�һ��˵����γ�һ��ѭ������


void connect_menu(MENU *l_menu,MENU* r_menu);
// �˺�����r_menu��l_menu�����˵�����һ��r_menu��������l_menu,
// l_menu���һ����r_menu��


void connect_keyfun(MENU* menu,void (*keyfun)(),KEY keytype);
// ������������keyfun���ӵ�menu��keytype��������ָ��


void connect_showfun(MENU* menu,void (*showfun)());
// ����ʾ����showfun���ӵ�menu��showָ��


void connect_clkfun(MENU* menu,void (*clkfun)());
//��ʱ�Ӵ�����clkfun���ӵ�menu��onClkָ��


/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
void default_messfun(unsigned int key);         
                                            //Ĭ�ϵ���Ϣ��������Ϊ�պ���
                                            //keyΪ���ռ��̼�ֵ


void default_onUp(void);                    //Ĭ�ϵ��ϼ���������Ϊ�պ���
void default_onDown(void);                  //Ĭ�ϵ��¼���������Ϊ�պ���
void default_onRight(void);                 //Ĭ�ϵ��Ҽ���������Ϊ�պ���
void default_onLeft(void);                  //Ĭ�ϵ������������Ϊ�պ���
void default_onCenter(void);                //Ĭ�ϵ����ļ���������Ϊ�պ���
void default_onClk(void);                   //Ĭ�ϵ���ʱ�Ӵ�������Ϊ�պ���
void default_show(void);                    //Ĭ�ϵ���ʾ������Ϊ�պ���
//////////////////////////////////////////////////////////////////////////////

#endif