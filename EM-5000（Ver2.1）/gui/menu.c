
//*****************************Copyright(c)************************************* 
//** ��      ˾��         ���ڻ����⼼���ɷ����޹�˾(CTI) 
//** ��      Ŀ��         ��ͨ�����������     
//** Ӳ���汾�ţ�       
//**================================================================== 
//**------------------------------------ �ļ���Ϣ-------------------------------
//** ��  �� ��:      menu.c
//** �ļ� ����:      ʵ�ֲ˵��ṹ��Ĵ�����������ʼ���Ⱥ�����Ĭ�ϵİ���������
//** ��     ��:      v0.1
//** ��  �� ��:      ������
//** ���� ����:      2010.12.29
//**============================================================================

#include "menu.h"          //������Ӧ��ͷ�ļ�
#include "stdlib.h"        //����ͨ�õĿ�
#include "string.h"

static struct MENU* menuBuf[100];
static int menuNum = 0;




/**************************************************************************
**��������:      extern struct MENU* newMenu
**��    ��:          �����˵��ṹ�庯������Ҫ��ɴӶ�����˵��ṹ������ڴ棬��
                 ��ʼ���ṹ�����ֵ
**��    ��:      void
**�� �� ֵ:      struct MENU* ����ָ��˵��ṹ���ָ��
**ע    �⣺     
***************************************************************************/
extern struct MENU* newMenu(void)      //�½��˵��ṹ�庯��
{
  struct MENU *re;     //����ֵ
  
  re = (struct MENU*)malloc(sizeof(struct MENU));    //Ϊ�˵������ڴ�
  
  menuBuf[menuNum] = re;
  menuNum++;
  
  initMenu(re);                    //��ʼ���˵�
  
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
**��������:      static void initMenu
**��    ��:      �ڸ������ڴ���˵�ʱ�Բ˵����г�ʼ����ʹ���Ա��һ�����ʵ�ֵ
**��    ��:      menuָ�봫����Ҫ���õĲ˵���ָ��
**�� �� ֵ:      void
**ע    �⣺     
***************************************************************************/
static void initMenu(struct MENU *menu)    //��ʾ���˵�����
{
   menu->topMenu = 0;      //ʹ���ϲ�˵�Ϊ��
   menu->subMenu = 0;      //ʹ���²�˵�Ϊ��
   
   menu->lMenu = 0;        //ʹ������˵�Ϊ��
   menu->rMenu = 0;        //ʹ���һ��˵�Ϊ��
   
 //  menu->x = 0;
 //  menu->y = 0;            //ʹ�˵�����Ϊ(0,0)
   
 //  menu->text = 0;         //ʹ�ı�Ϊ��
   
   menu->status1 = 0;      //ʹ����������Ϊ0
   menu->status2 = 0;
   
   menu->onUp = defaultKeyfun;    //���еİ���������ָ����ΪdefaultKeyfun
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
**��������:      extern void connectUpKey
**��    ��:      ʵ�����Ӳ˵����ϼ�������
**��    ��:      menu Ϊ��Ҫ���ӵĲ˵���ָ�룬
                 keyfun Ϊ��Ӧ�İ���������
**�� �� ֵ:      void
**ע    �⣺     
***************************************************************************/
extern void connectUpKey(struct MENU *menu,void (*keyfun)(void))
{
  if(menu && keyfun)
  {
    menu->onUp = keyfun;
  }
}





/**************************************************************************
**��������:      extern void connectDownKey
**��    ��:      ���Ӳ˵����¼�������
**��    ��:      menu Ϊ��Ҫ���ӵĲ˵�ָ��
                 keyfun Ϊ��Ҫ���ӵİ���������
**�� �� ֵ:      void
**ע    �⣺     
***************************************************************************/
extern void connectDownKey(struct MENU *menu,void (*keyfun)(void))
{
  if(menu && keyfun)
  {
    menu->onDown = keyfun;
  }
}




/**************************************************************************
**��������:      extern void connectLeftKey
**��    ��:      ���Ӳ˵����������
**��    ��:      menu Ϊ��Ҫ���ӵĲ˵�
                 keyfunΪ��Ҫ���ӵĺ���
**�� �� ֵ:      void
**ע    �⣺     
***************************************************************************/
extern void connectLeftKey(struct MENU *menu,void (*keyfun)(void))
{
  if(menu && keyfun)
  {
    menu->onLeft = keyfun;
  }
}



/**************************************************************************
**��������:      extern void connectRightKey
**��    ��:      ���Ӳ˵��Ҽ�������
**��    ��:      menu Ϊ��Ҫ���ӵĲ˵�
                 keyfun Ϊ��Ҫ���ӵ��Ҽ�������
**�� �� ֵ:      void
**ע    �⣺     
***************************************************************************/
extern void connectRightKey(struct MENU *menu,void (*keyfun)(void))
{
  if(menu && keyfun)
  {
    menu->onRight = keyfun;
  }
}








/**************************************************************************
**��������:      extern void connectOkKey
**��    ��:      ���Ӳ˵���OK��������
**��    ��:      menu Ϊ��Ҫ���ӵĲ˵�ָ��
                 keyfunΪ��Ҫ���ӵ�Ok��������
**�� �� ֵ:      void
**ע    �⣺     
***************************************************************************/
extern void connectOkKey(struct MENU *menu,void (*keyfun)(void))
{
  if(menu && keyfun)
  {
    menu->onOk= keyfun;
  }
}





/**************************************************************************
**��������:      extern void connectEscKey
**��    ��:      ���Ӳ˵���Esc��������
**��    ��:      menu Ϊ��Ҫ���ӵĲ˵��ṹ��
                 keyfunΪ��Ҫ���ӵ�Esc��������
**�� �� ֵ:      void
**ע    �⣺     
***************************************************************************/
extern void connectEscKey(struct MENU *menu,void (*keyfun)(void))
{
  if(menu && keyfun)
  {
    menu->onEsc = keyfun;
  }
}




/**************************************************************************
**��������:      extern void connectLongOkKey
**��    ��:        ���Ӳ˵��ĳ���  OK ��������
**��    ��:        menu Ϊ��Ҫ���ӵĲ˵�ָ��
                keyfunΪ��Ҫ���ӵ�Ok��������
**�� �� ֵ:       void
**ע    �⣺
***************************************************************************/
extern void connectLongOkKey( struct MENU *menu, void (*keyfun)(void) )
{
	if ( menu && keyfun )
	{
		menu->longOnOk = keyfun;
	}
}






/**************************************************************************
**��������:      extern void connectShowfun
**��    ��:      ���Ӳ˵�����ʾ����
**��    ��:      menu Ϊ��Ҫ���ӵĲ˵�
                 showfun Ϊ��Ҫ���ӵ���ʾ����
**�� �� ֵ:      void
**ע    �⣺     
***************************************************************************/
extern void connectShowfun(struct MENU *menu,void (*showfun)(void))
{
  if(menu && showfun)
  {
    menu->show = showfun;
  }
}



/**************************************************************************
**��������:      extern void connectExec
**��    ��:      ���Ӳ˵�Exec�����ĺ���
**��    ��:      menu Ϊ��Ҫ���ӵĲ˵�
                 execFun Ϊ��Ҫ���ӵ�Exec����
**�� �� ֵ:      void
**ע    �⣺     
***************************************************************************/
extern void connectExec(struct MENU *menu,void (*execFun)(void))
{
  if(menu && execFun)
  {
    menu->menuExec = execFun;
  }
}




/**************************************************************************
**��������:      extern void  connectMenu
**��    ��:         ʵ�������˵��Ļ����������������Ĳ˵�����ʱ�γ�һ��ѭ������
                 ���lMenu�˵��Ѿ��л��˵�����ô���Ͽ���ڵ㣬��rMenu��ѭ��
                 �����У����±պ��γ�һ��ѭ������
**��    ��:      lMenu Ϊ��Ҫ���ӵ���˵�
                 rMenu Ϊ��Ҫ���ӵ��Ҳ˵�
**�� �� ֵ:      void
**ע    �⣺     
***************************************************************************/
extern void connectMenu(struct MENU *lMenu,struct MENU *rMenu)  //���������˵�����
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
**��������:      extern void  addSubMenu
**��    ��:          ʹtopMenu��ָ��Ĳ˵����һ����subMenuָ��Ĳ˵���Ϊ���²�
                 �˵���ʹsubMenu��ָ��Ĳ˵������л��˵��ĸ��˵�ΪtopMenu

**��    ��:      topMenuΪҪ���õ��ϲ�˵���subMenuΪҪ���ӵ��²�˵�
**�� �� ֵ:      void
**ע    �⣺     
***************************************************************************/
extern void addSubMenu(struct MENU *topMenu,struct MENU *subMenu)  //��topMenu����Ӳ˵�����
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
**��������:      static void defaultKeyfun
**��    ��:      Ĭ�ϵĲ˵�����������
**��    ��:      void
**�� �� ֵ:      void
**ע    �⣺     
***************************************************************************/
static void defaultKeyfun(void)     //Ĭ�ϵİ���������
{
	
}
