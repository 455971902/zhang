
//*****************************Copyright(c)************************************* 
//** 公      司：         深圳华测检测技术股份有限公司(CTI) 
//** 项      目：         多通道气体采样器     
//** 硬件版本号：       
//**================================================================== 
//**------------------------------------ 文件信息-------------------------------
//** 文  件 名:      menu.c
//** 文件 描述:      实现菜单结构体的创建函数，初始化等函数，默认的按键处理函数
//** 版     本:      v0.1
//** 创  建 人:      陈世培
//** 创建 日期:      2010.12.29
//**============================================================================

#include "menu.h"          //包含对应的头文件
#include "stdlib.h"        //调用通用的库
#include "string.h"

static struct MENU* menuBuf[100];
static int menuNum = 0;




/**************************************************************************
**函数名称:      extern struct MENU* newMenu
**功    能:          建立菜单结构体函数，主要完成从堆里给菜单结构体分配内存，并
                 初始化结构体里的值
**参    数:      void
**返 回 值:      struct MENU* 返回指向菜单结构体的指针
**注    意：     
***************************************************************************/
extern struct MENU* newMenu(void)      //新建菜单结构体函数
{
  struct MENU *re;     //返回值
  
  re = (struct MENU*)malloc(sizeof(struct MENU));    //为菜单分配内存
  
  menuBuf[menuNum] = re;
  menuNum++;
  
  initMenu(re);                    //初始化菜单
  
  return re;
}

void freeAllMenu(void)
{
    int i;
    
    for(i = 0; i <= menuNum; i++)
    {
        if(menuBuf[i])
        {
            memset(menuBuf[i],0,sizeof(struct MENU));
            free(menuBuf[i]);
            menuBuf[i] = 0;
        }
    }
    menuNum = 0;
}



/**************************************************************************
**函数名称:      static void initMenu
**功    能:      在给分配内存给菜单时对菜单进行初始化，使其成员有一个合适的值
**参    数:      menu指针传入需要设置的菜单的指针
**返 回 值:      void
**注    意：     
***************************************************************************/
static void initMenu(struct MENU *menu)    //出示化菜单函数
{
   menu->topMenu = 0;      //使其上层菜单为空
   menu->subMenu = 0;      //使其下层菜单为空
   
   menu->lMenu = 0;        //使其左伙伴菜单为空
   menu->rMenu = 0;        //使其右伙伴菜单为空
   
 //  menu->x = 0;
 //  menu->y = 0;            //使菜单坐标为(0,0)
   
 //  menu->text = 0;         //使文本为无
   
   menu->status1 = 0;      //使两个辅助量为0
   menu->status2 = 0;
   
   menu->onUp = defaultKeyfun;    //所有的按键处理函数指针设为defaultKeyfun
   menu->onDown = defaultKeyfun;
   menu->onLeft = defaultKeyfun;
   menu->onRight = defaultKeyfun;
   menu->onOk = defaultKeyfun;
   menu->onEsc = defaultKeyfun;   
   menu->longOnOk = defaultKeyfun;   
   menu->show = defaultKeyfun;
   menu->menuExec = defaultKeyfun;
}






/**************************************************************************
**函数名称:      extern void connectUpKey
**功    能:      实现连接菜单的上键处理函数
**参    数:      menu 为需要连接的菜单的指针，
                 keyfun 为相应的按键处理函数
**返 回 值:      void
**注    意：     
***************************************************************************/
extern void connectUpKey(struct MENU *menu,void (*keyfun)(void))
{
  if(menu && keyfun)
  {
    menu->onUp = keyfun;
  }
}





/**************************************************************************
**函数名称:      extern void connectDownKey
**功    能:      连接菜单的下键处理函数
**参    数:      menu 为需要连接的菜单指针
                 keyfun 为需要连接的按键处理函数
**返 回 值:      void
**注    意：     
***************************************************************************/
extern void connectDownKey(struct MENU *menu,void (*keyfun)(void))
{
  if(menu && keyfun)
  {
    menu->onDown = keyfun;
  }
}




/**************************************************************************
**函数名称:      extern void connectLeftKey
**功    能:      连接菜单左键处理函数
**参    数:      menu 为需要连接的菜单
                 keyfun为需要连接的函数
**返 回 值:      void
**注    意：     
***************************************************************************/
extern void connectLeftKey(struct MENU *menu,void (*keyfun)(void))
{
  if(menu && keyfun)
  {
    menu->onLeft = keyfun;
  }
}



/**************************************************************************
**函数名称:      extern void connectRightKey
**功    能:      连接菜单右键处理函数
**参    数:      menu 为需要连接的菜单
                 keyfun 为需要连接的右键处理函数
**返 回 值:      void
**注    意：     
***************************************************************************/
extern void connectRightKey(struct MENU *menu,void (*keyfun)(void))
{
  if(menu && keyfun)
  {
    menu->onRight = keyfun;
  }
}








