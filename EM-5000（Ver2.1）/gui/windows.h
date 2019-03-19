
////////////////////////////////////////////////////////////////////////
//
//       windows.h
//       zuozhe: Asad
//       date:2010.09.20
//
//     本文件主要定义了menu菜单结构体，窗口结构体，定义了其基本的默认
//  处理函数，用于连接按键处理句柄的函数，新建菜单是用于初始化菜单的函数
//  菜单与菜单之间连接的函数。
//
////////////////////////////////////////////////////////////////////////
#ifndef _WINDOWS_H_

#define _WINDOWS_H_

#include "system.h"

/////////////////////////////////////////////////////////////////////////////

typedef unsigned int KEY;

/*

/////////////////////////////////////////////////////////////////////////////

#define UPKEY             4           //向上键
#define DOWNKEY           2           //向下键
#define RIGHTKEY          8           //右键
#define LEFTKEY           16          //左键
#define CENTERKEY         1           //中心键

/////////////////////////////////////////////////////////////////////////////

*/
typedef struct sysmenu   //菜单结构体
{
  struct sysmenu *top_menu;      //上层菜单指针
  struct sysmenu *sub_menu;      //下层子菜单指针
  struct sysmenu *right;         //右伙伴菜单指针
  struct sysmenu *left;          //左伙伴菜单指针
  
  //unsigned int ID;               //菜单的ID号，用于标识菜单
  const char *text;                    //菜单的文本
  const char *text_chn;       //菜单中文显示数据指针
  const unsigned char *icn;      //菜单图标数据数据指针
  unsigned char x_pos;           //菜单图标的x位置
  unsigned char y_pos;           //菜单图标的y位置
  unsigned char status1;         //用于菜单处理时的辅助量
  int status2;                   //菜单设置时辅助量2 
  void (*onUp)();                //向上键按下处理函数
  void (*onDown)();              //向下键按下处理函数
  void (*onLeft)();              //左键按下处理函数
  void (*onRight)();             //右键按下处理函数
  void (*onCenter)();            //中心键按下处理函数
  void (*onClk)();               //软时钟信号处理函数
  void (*show)();                //显示函数  
}MENU,*LpMENU;

typedef struct window   //窗口结构体
{
  MENU *curfous;                      //当前焦点菜单指针
  MENU *menu;                         //菜单指针
  unsigned char enter_menu;           //用于判断窗口是否进入菜单操作
  void (*messagefun)(unsigned int);   //消息处理函数
  void (*onCenter)();                 //中心键按下处理函数
  void (*onUp)();                     //向上键按下处理函数
  void (*onDown)();                   //向下键按下处理函数
  void (*onRight)();                  //右键按下处理函数
  void (*onLeft)();                   //左键按下处理函数
  void (*onClk)();                    //软时钟信号处理函数
  
  void (*show)();                     //显示函数

  SYSTEM* system;                       //系统指针，用于对系统需要处理的量进行操作
  
  unsigned char    curpos;            //用于在组菜单设置的时候记录焦点的位置
  
  unsigned char    set_g_curpos;      //用于在组里面设置时间等各种量的焦点位置
  
}WINDOW,*LpWINDOW;

//////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////

//WINDOW* new_window(void* system);     //创建窗口函数，system为系统结构体指针

void  init_menu(MENU* menu);                //初始化一个菜单

void  add_sub_menu(MENU* menu,MENU* sub_menu);
//使sub_menu的上层菜单为menu
//如果sub_menu是menu的第一个子菜单，则menu的下层菜单是sub_menu
//如果sub_menu不是menu的第一个子菜单，则sub_menu将被链接到menu的最后一个菜单，
//并且menu所有的下第一层菜单将形成一个循环链表


void connect_menu(MENU *l_menu,MENU* r_menu);
// 此函数将r_menu和l_menu两个菜单连在一起，r_menu的左伙伴是l_menu,
// l_menu的右伙伴是r_menu。


void connect_keyfun(MENU* menu,void (*keyfun)(),KEY keytype);
// 将按键处理函数keyfun链接到menu的keytype函数处理指针


void connect_showfun(MENU* menu,void (*showfun)());
// 将显示函数showfun链接到menu的show指针


void connect_clkfun(MENU* menu,void (*clkfun)());
//将时钟处理函数clkfun链接到menu的onClk指针


/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
void default_messfun(unsigned int key);         
                                            //默认的消息处理函数，为空函数
                                            //key为接收键盘键值


void default_onUp(void);                    //默认的上键处理函数，为空函数
void default_onDown(void);                  //默认的下键处理函数，为空函数
void default_onRight(void);                 //默认的右键处理函数，为空函数
void default_onLeft(void);                  //默认的左键处理函数，为空函数
void default_onCenter(void);                //默认的中心键处理函数，为空函数
void default_onClk(void);                   //默认的软时钟处理函数，为空函数
void default_show(void);                    //默认的显示函数，为空函数
//////////////////////////////////////////////////////////////////////////////

#endif