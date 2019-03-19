


#include "windows.h"

#include "key.h"



//////////////////////////////////////////////////////////////////////////////


void  init_menu(MENU* menu)                 //创建一个菜单，返回值为菜单的指针
{
  
  menu->top_menu=0;
  menu->sub_menu=0;
  menu->right=0;
  menu->left=0;
  
  menu->text=0;
  menu->text_chn=0;
  menu->icn=0;
  //menu->ID=0;
  menu->x_pos=0;
  menu->y_pos=0;
  menu->status1=0;
  menu->status2=0;
    
  menu->onUp=default_onUp;
  menu->onDown=default_onDown;
  menu->onRight=default_onRight;
  menu->onLeft=default_onLeft;
  menu->onCenter=default_onCenter;
  menu->onClk=default_onClk;
  menu->show=default_show;
  
}



void  add_sub_menu(MENU* menu,MENU* sub_menu)
{
  MENU* lp;
  
  if(menu!=0 && sub_menu!=0)
  {
      if(menu->sub_menu==0)
      {
        menu->sub_menu=sub_menu;
        sub_menu->top_menu=menu;
      }else
      {
        lp=menu->sub_menu;
        if(lp->right==0)
        {
          sub_menu->top_menu=menu;
      
          lp->right=sub_menu;
          lp->left=sub_menu;
      
          sub_menu->right=lp;
          sub_menu->left=lp;
        }else
        {
          sub_menu->top_menu=menu;
      
          sub_menu->right=lp;
          sub_menu->left=lp->left;

          lp->left->right=sub_menu;
          lp->left=sub_menu;
        }
      }
  }
}


void connect_menu(MENU *l_menu,MENU* r_menu)
{
  MENU* lp;
  
  if(r_menu!=0 && l_menu!=0)
  {
    if(l_menu->right==0)
    {
      l_menu->right=r_menu;
      r_menu->left=l_menu;
      
      l_menu->left=r_menu;
      r_menu->right=l_menu;
    }else
    {
      lp=l_menu->right;
      
      l_menu->right=r_menu;
      r_menu->left=l_menu;
      
      r_menu->right=lp;
      lp->left=r_menu;
    }
    if(l_menu->top_menu)
    {
      r_menu->top_menu=l_menu->top_menu;
    }
  }
}


void connect_keyfun(MENU* menu,void (*keyfun)(),KEY keytype)
{
  if(menu!=0)
  {
    switch(keytype)
    {
    case UPKEY:
      {
        menu->onUp=keyfun;
        break;
      }
    case DOWNKEY:
      {
        menu->onDown=keyfun;
        break;
      }
    case RIGHTKEY:
      {
        menu->onRight=keyfun;
        break;
      }
    case LEFTKEY:
      {
        menu->onLeft=keyfun;
        break;
      }
    case CENTERKEY:
      {
        menu->onCenter=keyfun;
        break;
      }
    default: break;
    }
  }
}


void connect_showfun(MENU* menu,void (*showfun)())
{
  if(menu!=0)
  {
    menu->show=showfun;
  }
}


void connect_clkfun(MENU* menu,void (*clkfun)())
{
  if(menu!=0)
  {
    menu->onClk=clkfun;
  }
}


//////////////////////////////////////////////////////////////////////////////
void default_messfun(unsigned int key)  
{                                         //默认的消息处理函数，为空函数
                                          //key为接收键盘键值
}                                         


void default_onUp(void)                   //默认的上键处理函数，为空函数
{
  
}

void default_onDown(void)                 //默认的下键处理函数，为空函数
{
  
}

void default_onRight(void)                //默认的右键处理函数，为空函数
{
  
}

void default_onLeft(void)                 //默认的左键处理函数，为空函数
{
  
}

void default_onCenter(void)               //默认的中心键处理函数，为空函数
{
  
}

void default_onClk(void)                  //默认的软时钟处理函数，为空函数
{
  
}

void default_show(void)                   //默认的显示函数，为空函数
{
  
}

///////////////////////////////////////////////////////////////////////////////