/**************************************************************************
**函数名称:      extern void connectOkKey
**功    能:      连接菜单的OK键处理函数
**参    数:      menu 为需要连接的菜单指针
                 keyfun为需要连接的Ok键处理函数
**返 回 值:      void
**注    意：     
***************************************************************************/
extern void connectOkKey(struct MENU *menu,void (*keyfun)(void))
{
  if(menu && keyfun)
  {
    menu->onOk= keyfun;
  }
}





/**************************************************************************
**函数名称:      extern void connectEscKey
**功    能:      连接菜单的Esc键处理函数
**参    数:      menu 为需要连接的菜单结构体
                 keyfun为需要连接的Esc键处理函数
**返 回 值:      void
**注    意：     
***************************************************************************/
extern void connectEscKey(struct MENU *menu,void (*keyfun)(void))
{
  if(menu && keyfun)
  {
    menu->onEsc = keyfun;
  }
}




/**************************************************************************
**函数名称:      extern void connectLongOkKey
**功    能:        连接菜单的长按  OK 键处理函数
**参    数:        menu 为需要连接的菜单指针
                keyfun为需要连接的Ok键处理函数
**返 回 值:       void
**注    意：
***************************************************************************/
extern void connectLongOkKey( struct MENU *menu, void (*keyfun)(void) )
{
	if ( menu && keyfun )
	{
		menu->longOnOk = keyfun;
	}
}






/**************************************************************************
**函数名称:      extern void connectShowfun
**功    能:      连接菜单的显示函数
**参    数:      menu 为需要连接的菜单
                 showfun 为需要连接的显示函数
**返 回 值:      void
**注    意：     
***************************************************************************/
extern void connectShowfun(struct MENU *menu,void (*showfun)(void))
{
  if(menu && showfun)
  {
    menu->show = showfun;
  }
}



/**************************************************************************
**函数名称:      extern void connectExec
**功    能:      连接菜单Exec函数的函数
**参    数:      menu 为需要连接的菜单
                 execFun 为需要连接的Exec函数
**返 回 值:      void
**注    意：     
***************************************************************************/
extern void connectExec(struct MENU *menu,void (*execFun)(void))
{
  if(menu && execFun)
  {
    menu->menuExec = execFun;
  }
}




/**************************************************************************
**函数名称:      extern void  connectMenu
**功    能:         实现两个菜单的互连，当两个独立的菜单连接时形成一个循环链表，
                 如果lMenu菜单已经有伙伴菜单，那么将断开其节点，将rMenu舔到循环
                 链表中，重新闭合形成一个循环链表。
**参    数:      lMenu 为需要连接的左菜单
                 rMenu 为需要连接的右菜单
**返 回 值:      void
**注    意：     
***************************************************************************/
extern void connectMenu(struct MENU *lMenu,struct MENU *rMenu)  //连接两个菜单函数
{
  struct MENU* lp;
  
  if(rMenu != 0 && lMenu != 0)
  {
    if(lMenu->rMenu == 0)
    {
      lMenu->rMenu = rMenu;
      rMenu->lMenu = lMenu;
      
      lMenu->lMenu = rMenu;
      rMenu->rMenu = lMenu;
    }else
    {
      lp = lMenu->rMenu;
      
      lMenu->rMenu = rMenu;
      rMenu->lMenu = lMenu;
      
      rMenu->rMenu = lp;
      lp->lMenu = rMenu;
    }
    if(lMenu->topMenu)
    {
      rMenu->topMenu = lMenu->topMenu;
    }
  }
}



/**************************************************************************
**函数名称:      extern void  addSubMenu
**功    能:          使topMenu所指向的菜单添加一个由subMenu指向的菜单作为其下层
                 菜单。使subMenu所指向的菜单的所有伙伴菜单的父菜单为topMenu

**参    数:      topMenu为要设置的上层菜单，subMenu为要连接的下层菜单
**返 回 值:      void
**注    意：     
***************************************************************************/
extern void addSubMenu(struct MENU *topMenu,struct MENU *subMenu)  //给topMenu添加子菜单函数
{
  struct MENU* lp;
  
  if(topMenu!=0 && subMenu!=0)
  {
      if(topMenu->subMenu==0)
      {
        topMenu->subMenu=subMenu;
        subMenu->topMenu=topMenu;
      }else
      {
        lp=topMenu->subMenu;
        if(lp->rMenu==0)
        {
          subMenu->topMenu=topMenu;
      
          lp->rMenu=subMenu;
          lp->lMenu=subMenu;
      
          subMenu->rMenu=lp;
          subMenu->lMenu=lp;
        }else
        {
          subMenu->topMenu=topMenu;
      
          subMenu->rMenu=lp;
          subMenu->lMenu=lp->lMenu;

          lp->lMenu->rMenu=subMenu;
          lp->lMenu=subMenu;
        }
      }
  }
}




/**************************************************************************
**函数名称:      static void defaultKeyfun
**功    能:      默认的菜单按键处理函数
**参    数:      void
**返 回 值:      void
**注    意：     
***************************************************************************/
static void defaultKeyfun(void)     //默认的按键处理函数
{
	
}
