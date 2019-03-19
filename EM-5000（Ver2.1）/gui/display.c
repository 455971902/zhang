

#include "lcd.h"
#include "key.h"
#include "display.h"
#include "stdlib.h"
#include "string.h"
#include "adc12.h"
#include "pump.h"
#include "charger.h"
#include "test.h"
#include "ccd.h"
#include "eeprom.h"
#include "tick.h"

static struct WINDOWS *win;     //窗口指针
static struct GUI     *gui;
static unsigned int NUM=50; 
SYSTEM systems;
struct PASSWORD password;
static unsigned int lock_cnt=0; 
extern REVIEW_DATA storeData;
extern float env_temp;
extern float curPressure; 


extern unsigned char setMod;
extern unsigned char lastStoreNum;				//最后存储的编号
extern MANU setManuParameter;					//手动设置参数
extern GROUP setGroupParameter[10];				//定时设置参数
extern CYL   setCylsParameter;              	// 循环采样结构体
extern CLV	  setCapParameter;				  	//定容模式结构体

static unsigned char  review_Num = 0;			//查看时显示的存储下标号(0~19)

static float cail_Flow = 0.0; 					//瞬时标况流量ml

void initCheck(void)
{


}



void setFlow(int x, int y, unsigned int flow, char state);

void setTime(int x, int y, unsigned char hour,unsigned char min, char state);
void setVol(int x, int y, double vol, char state);
void show_time( unsigned int mod );
struct GUI* newGui(void)    //新建一个界面
{
    struct GUI* re = 0;
    
    re = (struct GUI*)malloc(sizeof(struct GUI));
    
    if(re == 0)
    {
        return 0;
    }
    
    re->guiExec = guiExec;       //设置线程函数
    re->batLevel = 0;
    re->win = 0;
    
    gui = re;
	charge();                        //执行充电操作
    check();          //执行自检操作
	
	readReviewNum();				//恢复上次的采样组号
	review_Num = lastStoreNum;		
	eeprom_ReadMod();				//恢复上次的采样模式
	initSetPara();				
	
	
    gui->win = newWindow();       //新建一个窗口
    gui->win->curfous->show();    //显示新窗口
    
    setMaxFlow(MAX_FLOW);    //设置仪器的最大采样流量
    setMinFlow(MIN_FLOW);    //设置仪器的最小采样流量
    setMidflow(MID_FLOW);    //设置间隔流量
   
    
    return re;
}



void guiExec(void)                //界面执行中的循环程序
{
    charge();                        //执行充电操作
    gui->batLevel = getBatLev();
    
    if(gui->win)
    {
        gui->win->batLevel = gui->batLevel;
        gui->win->keyEventFun(getKey());   //执行窗口事件查询函数
    }
    if(gui->batLevel==5)
    {  
       static unsigned int ct=0;
       clr_lcd();
       print_ch_str( 46, 30, "电量低", 0 );
       ct++;
	   delay(58000);
       if(ct==10)
       {
         ct=0; 
		 if(systems.status == SAMPLING)
		 {
			 writeReviewData(lastStoreNum);
			 writeReviewNum();
		 }
         stopPump();   //停止泵转动
         powerOff();   //关机
       }
    }   
}


void drawBatBigIcn(int x,int y,int mod)
{
    static int batCount = 0;
    static int oldChargeState = -1;
    int newChargeState = 0;
    
    
    newChargeState = isCharge();
    if(x >= 0 && y >=0 && mod)
    {
        batCount = 0;
        fill_rect(x,y,60,4);
        fill_rect(x,y+4,4,24);
        fill_rect(x,y+28,60,4);
        fill_rect(x+56,y+4,4,24);
        fill_rect(x+64,y+4,4,24);
        fill_rect(x+60,y+4,4,4);
        fill_rect(x+60,y+24,4,4);
        
        oldChargeState = newChargeState;
        fill_rect(x+32,y+8,8,16);
        fill_rect(x+44,y+8,8,16);
        fill_rect(x+20,y+8,8,16);
        fill_rect(x+8,y+8,8,16);
    }
    
    if(newChargeState)
    {
        if(gui->batLevel == 0)
        {
            if(batCount == 100)
            {
                clr_rect(x+8,y+8,44,16);
                batCount = 0;
            }else if(batCount == 80)
            {
                fill_rect(x+44,y+8,8,16);
                fill_rect(x+32,y+8,8,16);
                fill_rect(x+20,y+8,8,16);
                fill_rect(x+8,y+8,8,16);
            }else if(batCount == 60)
            {
                fill_rect(x+32,y+8,8,16);
                fill_rect(x+20,y+8,8,16);
                fill_rect(x+8,y+8,8,16);
            }else if(batCount == 40)
            {
                fill_rect(x+20,y+8,8,16);
                fill_rect(x+8,y+8,8,16);
            }else if(batCount == 20)
            {
                fill_rect(x+8,y+8,8,16);
            }
        }else if(gui->batLevel == 1)
        {
            if(batCount == 80)
            {
                clr_rect(x+20,y+8,32,16);
                batCount = 0;
            }else if(batCount == 60)
            {
                fill_rect(x+44,y+8,8,16);
                fill_rect(x+32,y+8,8,16);
                fill_rect(x+20,y+8,8,16);
                fill_rect(x+8,y+8,8,16);
            }else if(batCount == 40)
            {
                fill_rect(x+32,y+8,8,16);
                fill_rect(x+20,y+8,8,16);
                fill_rect(x+8,y+8,8,16);
            }else if(batCount == 20)
            {
                fill_rect(x+20,y+8,8,16);
                fill_rect(x+8,y+8,8,16);
            }
        }else if(gui->batLevel == 2)
        {
            if(batCount == 60)
            {
                clr_rect(x+32,y+8,20,16);
                batCount = 0;
            }else if(batCount == 40)
            {
                fill_rect(x+44,y+8,8,16);
                fill_rect(x+32,y+8,8,16);
                fill_rect(x+20,y+8,8,16);
                fill_rect(x+8,y+8,8,16);
            }else if(batCount == 20)
            {
                fill_rect(x+32,y+8,8,16);
                fill_rect(x+20,y+8,8,16);
                fill_rect(x+8,y+8,8,16);
            }
        }else if(gui->batLevel == 3)
        {
            if(batCount == 40)
            {
                clr_rect(x+44,y+8,8,16);
                batCount = 0;
            }else if(batCount == 20)
            {
                fill_rect(x+44,y+8,8,16);
                fill_rect(x+32,y+8,8,16);
                fill_rect(x+20,y+8,8,16);
                fill_rect(x+8,y+8,8,16);
            }
        }else if(gui->batLevel == 4)
        {
            batCount = 0;
            fill_rect(x+44,y+8,8,16);
        }
        batCount ++;
        oldChargeState = newChargeState;
    }else
    {
        if(newChargeState != oldChargeState)
        {
            oldChargeState = newChargeState;
            fill_rect(x+32,y+8,8,16);
            fill_rect(x+44,y+8,8,16);
            fill_rect(x+20,y+8,8,16);
            fill_rect(x+8,y+8,8,16);
        }
    }
}


void maintainMenuShow(void)
{
	struct MENU *lp;
	int index_icn;			// 用于打印图标的索引
	
	clr_lcd();
	win->mLevel = 1;
	lp = win->curfous;
	
	draw_line(32,15,122,15);
    draw_rectmod();
	draw_symbol(7,4,14,14,wei_hu[0],0);
	draw_symbol(7,4+14,14,14,wei_hu[1],0);
	draw_symbol(7,4+28,14,14,wei_hu[2],0);
	draw_symbol(7,4+43,14,14,wei_hu[3],0);
	
	for(index_icn = 0; index_icn < 4; index_icn++)
	{
        if(index_icn==lp->status1)
        {
			draw_symbol( 37+index_icn*22-3, 23, 21,21,
					view_Mbar_mu[index_icn], 1 );
			draw_rect(37+index_icn*22-5,23-2,25,25);
		}
		else
		{
			draw_symbol( 37+index_icn*22, 19+4, 16,16,
						view_bar_mu[index_icn], 0 );
			
		}
	}
	print_ch_str( 53, 3, view_bar_text_chn[lp->status1], 0 );

}
void maintainMenuEsc(void)
{
	struct MENU *lp;
	lp = win->curfous;
    if ( lp->topMenu )
    {
        win->curfous = lp->topMenu;
        win->curfous->show();
    }
}
void maintainMenuLeftKey(void)
{
	struct MENU *lp;
	int before_index_icn;	// 当前焦点改变前位置
	int after_index_icn;	// 当前焦点改变后位置
	lp = win->curfous;

	before_index_icn = lp->status1;	// 当前焦点改变前位置

	
	clr_rect(37+before_index_icn*22-5,23-2,25,25);
	draw_symbol( 37+before_index_icn*22, 19+4, 16,16,
        		view_bar_mu[before_index_icn], 0 );

	if ( lp->status1 == 0 )
	{
		lp->status1 = 3;	// 使焦点维持在0——5之间
	}
	else
	{
		lp->status1--;	// 改变当前焦点
	}
	after_index_icn = lp->status1;	// 当前焦点改变后位置

	draw_symbol( 37+after_index_icn*22-3, 23, 21,21,
        		view_Mbar_mu[after_index_icn], 1 );
	draw_rect(37+after_index_icn*22-5,23-2,25,25);
	
	print_ch_str( 53, 3, view_bar_text_chn[after_index_icn], 0 );

}

void maintainMenuRightKey(void)
{
	struct MENU *lp;
	int before_index_icn;	// 当前焦点改变前位置
	int after_index_icn;	// 当前焦点改变后位置
	lp = win->curfous;

	before_index_icn = lp->status1;	// 当前焦点改变前位置

	
	clr_rect(37+before_index_icn*22-5,23-2,25,25);
	draw_symbol( 37+before_index_icn*22, 19+4, 16,16,
        		view_bar_mu[before_index_icn], 0 );

	if ( lp->status1 == 3 )
	{
		lp->status1 = 0;	// 使焦点维持在0——5之间
	}
	else
	{
		lp->status1++;	// 改变当前焦点
	}
	after_index_icn = lp->status1;	// 当前焦点改变后位置

	draw_symbol( 37+after_index_icn*22-3, 23, 21,21,
        		view_Mbar_mu[after_index_icn], 1 );
	draw_rect(37+after_index_icn*22-5,23-2,25,25);
	
	print_ch_str( 53, 3, view_bar_text_chn[after_index_icn], 0 );

}

void maintainMenuOkKey(void)
{
	struct MENU *lp;
	
	lp = win->curfous;

	switch ( lp->status1 )
	{
		case 0:	// 查询界面
			if ( lp->subMenu )
			{ 
				if((systems.status != STOP)&&(systems.status != COMPLETE))
				{					
					win->curfous = lp->topMenu;
				}
				else
				{
					review_Num = lastStoreNum;	
					readAllReviewData(review_Num);
					win->curfous = lp->subMenu;					
				}
				win->curfous->show();
			}
			break;
			
		case 1:			//
			if ( lp->subMenu )
			{   
				win->curfous = lp->subMenu->rMenu;
				win->curfous->show();
			}
			break;
		
		case 2:
			if ( lp->subMenu )
			{   
				win->curfous = lp->subMenu->rMenu->rMenu;
				win->curfous->show();
			}
			break;
			
		case 3:
			if ( systems.status == STOP)
			{   
				win->oldCurfous = win->curfous;
				win->curfous = win->cleanMenu;				
			}
			else
			{
				win->curfous = lp->lMenu;	
			}
			win->curfous->show();
			break;		
		default:
			break;
	}
}
void maintainMenuLongKey(void)
{
    struct MENU *lp;
	lp = win->curfous;

	
	if ( lp->subMenu && (lp->status1 == 1 ))
	{
		 win->curfous = lp->subMenu->rMenu->rMenu->rMenu;
		 win->curfous->show();
	} 
	
}

void show_cur_time(void)
{
	static int count = 0;
	static const char* move[6] = {"年","月","日","时","分","秒"};	
	
	clr_rect(VIEW_X,VIEW_Y+13,70,12);
	
	switch(count)
	{
		case 0:			
			if(storeData.curTime.year >= 10)
			{
				print_int(VIEW_X+50,VIEW_Y+13,storeData.curTime.year,0);  //年
			}
			else
			{
				putchar(VIEW_X+50,VIEW_Y+13,'0',0);
				print_int(VIEW_X+56,VIEW_Y+13,storeData.curTime.year,0);  //年
			}
			break;
			
		case 1:
			if(storeData.curTime.year >= 10)
			{
				print_int(VIEW_X+38,VIEW_Y+13,storeData.curTime.year,0);  //年
			}
			else
			{
				putchar(VIEW_X+38,VIEW_Y+13,'0',0);
				print_int(VIEW_X+44,VIEW_Y+13,storeData.curTime.year,0);  //年
			}
			print_ch_str(VIEW_X+50,VIEW_Y+13,move[0],0);
			break;
			
		case 2:
			if(storeData.curTime.year >= 10)
			{
				print_int(VIEW_X+26,VIEW_Y+13,storeData.curTime.year,0);  //年
			}
			else
			{
				putchar(VIEW_X+26,VIEW_Y+13,'0',0);
				print_int(VIEW_X+32,VIEW_Y+13,storeData.curTime.year,0);  //年
			}
			print_ch_str(VIEW_X+38,VIEW_Y+13,move[0],0);
			if(storeData.curTime.month  >= 10)
			{
				print_int(VIEW_X+50,VIEW_Y+13,storeData.curTime.month  ,0);  //月
			}
			else
			{
				putchar(VIEW_X+50,VIEW_Y+13,'0',0);
				print_int(VIEW_X+56,VIEW_Y+13,storeData.curTime.month,0);  
			}
			break;
			
		case 3:
			if(storeData.curTime.year >= 10)
			{
				print_int(VIEW_X+14,VIEW_Y+13,storeData.curTime.year,0);  //年
			}
			else
			{
				putchar(VIEW_X+14,VIEW_Y+13,'0',0);
				print_int(VIEW_X+20,VIEW_Y+13,storeData.curTime.year,0);  //年
			}
			print_ch_str(VIEW_X+26,VIEW_Y+13,move[0],0);
			if(storeData.curTime.month  >= 10)
			{
				print_int(VIEW_X+38,VIEW_Y+13,storeData.curTime.month  ,0);  //月
			}
			else
			{
				putchar(VIEW_X+38,VIEW_Y+13,'0',0);
				print_int(VIEW_X+44,VIEW_Y+13,storeData.curTime.month,0);  				
			}
			print_ch_str(VIEW_X+50,VIEW_Y+13,move[1],0);
			break;
			
		case 4:
			if(storeData.curTime.year >= 10)
			{
				print_int(VIEW_X+2,VIEW_Y+13,storeData.curTime.year,0);  //年
			}
			else
			{
				putchar(VIEW_X+2,VIEW_Y+13,'0',0);
				print_int(VIEW_X+8,VIEW_Y+13,storeData.curTime.year,0);  //年
			}
			print_ch_str(VIEW_X+14,VIEW_Y+13,move[0],0);
			if(storeData.curTime.month  >= 10)
			{
				print_int(VIEW_X+26,VIEW_Y+13,storeData.curTime.month  ,0);  //月
			}
			else
			{
				putchar(VIEW_X+26,VIEW_Y+13,'0',0);
				print_int(VIEW_X+32,VIEW_Y+13,storeData.curTime.month,0);  				
			}
			print_ch_str(VIEW_X+38,VIEW_Y+13,move[1],0);
			
			if(storeData.curTime.date  >= 10)
			{
				print_int(VIEW_X+50,VIEW_Y+13,storeData.curTime.date  ,0);  //日
			}
			else
			{
				putchar(VIEW_X+50,VIEW_Y+13,'0',0);
				print_int(VIEW_X+56,VIEW_Y+13,storeData.curTime.date,0);  				
			}
			break;
		
		case 5:
			print_ch_str(VIEW_X+2,VIEW_Y+13,move[0],0);
			if(storeData.curTime.month  >= 10)
			{
				print_int(VIEW_X+14,VIEW_Y+13,storeData.curTime.month  ,0);  //月
			}
			else
			{
				putchar(VIEW_X+14,VIEW_Y+13,'0',0);
				print_int(VIEW_X+20,VIEW_Y+13,storeData.curTime.month,0);  				
			}
			print_ch_str(VIEW_X+26,VIEW_Y+13,move[1],0);
			
			if(storeData.curTime.date  >= 10)
			{
				print_int(VIEW_X+38,VIEW_Y+13,storeData.curTime.date  ,0);  //日
			}
			else
			{
				putchar(VIEW_X+38,VIEW_Y+13,'0',0);
				print_int(VIEW_X+44,VIEW_Y+13,storeData.curTime.date,0);  				
			}
			print_ch_str(VIEW_X+50,VIEW_Y+13,move[2],0);
			break;
			
		case 6:
			if(storeData.curTime.month  >= 10)
			{
				print_int(VIEW_X+2,VIEW_Y+13,storeData.curTime.month  ,0);  //月
			}
			else
			{
				putchar(VIEW_X+2,VIEW_Y+13,'0',0);
				print_int(VIEW_X+8,VIEW_Y+13,storeData.curTime.month,0);  				
			}
			print_ch_str(VIEW_X+14,VIEW_Y+13,move[1],0);
			
			if(storeData.curTime.date  >= 10)
			{
				print_int(VIEW_X+26,VIEW_Y+13,storeData.curTime.date  ,0);  //日
			}
			else
			{
				putchar(VIEW_X+26,VIEW_Y+13,'0',0);
				print_int(VIEW_X+32,VIEW_Y+13,storeData.curTime.date,0);  				
			}
			print_ch_str(VIEW_X+38,VIEW_Y+13,move[2],0);
			
			if(storeData.curTime.hour >= 10)
			{
				print_int(VIEW_X+50,VIEW_Y+13,storeData.curTime.hour,0);  //日   		
			}
			else
			{
				putchar(VIEW_X+50,VIEW_Y+13,'0',0);
				print_int(VIEW_X+56,VIEW_Y+13,storeData.curTime.hour,0); 
			} 	
			break;
			
		case 7:
			print_ch_str(VIEW_X+2,VIEW_Y+13,move[1],0);
			
			if(storeData.curTime.date  >= 10)
			{
				print_int(VIEW_X+14,VIEW_Y+13,storeData.curTime.date  ,0);  //日
			}
			else
			{
				putchar(VIEW_X+14,VIEW_Y+13,'0',0);
				print_int(VIEW_X+20,VIEW_Y+13,storeData.curTime.date,0);  				
			}
			print_ch_str(VIEW_X+26,VIEW_Y+13,move[2],0);
			
			if(storeData.curTime.hour >= 10)
			{
				print_int(VIEW_X+38,VIEW_Y+13,storeData.curTime.hour,0);  //日   		
			}
			else
			{
				putchar(VIEW_X+38,VIEW_Y+13,'0',0);
				print_int(VIEW_X+44,VIEW_Y+13,storeData.curTime.hour,0); 
			} 	
			print_ch_str(VIEW_X+50,VIEW_Y+13,move[3],0);
			break;
			
		case 8:
			if(storeData.curTime.date  >= 10)
			{
				print_int(VIEW_X+2,VIEW_Y+13,storeData.curTime.date  ,0);  //日
			}
			else
			{
				putchar(VIEW_X+2,VIEW_Y+13,'0',0);
				print_int(VIEW_X+8,VIEW_Y+13,storeData.curTime.date,0);  				
			}
			print_ch_str(VIEW_X+14,VIEW_Y+13,move[2],0);
			
			if(storeData.curTime.hour >= 10)
			{
				print_int(VIEW_X+26,VIEW_Y+13,storeData.curTime.hour,0);  //日   		
			}
			else
			{
				putchar(VIEW_X+26,VIEW_Y+13,'0',0);
				print_int(VIEW_X+32,VIEW_Y+13,storeData.curTime.hour,0); 
			} 	
			print_ch_str(VIEW_X+38,VIEW_Y+13,move[3],0);
			
			if(storeData.curTime.min >= 10)
			{
				print_int(VIEW_X+50,VIEW_Y+13,storeData.curTime.min,0);  //日   		
			}
			else
			{
				putchar(VIEW_X+50,VIEW_Y+13,'0',0);
				print_int(VIEW_X+56,VIEW_Y+13,storeData.curTime.min,0); 
			}
			break;
			
		case 9:
			print_ch_str(VIEW_X+2,VIEW_Y+13,move[2],0);
			
			if(storeData.curTime.hour  >= 10)
			{
				print_int(VIEW_X+14,VIEW_Y+13,storeData.curTime.hour  ,0);  //日
			}
			else
			{
				putchar(VIEW_X+14,VIEW_Y+13,'0',0);
				print_int(VIEW_X+20,VIEW_Y+13,storeData.curTime.hour,0);  				
			}
			print_ch_str(VIEW_X+26,VIEW_Y+13,move[2],0);
			
			if(storeData.curTime.min >= 10)
			{
				print_int(VIEW_X+38,VIEW_Y+13,storeData.curTime.min,0);  //日   		
			}
			else
			{
				putchar(VIEW_X+38,VIEW_Y+13,'0',0);
				print_int(VIEW_X+44,VIEW_Y+13,storeData.curTime.min,0); 
			} 	
			print_ch_str(VIEW_X+50,VIEW_Y+13,move[4],0);
			break;
			
		case 10:
			if(storeData.curTime.hour  >= 10)
			{
				print_int(VIEW_X+2,VIEW_Y+13,storeData.curTime.hour  ,0);  //日
			}
			else
			{
				putchar(VIEW_X+2,VIEW_Y+13,'0',0);
				print_int(VIEW_X+8,VIEW_Y+13,storeData.curTime.hour,0);  				
			}
			print_ch_str(VIEW_X+14,VIEW_Y+13,move[3],0);
			
			if(storeData.curTime.min >= 10)
			{
				print_int(VIEW_X+26,VIEW_Y+13,storeData.curTime.min,0);  //日   		
			}
			else
			{
				putchar(VIEW_X+26,VIEW_Y+13,'0',0);
				print_int(VIEW_X+32,VIEW_Y+13,storeData.curTime.min,0); 
			} 	
			print_ch_str(VIEW_X+38,VIEW_Y+13,move[3],0);
			
			if(storeData.curTime.sec >= 10)
			{
				print_int(VIEW_X+50,VIEW_Y+13,storeData.curTime.sec,0);  //日   		
			}
			else
			{
				putchar(VIEW_X+50,VIEW_Y+13,'0',0);
				print_int(VIEW_X+56,VIEW_Y+13,storeData.curTime.sec,0); 
			}
			break;
			
		case 11:
			print_ch_str(VIEW_X+2,VIEW_Y+13,move[3],0);
			
			if(storeData.curTime.min  >= 10)
			{
				print_int(VIEW_X+14,VIEW_Y+13,storeData.curTime.min  ,0);  //日
			}
			else
			{
				putchar(VIEW_X+14,VIEW_Y+13,'0',0);
				print_int(VIEW_X+20,VIEW_Y+13,storeData.curTime.min,0);  				
			}
			print_ch_str(VIEW_X+26,VIEW_Y+13,move[4],0);
			
			if(storeData.curTime.sec >= 10)
			{
				print_int(VIEW_X+38,VIEW_Y+13,storeData.curTime.sec,0);  //日   		
			}
			else
			{
				putchar(VIEW_X+38,VIEW_Y+13,'0',0);
				print_int(VIEW_X+44,VIEW_Y+13,storeData.curTime.sec,0); 
			} 	
			print_ch_str(VIEW_X+50,VIEW_Y+13,move[5],0);
			break;		
			
		case 12:
			if(storeData.curTime.min  >= 10)
			{
				print_int(VIEW_X+2,VIEW_Y+13,storeData.curTime.min  ,0);  //日
			}
			else
			{
				putchar(VIEW_X+2,VIEW_Y+13,'0',0);
				print_int(VIEW_X+8,VIEW_Y+13,storeData.curTime.min,0);  				
			}
			print_ch_str(VIEW_X+14,VIEW_Y+13,move[4],0);
			
			if(storeData.curTime.sec >= 10)
			{
				print_int(VIEW_X+26,VIEW_Y+13,storeData.curTime.sec,0);  //日   		
			}
			else
			{
				putchar(VIEW_X+26,VIEW_Y+13,'0',0);
				print_int(VIEW_X+32,VIEW_Y+13,storeData.curTime.sec,0); 
			} 	
			print_ch_str(VIEW_X+38,VIEW_Y+13,move[5],0);
			break;
			
		case 13:
			print_ch_str(VIEW_X+2,VIEW_Y+13,move[4],0);
			
			if(storeData.curTime.sec  >= 10)
			{
				print_int(VIEW_X+14,VIEW_Y+13,storeData.curTime.sec  ,0);  //日
			}
			else
			{
				putchar(VIEW_X+14,VIEW_Y+13,'0',0);
				print_int(VIEW_X+20,VIEW_Y+13,storeData.curTime.sec,0);  				
			}
			print_ch_str(VIEW_X+26,VIEW_Y+13,move[5],0);
			break;
			
		case 14:
			if(storeData.curTime.sec  >= 10)
			{
				print_int(VIEW_X+2,VIEW_Y+13,storeData.curTime.sec ,0);  //日
			}
			else
			{
				putchar(VIEW_X+2,VIEW_Y+13,'0',0);
				print_int(VIEW_X+8,VIEW_Y+13,storeData.curTime.sec,0);  				
			}
			print_ch_str(VIEW_X+14,VIEW_Y+13,move[5],0);
			break;
			
		case 15:
			print_ch_str(VIEW_X+2,VIEW_Y+13,move[5],0);
			break;
		default:
			break;
	}
	if(count >= 15)
	{
		count = 0;
	}
	else
	{
		count++;
	}
	
}


void sumTimeShow(int x, int y, unsigned long time)
{
	unsigned char old_hour = 0;
	unsigned char old_min = 0;
	unsigned char old_sec = 0;

	old_hour = time / 3600;
	old_min = (time/60)%60;		
	old_sec = time %60;
		
	//显示时
	if ( old_hour < 10 )
	{	
		print_int( x, y, 0,0);
		print_int( x+6, y, old_hour,0);
	}
	else
	{
		print_int( x, y, old_hour, 0);
	}
	putchar(x+12,y,'h',0);
	
	//显示分
	if ( old_min < 10 )
	{	// printTime.min宽度为1个字符宽度，在其前面填充0
		
		print_int( x+18, y, 0,0 );
		print_int( x+24, y, old_min, 0 );
	}
	else
	{
		print_int( x+18, y, old_min, 0 );
	}
	putchar(x+30,y,'m',0);
		
	//显示秒
	if ( old_sec < 10 )
	{	// printTime.min宽度为1个字符宽度，在其前面填充0

		print_int( x+36, y, 0, 0 );
		print_int( x+42, y, old_sec, 0 );
	}
	else
	{
		print_int( x+36, y, old_sec, 0 );
	}	
	putchar(x+48,y,'s',0);

}
void viewFlowShow(int x, int y, unsigned int flow, char mod)
{
   if(mod)   //正显
    {           
		
		putchar(x,y,flow/1000+'0',1);
		putchar(x+6,y,(flow/100)%10+'0',1);
		putchar(x+12,y,(flow%100)/10+'0',1);
		putchar(x+18,y,flow%10+'0',1);
		print_str(x+24,y,"ml/min",1);
		
    }
   else   
   {

	   putchar(x,y,flow/1000+'0',0);
	   putchar(x+6,y,(flow/100)%10+'0',0);
	   putchar(x+12,y,(flow%100)/10+'0',0);
	   putchar(x+18,y,flow%10+'0',0);
	   print_str(x+24,y,"ml/min",0);
   } 
	
}

void viewMenuShow(void)
{
	struct MENU *lp;
	char shi = 0,ge = 0;

	win->mLevel = 2;
	lp = win->curfous;	
	
	clr_lcd();
	lp->status1 = 0;
	
    draw_rectmod();
	draw_symbol( 119, 3,7, 58, gundong, 0 );
	
	draw_symbol(7,4,14,14,wei_hu[4],0);
	draw_symbol(7,4+14,14,14,wei_hu[5],0);
	draw_symbol(7,4+28,14,14,wei_hu[2],0);
	draw_symbol(7,4+43,14,14,wei_hu[3],0);
	
	draw_symbol( VIEW_X+16, VIEW_Y, 9, 12, jian_mu1, 0 );
	draw_symbol( VIEW_X+54, VIEW_Y, 9, 12, plus_mu1, 0 );	

	if(review_Num < 99)
	{
		shi = review_Num/10;
		ge = review_Num %10;
	}
	else
	{
		shi = ge = 0;
	}
	draw_symbol(VIEW_X + 30,VIEW_Y-2,9,16,num_char[shi],1); 
	draw_symbol(VIEW_X + 40,VIEW_Y-2,9,16,num_char[ge],1);
		
	switch(lp->status1)
	{
		case 0:
			fill_rect (118+2,10,5,9);	
			print_ch_str(VIEW_X,VIEW_Y+29,"流量",0);
			putchar(VIEW_X+24,VIEW_Y+29,':',0);
			viewFlowShow(VIEW_X+30,VIEW_Y+29,storeData.flow,0);
					
			print_ch_str(VIEW_X,VIEW_Y+42,"累计",0);	
			putchar(VIEW_X+24,VIEW_Y+42,':',0);
			sumTimeShow(VIEW_X+30,VIEW_Y+42,storeData.sumTime);
			break;
			
		case 1:		
			fill_rect (118+2,10+9,5,9);
			print_ch_str(VIEW_X,VIEW_Y+29,"累计",0);	
			putchar(VIEW_X+24,VIEW_Y+29,':',0);
			sumTimeShow(VIEW_X+30,VIEW_Y+29,storeData.sumTime);
			
			print_ch_str(VIEW_X,VIEW_Y+42,"实体",0);
			putchar(VIEW_X+24,VIEW_Y+42,':',0);
			used_volume_show(storeData.entity_V.d,VIEW_X+30,VIEW_Y+42);
			putchar(VIEW_X+72,VIEW_Y+42,'L',0);
			break;
			
		case 2:
			fill_rect (118+2,10+18,5,9);
			print_ch_str(VIEW_X,VIEW_Y+29,"实体",0);
			putchar(VIEW_X+24,VIEW_Y+29,':',0);
			used_volume_show(storeData.entity_V.d,VIEW_X+30,VIEW_Y+29);		
			putchar(VIEW_X+72,VIEW_Y+29,'L',0);
				
			print_ch_str(VIEW_X,VIEW_Y+42,"计温",0);
			putchar(VIEW_X+24,VIEW_Y+42,':',0);
			print_float(VIEW_X+30,VIEW_Y+42,storeData.envT.f,1,0);
			draw_symbol(VIEW_X+60,VIEW_Y+42,12,12,huashi,0);
			break;
			
		case 3:
			fill_rect (118+2,10+27,5,9);
			print_ch_str(VIEW_X,VIEW_Y+29,"计温",0);
			putchar(VIEW_X+24,VIEW_Y+29,':',0);
			print_float(VIEW_X+30,VIEW_Y+29,storeData.envT.f,1,0);
			draw_symbol(VIEW_X+60,VIEW_Y+29,12,12,huashi,0);

			print_ch_str(VIEW_X,VIEW_Y+42,"计压",0);
			putchar(VIEW_X+24,VIEW_Y+42,':',0);
			print_float(VIEW_X+30,VIEW_Y+42,storeData.envP.f,1,0);
			print_str(VIEW_X+60,VIEW_Y+42,"kpa",0);
			break;
			
		case 4:
			fill_rect (118+2,10+36,5,9);	
			print_ch_str(VIEW_X,VIEW_Y+29,"计压",0);
			putchar(VIEW_X+24,VIEW_Y+29,':',0);
			print_float(VIEW_X+30,VIEW_Y+29,storeData.envP.f,1,0);
			print_str(VIEW_X+60,VIEW_Y+29,"kpa",0);
			
			print_ch_str(VIEW_X,VIEW_Y+42,"标体",0);
			putchar(VIEW_X+24,VIEW_Y+42,':',0);
			used_volume_show(storeData.standard_V.d,VIEW_X+30,VIEW_Y+42);
			putchar(VIEW_X+72,VIEW_Y+42,'L',0);
			break;
		default:
			break;
	}
}
void viewMenuEsc(void)
{
	struct MENU *lp;
	lp = win->curfous;
    if ( lp->topMenu )
    {
        win->curfous = lp->topMenu;
        win->curfous->show();
    }

}
void viewMenuExec(void)
{
	static int cnt = 0;
	if(cnt == 8)
	{
		show_cur_time();
		cnt = 0;
	}
	else
	{
		cnt++;
	}
}
void viewMenuOnUp(void)
{
	struct MENU *lp;

	win->mLevel = 2;
	lp = win->curfous;	
	
	switch(lp->status1)
	{
			
		case 1:
			lp->status1 = 0;
			clr_rect(118+2,10+9,5,9);		
			fill_rect (118+2,10,5,9);
			clr_rect(VIEW_X+30,VIEW_Y+29,60,28);
			print_ch_str(VIEW_X,VIEW_Y+29,"流量",0);	
			putchar(VIEW_X+24,VIEW_Y+29,':',0);
			viewFlowShow(VIEW_X+30,VIEW_Y+29,storeData.flow,0);

			print_ch_str(VIEW_X,VIEW_Y+42,"累计",0);
			putchar(VIEW_X+24,VIEW_Y+42,':',0);	
			sumTimeShow(VIEW_X+30,VIEW_Y+42,storeData.sumTime);
			break;
			
		case 2:
			lp->status1 = 1;
			clr_rect(VIEW_X+30,VIEW_Y+29,60,28);
			fill_rect(118+2,10+9,5,9);
			clr_rect(118+2,10+18,5,9);
			print_ch_str(VIEW_X,VIEW_Y+29,"累计",0);
			putchar(VIEW_X+24,VIEW_Y+29,':',0);			
			sumTimeShow(VIEW_X+30,VIEW_Y+29,storeData.sumTime);
			
			print_ch_str(VIEW_X,VIEW_Y+42,"实体",0);	
			putchar(VIEW_X+24,VIEW_Y+42,':',0);	
			used_volume_show(storeData.entity_V.d,VIEW_X+30,VIEW_Y+42);
			putchar(VIEW_X+72,VIEW_Y+42,'L',0);
			break;
		
		case 3:
			lp->status1 = 2;
			clr_rect(118+2,10+27,5,9);		
			fill_rect (118+2,10+18,5,9);
			clr_rect(VIEW_X+30,VIEW_Y+29,60,28);
			print_ch_str(VIEW_X,VIEW_Y+29,"实体",0);	
			putchar(VIEW_X+24,VIEW_Y+29,':',0);
			used_volume_show(storeData.entity_V.d,VIEW_X+30,VIEW_Y+29);
			putchar(VIEW_X+72,VIEW_Y+29,'L',0);
			
			print_ch_str(VIEW_X,VIEW_Y+42,"计温",0);
			putchar(VIEW_X+24,VIEW_Y+42,':',0);
			print_float(VIEW_X+30,VIEW_Y+42,storeData.envT.f,1,0);
			draw_symbol(VIEW_X+60,VIEW_Y+42,12,12,huashi,0);
			break;
			
		case 4:
			lp->status1 = 3;
			clr_rect(118+2,10+36,5,8);		
			fill_rect (118+2,10+27,5,9);
			clr_rect(VIEW_X+30,VIEW_Y+29,60,28);
			print_ch_str(VIEW_X,VIEW_Y+29,"计温",0);	
			putchar(VIEW_X+24,VIEW_Y+29,':',0);
			print_float(VIEW_X+30,VIEW_Y+29,storeData.envT.f,1,0);
			draw_symbol(VIEW_X+60,VIEW_Y+29,12,12,huashi,0);

			print_ch_str(VIEW_X,VIEW_Y+42,"计压",0);
			putchar(VIEW_X+24,VIEW_Y+42,':',0);			
			print_float(VIEW_X+30,VIEW_Y+42,storeData.envP.f,1,0);
			print_str(VIEW_X+60,VIEW_Y+42,"kpa",0);
			break;
		default:
			break;
	}
}

void viewMenuOnDown(void)
{
	struct MENU *lp;

	win->mLevel = 2;
	lp = win->curfous;	
		
	switch(lp->status1)
	{
		case 0:
			lp->status1 = 1;
			clr_rect(118+2,10,5,9);
			fill_rect(118+2,10+9,5,9);
			clr_rect(VIEW_X+30,VIEW_Y+29,60,28);
			print_ch_str(VIEW_X,VIEW_Y+29,"累计",0);
			putchar(VIEW_X+24,VIEW_Y+29,':',0);			
			sumTimeShow(VIEW_X+30,VIEW_Y+29,storeData.sumTime);
			
			print_ch_str(VIEW_X,VIEW_Y+42,"实体",0);	
			putchar(VIEW_X+24,VIEW_Y+42,':',0);	
			used_volume_show(storeData.entity_V.d,VIEW_X+30,VIEW_Y+42);
			putchar(VIEW_X+72,VIEW_Y+42,'L',0);
			break;
		
		case 1:
			lp->status1 = 2;
			clr_rect(118+2,10+9,5,9);		
			fill_rect (118+2,10+18,5,9);
			clr_rect(VIEW_X+30,VIEW_Y+29,60,28);
			print_ch_str(VIEW_X,VIEW_Y+29,"实体",0);	
			putchar(VIEW_X+24,VIEW_Y+29,':',0);
			used_volume_show(storeData.entity_V.d,VIEW_X+30,VIEW_Y+29);
			putchar(VIEW_X+72,VIEW_Y+29,'L',0);
			
			print_ch_str(VIEW_X,VIEW_Y+42,"计温",0);
			putchar(VIEW_X+24,VIEW_Y+42,':',0);
			print_float(VIEW_X+30,VIEW_Y+42,storeData.envT.f,1,0);
			draw_symbol(VIEW_X+60,VIEW_Y+42,12,12,huashi,0);
			break;
			
		case 2:
			lp->status1 = 3;
			clr_rect(118+2,10+18,5,9);		
			fill_rect (118+2,10+27,5,9);
			clr_rect(VIEW_X+30,VIEW_Y+29,60,28);
			print_ch_str(VIEW_X,VIEW_Y+29,"计温",0);	
			putchar(VIEW_X+24,VIEW_Y+29,':',0);
			print_float(VIEW_X+30,VIEW_Y+29,storeData.envT.f,1,0);
			draw_symbol(VIEW_X+60,VIEW_Y+29,12,12,huashi,0);
			
			print_ch_str(VIEW_X,VIEW_Y+42,"计压",0);
			putchar(VIEW_X+24,VIEW_Y+42,':',0);		
			print_float(VIEW_X+30,VIEW_Y+42,storeData.envP.f,1,0);
			print_str(VIEW_X+60,VIEW_Y+42,"kpa",0);
			break;
			
		case 3:
			lp->status1 = 4;
			clr_rect(118+2,10+27,5,9);		
			fill_rect (118+2,10+36,5,9);
			clr_rect(VIEW_X+30,VIEW_Y+29,60,28);
			print_ch_str(VIEW_X,VIEW_Y+29,"计压",0);	
			putchar(VIEW_X+24,VIEW_Y+29,':',0);
			print_float(VIEW_X+30,VIEW_Y+29,storeData.envP.f,1,0);
			print_str(VIEW_X+60,VIEW_Y+29,"kpa",0);
			
			print_ch_str(VIEW_X,VIEW_Y+42,"标体",0);
			putchar(VIEW_X+24,VIEW_Y+42,':',0);		
			used_volume_show(storeData.standard_V.d,VIEW_X+30,VIEW_Y+42);
			putchar(VIEW_X+72,VIEW_Y+42,'L',0);
			break;
		default:		
			break;
	}
}

void viewMenuOnLeft(void)
{	
	
	if(review_Num == 0)
	{
		review_Num = STORE_NUM;
	}
	
	review_Num--;
	readAllReviewData(review_Num);
	
	win->curfous->show();
}

void viewMenuOnRight(void)
{
	review_Num++;
	if(review_Num == STORE_NUM)
	{
		review_Num = 0;
	}
	readAllReviewData(review_Num);

	win->curfous->show();
}


void freeWindow(void)
{
    if(win)
    {
        freeAllMenu();
        memset(win,0,sizeof(struct WINDOWS));
        free(win);
    }
}


struct WINDOWS* newWindow(void)       //新建窗口函数
{
    struct WINDOWS *re;

    re =(struct WINDOWS*)malloc(sizeof(struct WINDOWS));

    re->keyEventFun = keyEvent;          //连接按键处理函数

    re->mLevel = 0;                      //第0级菜单

    re->oldMLevel = 0;
    
    re->oldCurfous=NULL;
    re->twoOldCurfous=NULL;
    re->lockOldCurfous=NULL;
    

    win = re;

    re->batLevel = 3;
    re->show_flag=0;
    re->lock_flag=0;
    re->no_clr=0;
    // 手动模式采样各参数默认值
    InitAllPara();
	//InitStoreData();
	
    struct MENU *errorMenu;
    errorMenu = newMenu();
    win->errMenu = errorMenu;
    
    connectShowfun( errorMenu, errorMenuShow );
    connectLeftKey( errorMenu, errorMenuLeft );
    connectRightKey( errorMenu, errorMenuRight );
    connectOkKey( errorMenu, errorMenuOnOk );
    connectOkKey(errorMenu,errorMenuEsc);
    connectLongOkKey( errorMenu,errMenuLongOkKey);
	
    struct MENU *cleanMenu;
    cleanMenu = newMenu();
    win->cleanMenu = cleanMenu;    
    connectShowfun( cleanMenu, cleanMenuShow );
	connectExec(cleanMenu,cleanMenuExec);
	connectEscKey(cleanMenu,cleanMenuOnEsc);
	
    struct MENU *twoErrMenu;      // 第二个出错处理菜单指针   
    twoErrMenu = newMenu();
   
    win->twoErrMenu = twoErrMenu;
    connectShowfun( twoErrMenu, twoErrMenuShow );
    connectOkKey( twoErrMenu, twoErrMenuOkKey );
    connectLongOkKey( twoErrMenu,errMenuLongOkKey);
	
	struct MENU *lockMenu;      // 第二个出错处理菜单指针  
    lockMenu = newMenu();
   
    win->lockMenu = lockMenu;
    connectShowfun( lockMenu, lockMenuShow );
    connectOkKey  ( lockMenu, lockMenuOkKey);
    connectExec   ( lockMenu, lockMenuExec );
    connectLongOkKey( lockMenu,lockMenuLongOkKey);
	
	
	
	//首页
     struct MENU *topMenu;
    topMenu = newMenu();

    win->menu = topMenu;
    win->curfous = win->menu;
    
    connectShowfun( topMenu, topMenuShow );
    connectOkKey(topMenu,topMenuOkKey);
    connectUpKey(topMenu,topMenuUpKey);
    connectDownKey(topMenu,topMenuDownKey);
    connectLeftKey(topMenu,topMenuLeftKey);
    connectRightKey(topMenu,topMenuRightKey);
    connectEscKey(topMenu,topMenuEsc);
    connectExec( topMenu, topMenuExec);
    connectLongOkKey(topMenu,topMenuLongOkKey);
	
	
		//菜单2    启动警告
    struct MENU *StartWarnMenu;  // 焦点 0
    StartWarnMenu = newMenu();	
	
    addSubMenu(topMenu,StartWarnMenu);
	
    connectShowfun(StartWarnMenu,StartWarnMenuShow);
    connectOkKey(StartWarnMenu,StartWarnMenuOkKey);
    connectEscKey(StartWarnMenu,StartWarnMenuEsc) ;

	
	//菜单3
	//设置菜单
    struct MENU *allSetMenu;     // 焦点 2
    allSetMenu = newMenu();
	
    connectMenu( StartWarnMenu, allSetMenu );
	
    connectShowfun( allSetMenu, allSetMenuShow );
    connectLeftKey( allSetMenu, allSetMenuLeftKey );
    connectRightKey( allSetMenu, allSetMenuRightKey );
    connectOkKey( allSetMenu, allSetMenuOkKey );
    connectEscKey( allSetMenu, allSetMenuEsc );
	
	
	//菜单5
	//模式设置参数
    struct MENU *modSetMenu;   //allset  下的焦点  0
    modSetMenu = newMenu();  
    addSubMenu( allSetMenu, modSetMenu );
    connectShowfun( modSetMenu, modSetMenuShow );
    connectUpKey( modSetMenu, modSetMenuUpKey );
    connectDownKey( modSetMenu, modSetMenuDownKey );
    connectOkKey( modSetMenu, modSetMenuOkKey );
    connectEscKey( modSetMenu, modSetMenuEsc );	
	  
    // 定时模式组号设置菜单

    struct MENU *timeGroupSetMenu;
    timeGroupSetMenu = newMenu();
    connectMenu( modSetMenu, timeGroupSetMenu );

    connectShowfun( timeGroupSetMenu, timeGroupSetMenuShow );

    connectUpKey( timeGroupSetMenu, timeGroupSetMenuOnUp );
    connectDownKey( timeGroupSetMenu, timeGroupSetMenuOnDown );

    connectLeftKey( timeGroupSetMenu, timeGroupSetMenuOnLeft );
    connectRightKey( timeGroupSetMenu, timeGroupSetMenuOnRight );

    connectOkKey( timeGroupSetMenu, timeGroupSetMenuOnOk );
    connectEscKey( timeGroupSetMenu, timeGroupSetMenuOnEsc );

    connectExec( timeGroupSetMenu, timeGroupSetMenuExec );
	

   // 定时关机设置菜单

    struct MENU *timePowerOffSetMenu;
    timePowerOffSetMenu = newMenu();
    connectMenu( timeGroupSetMenu, timePowerOffSetMenu );
    connectShowfun( timePowerOffSetMenu, timePowerOffSetMenuShow );
    connectUpKey( timePowerOffSetMenu, timePowerOffSetMenuOnUp );
    connectDownKey( timePowerOffSetMenu, timePowerOffSetMenuOnDown );
    connectOkKey( timePowerOffSetMenu, timePowerOffSetMenuOnOk );
    connectEscKey( timePowerOffSetMenu, timePowerOffSetMenuOnEsc );
    connectExec( timePowerOffSetMenu, timePowerOffSetMenuExec );


    //定时关机时间设置菜单

    struct MENU *powerTimeSetMenu;
    powerTimeSetMenu = newMenu();
    addSubMenu( timePowerOffSetMenu, powerTimeSetMenu );

    connectShowfun( powerTimeSetMenu, powerTimeSetMenuShow );

    connectUpKey( powerTimeSetMenu, powerTimeSetMenuOnUp );
    connectDownKey( powerTimeSetMenu, powerTimeSetMenuOnDown );

    connectLeftKey( powerTimeSetMenu, powerTimeSetMenuOnLeft );
    connectRightKey( powerTimeSetMenu, powerTimeSetMenuOnRight );

    connectOkKey( powerTimeSetMenu, powerTimeSetMenuOnOk );
    connectEscKey( powerTimeSetMenu, powerTimeSetMenuOnEsc );

    connectExec( powerTimeSetMenu, powerTimeSetMenuExec );
	
	
     //手动模式流量参数设置菜单
  
    struct MENU *manuParaSetMenu;// allseT 下的焦点 1
    manuParaSetMenu = newMenu();
    addSubMenu(modSetMenu,manuParaSetMenu);
    connectShowfun( manuParaSetMenu, manuParaSetMenuShow );
    connectUpKey( manuParaSetMenu, manuParaSetMenuOnUp );
    connectDownKey( manuParaSetMenu, manuParaSetMenuOnDown );
    connectLeftKey( manuParaSetMenu, manuParaSetMenuOnLeft );
    connectRightKey( manuParaSetMenu, manuParaSetMenuOnRight );
    connectOkKey( manuParaSetMenu, manuParaSetMenuOnOk );
    connectEscKey( manuParaSetMenu, manuParaSetMenuOnEsc );
    connectExec( manuParaSetMenu, manuParaSetMenuExec );
	
    // 定时模式参数设置菜单

    struct MENU *timeGroupParaSetMenu;
    timeGroupParaSetMenu = newMenu();
    connectMenu( manuParaSetMenu, timeGroupParaSetMenu );

    connectShowfun( timeGroupParaSetMenu, timeGroupParaSetMenuShow );

    connectUpKey( timeGroupParaSetMenu, timeGroupParaSetMenuOnUp );
    connectDownKey( timeGroupParaSetMenu, timeGroupParaSetMenuOnDown );
	
    connectLeftKey( timeGroupParaSetMenu, timeGroupParaSetMenuOnLeft );
    connectRightKey( timeGroupParaSetMenu, timeGroupParaSetMenuOnRight );
	
    connectOkKey( timeGroupParaSetMenu, timeGroupParaSetMenuOnOk );
    connectEscKey( timeGroupParaSetMenu, timeGroupParaSetMenuOnEsc );
	
	//循环模式参数
    struct MENU *cylParaSetMenu;
    cylParaSetMenu = newMenu();
    connectMenu( timeGroupParaSetMenu, cylParaSetMenu );

    connectShowfun( cylParaSetMenu, cylParaSetMenuShow );

    connectUpKey( cylParaSetMenu, cylParaSetMenuOnUp );
    connectDownKey( cylParaSetMenu, cylParaSetMenuOnDown );
	
    connectLeftKey(  cylParaSetMenu, cylParaSetMenuOnLeft);
    connectRightKey(  cylParaSetMenu, cylParaSetMenuOnRight);

    connectOkKey( cylParaSetMenu, cylParaSetMenuOnOk );
    connectEscKey( cylParaSetMenu, cylParaSetMenuOnEsc );	

	//定容模式参数菜单
	struct MENU *volParaSetMenu;
    volParaSetMenu = newMenu();
	connectMenu(cylParaSetMenu, volParaSetMenu);
	connectShowfun(volParaSetMenu,volParaSetMenuShow);
    connectUpKey( volParaSetMenu, volParaSetMenuOnUp );
    connectDownKey( volParaSetMenu, volParaSetMenuOnDown );
    connectLeftKey(  volParaSetMenu, volParaSetMenuOnLeft);
    connectRightKey(  volParaSetMenu, volParaSetMenuOnRight);
	connectOkKey( volParaSetMenu, volParaSetMenuOnOk );
	connectEscKey( volParaSetMenu, volParaSetMenuOnEsc );

	//采样时模式切换警告菜单   
    
    struct MENU *setTVWarnMenu;
    setTVWarnMenu=newMenu();
    connectMenu(volParaSetMenu,setTVWarnMenu);
    connectShowfun(setTVWarnMenu,setTVWarnMenuShow);
    connectOkKey(setTVWarnMenu,setTVWarnMenuOkKey);
    connectRightKey(setTVWarnMenu,setTVWarnMenuLeftKey);
    connectLeftKey(setTVWarnMenu,setTVWarnMenuLeftKey);
    
    struct MENU *setTVWrongMenu;    
    setTVWrongMenu=newMenu();
    connectMenu(setTVWarnMenu,setTVWrongMenu);
    connectShowfun(setTVWrongMenu,setTVWrongMenuShow);
    connectOkKey(setTVWrongMenu,setTVWrongMenuOkKey);
	
	
	//停止采样告警
    struct MENU *StopWarnMenu;   //焦点 5
    StopWarnMenu = newMenu();
   
    connectMenu( allSetMenu,StopWarnMenu);
    connectShowfun(StopWarnMenu,StopWarnMenuShow);
    connectLeftKey(StopWarnMenu,StopWarnMenuLeftKey);
    connectRightKey(StopWarnMenu,StopWarnMenuLeftKey);
    connectOkKey(StopWarnMenu,StopWarnMenuOkKey);
    connectEscKey(StopWarnMenu,StopWarnMenuEsc);
	
	
	//组告警界面
	 struct MENU *groupWarnMenu;
    groupWarnMenu = newMenu();
    addSubMenu( timeGroupParaSetMenu, groupWarnMenu );
    connectShowfun( groupWarnMenu, groupWarnMenuShow );
    connectOkKey( groupWarnMenu, groupWarnMenuOnOk );
    
    struct MENU *groupStartWarnMenu;
    groupStartWarnMenu = newMenu();
    connectMenu( groupWarnMenu, groupStartWarnMenu );    
    connectShowfun( groupStartWarnMenu, groupStartWarnMenuShow );
    connectOkKey( groupStartWarnMenu, groupStartWarnMenuOkKey );
   
    connectLeftKey(  groupStartWarnMenu, groupStartWarnMenuLeftKey );
    connectRightKey(  groupStartWarnMenu, groupStartWarnMenuRightKey );
    connectEscKey( groupStartWarnMenu, groupStartWarnMenuEsc );
	
	
    
   // 循环模式错误流量警告菜单
     
    struct MENU *CerrFlowMenu;
    CerrFlowMenu = newMenu();
    addSubMenu( cylParaSetMenu, CerrFlowMenu );
    connectShowfun( CerrFlowMenu, errFlowMenuShow );
    connectOkKey( CerrFlowMenu, errFlowMenuOnOk );
    
	
	//维护界面
	struct MENU *maintainMenu;
    maintainMenu= newMenu();
	connectMenu(StopWarnMenu,maintainMenu);
	connectShowfun(maintainMenu,maintainMenuShow);
	connectEscKey( maintainMenu, maintainMenuEsc );
	connectLeftKey( maintainMenu, maintainMenuLeftKey);
	connectRightKey(maintainMenu,maintainMenuRightKey);
	connectOkKey(maintainMenu,maintainMenuOkKey);
	connectLongOkKey(maintainMenu,maintainMenuLongKey);
	

     // 数据查询菜单
	
	struct MENU *viewMenu;
    viewMenu= newMenu();
	addSubMenu(maintainMenu,viewMenu);
    connectShowfun(viewMenu,viewMenuShow);
	connectEscKey(viewMenu,viewMenuEsc);
	connectExec(viewMenu,viewMenuExec);
    connectUpKey( viewMenu, viewMenuOnUp );
    connectDownKey( viewMenu, viewMenuOnDown );
	connectLeftKey(viewMenu,viewMenuOnLeft);
	connectRightKey(viewMenu,viewMenuOnRight);

		    /*
    ** 输入密码菜单
    */
    struct MENU *inputNumberMenu;
    inputNumberMenu = newMenu();
	
    connectMenu( viewMenu, inputNumberMenu );
    
    connectShowfun( inputNumberMenu, inputNumberMenuShow );

    connectUpKey( inputNumberMenu, inputNumberMenuOnUp );
    connectDownKey( inputNumberMenu, inputNumberMenuOnDown );
    
    connectLeftKey(inputNumberMenu, inputNumberMenuOnLeft);
    connectRightKey(inputNumberMenu, inputNumberMenuOnRight);    

    connectOkKey( inputNumberMenu, inputNumberMenuOnOk );
    connectEscKey( inputNumberMenu, inputNumberMenuOnEsc );
    connectLongOkKey(inputNumberMenu,inputNumberMenuLongOkKey);
	
	
	//系统时间设置菜单
    struct MENU *sysTimeSetMenu;
    sysTimeSetMenu = newMenu();
    connectMenu( inputNumberMenu, sysTimeSetMenu );
    connectShowfun( sysTimeSetMenu, sysTimeSetMenuShow );
    connectUpKey( sysTimeSetMenu, sysTimeSetMenuOnUp );
    connectDownKey( sysTimeSetMenu, sysTimeSetMenuOnDown );
    connectLeftKey( sysTimeSetMenu, sysTimeSetMenuOnLeft );
    connectRightKey( sysTimeSetMenu, sysTimeSetMenuOnRight );
    connectOkKey( sysTimeSetMenu, sysTimeSetMenuOnOk );
    connectEscKey( sysTimeSetMenu, sysTimeSetMenuOnEsc );	
	
	
    struct MENU *cacuTBMenu;
    cacuTBMenu= newMenu();
    connectMenu(sysTimeSetMenu,cacuTBMenu );
    connectShowfun(cacuTBMenu,cacuTBMenuShow);
    connectUpKey( cacuTBMenu, cacuTBMenuOnUp );
    connectDownKey( cacuTBMenu, cacuTBMenuOnDown );
    connectLeftKey( cacuTBMenu, cacuTBMenuOnLeft );
    connectRightKey( cacuTBMenu, cacuTBMenuOnRight );
    connectOkKey( cacuTBMenu, cacuTBMenuOnOk );
    connectEscKey(cacuTBMenu, cacuTBMenuOnEsc);
    connectExec( cacuTBMenu, cacuTBMenuExec );
	
	    
    /*
    ** 提示菜单
    */
    struct MENU *clewMenu;
    clewMenu = newMenu();
    addSubMenu( inputNumberMenu, clewMenu );

    connectShowfun( clewMenu, clewMenuShow );
    
    connectLeftKey( clewMenu, clewMenuOnLeft);
    connectRightKey( clewMenu, clewMenuOnRight);    

    connectOkKey( clewMenu, clewMenuOnOk );
    connectEscKey( clewMenu, clewMenuOnEsc );

    connectUpKey(clewMenu,clewMenuOnUp);
    connectDownKey(clewMenu,clewMenuOnDown);
	
	
   //校准流量菜单
    struct MENU *calibrateFlowMenu;
    calibrateFlowMenu = newMenu();
    addSubMenu( clewMenu, calibrateFlowMenu );
    connectShowfun( calibrateFlowMenu, calibrateFlowMenuShow );

    connectUpKey( calibrateFlowMenu, calibrateFlowMenuOnUp );
    connectDownKey( calibrateFlowMenu, calibrateFlowMenuOnDown );

    connectLeftKey( calibrateFlowMenu, calibrateFlowMenuOnLeft );
    connectRightKey( calibrateFlowMenu, calibrateFlowMenuOnRight );

    connectOkKey( calibrateFlowMenu, calibrateFlowMenuOnOk );
    connectEscKey( calibrateFlowMenu, calibrateFlowMenuOnEsc );
    connectExec( calibrateFlowMenu, calibrateFlowMenuExec );    



    
    
    

	

        
//函数连接///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////




    return re;
}

void initSetPara(void)
{
	unsigned char i = 0;
	
	eeprom_ReadManuSetPara();			//恢复手动数据参数
	for(i = 0; i < 10; i++)				//恢复定时参数
	{	
		eeprom_ReadGroupSetPara(i);
	}
	eeprom_ReadCylsSetPara();			//恢复循环参数
	eeprom_ReadCapSetPara();			//恢复定容参数
	
}

void InitStoreData(void)
{
	//storeData.smpNum = storeData[i].
}

void InitAllPara(void)
{
    unsigned int index;
    
    // 手动模式采样各参数默认值
	
	
    systems.status = STOP;
    systems.mod = setMod;
    systems.old_mod = setMod;
    systems.manu.flow = setManuParameter.flow;
    systems.manu.smp_time = setManuParameter.smp_time;
   // systems.manu.tim_vol_status = TIMING;
   // systems.manu.smp_volume = 1.0;
	
	systems.envir_temp = ORIGN_TEMP;
	systems.bar_pre = 101.3;
    systems.set_temp_flag = setManuParameter.temp_flag;
	
    for ( index=0; index<10; index++ )
    {
        systems.groups[index].flow = setGroupParameter[index].flow;
        systems.groups[index].smp_time = setGroupParameter[index].smp_time;
        systems.groups[index].start_time.hour = setGroupParameter[index].start_time.hour;
		systems.groups[index].start_time.min = setGroupParameter[index].start_time.min;
        systems.groups[index].srb = setGroupParameter[index].srb;
    }

    // 循环模式采样各参数默认值
    systems.cyls.cyl_num = setCylsParameter.cyl_num;
    systems.cyls.flow = setCylsParameter.flow;
    systems.cyls.mid_time = setCylsParameter.mid_time;
    systems.cyls.smp_time = setCylsParameter.smp_time;
	
	
    systems.constant_v.cyl_num = setCapParameter.cyl_num;
    systems.constant_v.flow = setCapParameter.flow;
    systems.constant_v.mid_time = setCapParameter.mid_time;
	systems.constant_v.smp_volume.d= setCapParameter.smp_volume.d;

    systems.a_flow = ORIGIN_FLOW;
    systems.a_volume = ORIGN_VOL;
    systems.a_min = 1;

    //systems.language = CHINESE;
   // systems.set_language = CHINESE;
    systems.used_time = 0;
    systems.percent = 0;
    systems.used_sec = 0;
    systems.start_min = 0;
    

}


/*
void mod_Map(void)
{
	unsigned char i = 0;
	
	setMod = systems.mod;
	
	setManuParameter.flow = systems.manu.flow;	
	setManuParameter.smp_time = systems.manu.smp_time;
	setManuParameter.temp_flag = systems.manu.temp_flag;
	setManuParameter.temperature = systems.manu.temperature;
	
	for(i = 0; i < 10; i++)
	{
		setGroupParameter[i].start_time.hour = systems.groups[i].start_time.hour;	
		setGroupParameter[i].start_time.min = systems.groups[i].start_time.min;
		setGroupParameter[i].smp_time = systems.groups[i].smp_time;	
		setGroupParameter[i].flow = systems.groups[i].flow;
		setGroupParameter[i].srb = systems.groups[i].srb;
		
	}
		
	setCylsParameter.flow = systems.cyls.flow;	
	setCylsParameter.mid_time = systems.cyls.mid_time;
	setManuParameter.smp_time = systems.cyls.smp_time;
	setCylsParameter.cyl_num = systems.cyls.cyl_num;
	
	setCapParameter.flow = systems.constant_v.flow;	
	setCapParameter.mid_time = systems.constant_v.mid_time;
	setCapParameter.smp_volume = systems.constant_v.smp_volume;
	setCapParameter.cyl_num = systems.constant_v.cyl_num;
}

*/
void keyEvent(unsigned char key)      //按键处理函数
{
	
    if(systems.status == SAMPLING)
    {
        if(getFault())
        {
           key = CANCELKEY;
        }
        
    }
    

        switch(key)
        {
          case UPKEY:
          {
            win->curfous->onUp();
          }
          break;
          case DOWNKEY:
          {
            win->curfous->onDown();
          }
          break;
          case LEFTKEY:
          {
            win->curfous->onLeft();
          }
          break;
          case RIGHTKEY:
         {
            win->curfous->onRight();
         }
        break;
        case CENTERKEY:
        {
            win->curfous->onOk();
        }
        break;
        case LENTERKEY:
        {
            win->curfous->longOnOk();
        }
        break;
        case CANCELKEY:
        {
            win->curfous->onEsc();
        }
        break;
    
        case KEY_POWER:
        {	
			if(systems.status == SAMPLING)
			{
				writeReviewData(lastStoreNum);
				writeReviewNum();
            }
			stopPump();   //停止泵转动		
            powerOff();   //关机
            goto exit;
        }
      
        default:break;
        }
    

    systems.dynamic_flow = getCurFlow();
    Get_time( &systems.sys_time );
    sysExec();
	win->curfous->menuExec(); 
    
    
exit:
    _NOP();
}

unsigned int test = 0;

void sysExec(void)             //系统的循环执行函数
{
	static unsigned int old_sec = 0;    // 旧的秒
	static unsigned int old_complete = 0;    // 旧的
	unsigned int group, index;
	unsigned int sys_time;          // 当前系统时间，按分计算
	unsigned int grp_start_time;    // 组的启动时间
	unsigned int min_time = 2880;     // 距离最近将要进行采样组所需时间

 
    
    systems.bar_pre = curPressure;
    systems.envir_temp = env_temp;
	
	switch ( systems.status )
	{
     case STOP:
          systems.used_sec = 0;
          systems.start_min = 0;
          systems.percent = 0;
          systems.used_time = 0;
          systems.dynamic_flow = 0; 
          systems.dynamic_volume = 0;        
          systems.group_status = 0;
		  systems.dynamic_cail_volume = 0;
		  systems.oldstatus = 0;
        break;    
        
     case PAUSE:
	 	systems.dynamic_flow = 0;
		break;
		
	case SAMPLING:
		switch(systems.mod )
		{
		case HAND_MOVE_MODE:
			if(old_sec != systems.sys_time.sec)//秒变
			{
				old_sec = systems.sys_time.sec;
				systems.used_sec++;
				systems.dynamic_cail_volume += cail_Flow / 60; // 计算已采气体体积
				systems.dynamic_volume += systems.dynamic_flow / 60; // 计算已采气体体积
				
				storeData.sumTime = (unsigned long)systems.used_time*60 + systems.used_sec;
				storeData.entity_V.d = systems.dynamic_volume;
				storeData.standard_V.d = systems.dynamic_cail_volume;
				
				if ( systems.used_sec == 60 )    // 计满一分钟
				{
					systems.used_sec = 0;
					systems.used_time++;
				}				
			}

			if ( systems.manu.smp_time <= systems.used_time )
			{	// 采样完成
				
				writeReviewData(lastStoreNum);		
				writeReviewNum();
				
				systems.used_time = 0;
				systems.used_sec = 0;
				systems.status = COMPLETE;
				old_complete = 1;
				
				stopPump();
				systems.dynamic_flow = 0;  
					
			}
			break;
		case TIMING_MODE:
			if ( old_sec != systems.sys_time.sec )
			{
				old_sec = systems.sys_time.sec;
				systems.used_sec++;
				
				systems.dynamic_cail_volume += cail_Flow / 60; // 计算已采气体体积
				systems.dynamic_volume += systems.dynamic_flow / 60; // 计算已采气体体积
				
				storeData.sumTime = (unsigned long)systems.used_time*60 + systems.used_sec;
				storeData.entity_V.d = systems.dynamic_volume;
				storeData.standard_V.d = systems.dynamic_cail_volume;
				group = systems.cur_group;

				if ( systems.used_sec == 60 )    // 计满一分钟
				{
					systems.used_sec = 0;
					systems.used_time++;
				}

				if ( systems.used_time >= systems.groups[group].smp_time )   // 已采完成
				{
                   
					writeReviewData(lastStoreNum);	
					writeReviewNum();
					
					systems.group_status &= ~( 0x00000001 << (group*2) );
					systems.group_status |= ( 0x00000002 ) << (group*2);     // 将当前组设为采样完成状态
					sys_time = systems.sys_time.hour * 60;
					sys_time += systems.sys_time.min;

					for ( index = 0; index < 10; index++ )
					{
						if ( 1 == systems.groups[index].srb
							&& !( systems.group_status & ( 0x00000003 << (index*2))))
						{
							grp_start_time = systems.groups[index].start_time.hour * 60;
							grp_start_time += systems.groups[index].start_time.min;

							if ( grp_start_time <= sys_time )
							{
								grp_start_time += 1440;
							}

							if ( min_time > grp_start_time )
							{
								min_time = grp_start_time;
								group = index;
							}
						}
					}

					if ( min_time != 2880 )
					{						
						systems.status = WAITING;                         
						systems.cur_group = group;
					}
					else
					{
						systems.status = COMPLETE;
                        old_complete = 1;                        
						systems.group_status = 0;      // 所有组状态复位
					}
					stopPump();
					systems.used_time = 0;
					systems.used_sec = 0;
					systems.dynamic_flow = 0; 						
					systems.dynamic_volume = 0; 					
					systems.dynamic_cail_volume = 0;
				}
			}
			break;
		case CIRCLE_MODE:
			if ( old_sec != systems.sys_time.sec )
			{				
				old_sec = systems.sys_time.sec;
				systems.used_sec++;
				systems.dynamic_volume += systems.dynamic_flow / 60; // 计算已采气体体积
				systems.dynamic_cail_volume += cail_Flow / 60; // 计算已采气体体积
				
				storeData.sumTime = (unsigned long)systems.used_time*60 + systems.used_sec;
				storeData.entity_V.d = systems.dynamic_volume;
				storeData.standard_V.d = systems.dynamic_cail_volume;

				if ( systems.used_sec == 60 )    // 计满一分钟,chulihanshu
				{
					systems.used_sec = 0;
                 
					systems.used_time++;
				}
			}

			//采样完成
			if ( systems.used_time >= systems.cyls.smp_time )     //本次循环时间已到
            {
				
				writeReviewData(lastStoreNum);
				writeReviewNum();	
				
				if ( systems.cyls.cyl_num - systems.cur_cyls - 1 )
				{
					systems.cur_cyls++;           //当前循环次数加1				
					systems.wait_sec = 59;         //
					systems.status = WAITING;       //进入等待状态，等下一次循环的到来
                    systems.wait_time = systems.cyls.mid_time - 1;    //距离下一次采样需要等待的时间
                        
				}
				else      //所有循环已结束
				{
					systems.cur_cyls = 0;
					systems.status = COMPLETE;
					old_complete = 1;
				}
				stopPump();
				systems.used_time = 0;
				systems.used_sec = 0;
				systems.dynamic_flow = 0; 					
				systems.dynamic_volume = 0;  
				systems.dynamic_cail_volume = 0;
			}			
			break;
			
		case CAP_MODE:
			if ( old_sec != systems.sys_time.sec )
			{
				old_sec = systems.sys_time.sec;		
				systems.used_sec++;
				if ( systems.used_sec == 60 )    // 计满一分钟,chulihanshu
				{
					systems.used_sec = 0;
                 
					systems.used_time++;
				}
				systems.dynamic_volume += systems.dynamic_flow / 60; // 计算已采气体体积
				systems.dynamic_cail_volume += cail_Flow / 60; // 计算已采气体体积

				systems.percent = 1.0 * systems.dynamic_volume / ( systems.constant_v.smp_volume.d * 1000 );
				
				storeData.sumTime = (unsigned long)systems.used_time*60 + systems.used_sec;
				storeData.entity_V.d = systems.dynamic_volume;	
				storeData.standard_V.d = systems.dynamic_cail_volume;	
				
				if ( systems.percent >= 1 )
				{
					systems.percent = 1;
				}
			}

			if ( systems.dynamic_volume >= systems.constant_v.smp_volume.d*1000 )     //本次循环时间已到
            {
				writeReviewData(lastStoreNum);
				writeReviewNum();
				
				if ( systems.constant_v.cyl_num - systems.cur_cyls - 1 )
				{
					systems.cur_cyls++;           //当前循环次数加1
					systems.wait_sec = 59;
					systems.status = WAITING;       //进入等待状态，等下一次循环的到来
                    systems.wait_time = systems.constant_v.mid_time - 1;    //距离下一次采样需要等待的时间
				}
				else      //所有循环已结束
				{
					systems.cur_cyls = 0;
					systems.status = COMPLETE;
					old_complete = 1;	
				}
				systems.used_time = 0;
				systems.used_sec = 0;	
				systems.percent = 0;
				stopPump();
				systems.dynamic_flow = 0; 					
				systems.dynamic_volume = 0;  
				systems.dynamic_cail_volume = 0;
			}	
			break;
		default: break;
		}
		break;

	case COMPLETE: //采样完成弹窗
        if ( old_complete )
        {
            old_complete = 0;
            
            systems.dynamic_flow = 0;
            systems.dynamic_volume = 0;
			systems.dynamic_cail_volume = 0;
             if(win->curfous != win->lockMenu)
            {
               win->twoOldCurfous = win->curfous;
             }
             else
             {
              win->twoOldCurfous = win->lockOldCurfous;
              
             }
             win->curfous = win->twoErrMenu;               
             win->curfous->show();             
        }
		break;

	case WAITING:
        systems.dynamic_flow = 0;
		switch ( systems.mod )
		{
		case CIRCLE_MODE:
			if ( old_sec != systems.sys_time.sec )
			{
				old_sec = systems.sys_time.sec;
				if ( systems.wait_sec == 0 )
				{
					systems.wait_sec = 59;
					if ( systems.wait_time == 0 )
					{
						systems.status = SAMPLING;         // 进入采样状态
						
						if(systems.oldstatus == 0)
						{
							systems.wait_sec = 0;
							
							lastStoreNum++;
							if(lastStoreNum >= STORE_NUM)
							{
								lastStoreNum = 0;
							}
							storeData.curTime.year = systems.sys_time.year;
							storeData.curTime.month = systems.sys_time.month;
							storeData.curTime.date = systems.sys_time.date;
							storeData.curTime.hour = systems.sys_time.hour;
							storeData.curTime.min = systems.sys_time.min;
							storeData.curTime.sec = systems.sys_time.sec;
							storeData.envP.f = systems.bar_pre;		
							storeData.envT.f = systems.envir_temp;
							storeData.flow = systems.cyls.flow;
							systems.oldstatus = 1;
						}
                        startPump( systems.cyls.flow );     // 启动采样                        
					}
					else
					{
						systems.wait_time--;       //等待时间自减1
					}
				}
				else
				{
					systems.wait_sec--;
				}
			}
			break;
			
		case TIMING_MODE:
			if ( old_sec != systems.sys_time.sec )
			{
				old_sec = systems.sys_time.sec;
                                    
				group = systems.cur_group;
				
				if ( systems.groups[group].start_time.hour == systems.sys_time.hour
						&& systems.groups[group].start_time.min == systems.sys_time.min )
				{
					systems.group_status |= 0x00000001 << (group*2);   // 设置为正在采样状态
                    
					systems.status = SAMPLING;    // 进行采样
					lastStoreNum++;
					if(lastStoreNum >= STORE_NUM)
					{
						lastStoreNum = 0;
					}
					storeData.curTime.year = systems.sys_time.year;
					storeData.curTime.month = systems.sys_time.month;
					storeData.curTime.date = systems.sys_time.date;
					storeData.curTime.hour = systems.sys_time.hour;
					storeData.curTime.min = systems.sys_time.min;
					storeData.curTime.sec = systems.sys_time.sec;
					storeData.envP.f = systems.bar_pre;		
					storeData.envT.f = systems.envir_temp;
					storeData.flow = systems.groups[systems.cur_group].flow;
					systems.used_sec = 0;
                    
                    startPump( systems.groups[group].flow );
				}
			}
			break;
			
			case CAP_MODE:
			if ( old_sec != systems.sys_time.sec )
			{
				old_sec = systems.sys_time.sec;
                                    
				if ( systems.wait_sec == 0 )
				{
					systems.wait_sec = 59;
					if ( systems.wait_time == 0 )
					{
						systems.wait_sec = 0;
						systems.status = SAMPLING;         // 进入采样状态
						lastStoreNum++;
						if(lastStoreNum >= STORE_NUM)
						{
							lastStoreNum = 0;
						}  
						storeData.curTime.year = systems.sys_time.year;
						storeData.curTime.month = systems.sys_time.month;
						storeData.curTime.date = systems.sys_time.date;
						storeData.curTime.hour = systems.sys_time.hour;
						storeData.curTime.min = systems.sys_time.min;
						storeData.curTime.sec = systems.sys_time.sec;
						storeData.envP.f = systems.bar_pre;		
						storeData.envT.f = systems.envir_temp;
						storeData.flow = systems.constant_v.flow;
                        startPump( systems.constant_v.flow );     // 启动采样                        
					}
					else
					{
						systems.wait_time--;       //等待时间自减1
					}
				}
				else
				{
					systems.wait_sec--;
				}
			}
				break;

		default: break;
		}

		break;
	default: break;
	}
    
    batlevelExec();
    
    if ( systems.power_status )   // 系统自动关机状态是否启用
    {   
        if ( systems.power_time.hour == systems.sys_time.hour
             && systems.power_time.min == systems.sys_time.min
             && systems.power_time.sec == systems.sys_time.sec )
        {
            stopPump();
            
            powerOff();
        }
    }
    
     if(win->show_flag==1)
    {
       clr_rect(0,0,24,12);
      print_int(0,0,getPress(),0);
      clr_rect(25,0,24,12);
      print_int(25,0,get_smppresure(),0);
      clr_rect(0,25,24,12);
      print_int(0,25,TB0CCR1,0);
      
      print_int(0,50,get_DCO(),0);
    
    }  
    
    
}

void batlevelExec(void)             
{
    if ( systems.bat_count > 0 )
    {
        systems.bat_count--;
    }
    else
    {
        if ( win->batLevel == 0 
            && win->curfous != win->errMenu&&win->twoOldCurfous!=win->errMenu&&win->curfous!=win->lockMenu)
        {
            
            win->oldCurfous = win->curfous;
            win->curfous = win->errMenu;
            win->curfous->show();
        }
    }
}

void errorMenuShow(void)
{

        clr_lcd();
    
        draw_rect( 0, 0, 128, 64 );
        draw_rect( 44, 10, 37, 20 );
        draw_rect( 80, 13, 6, 14 );
       
            print_ch_str( 46, 30, "电量低", 0 );
            draw_button( 85, 45, "确定", 1, 1 );

    
}


void errorMenuLeft(void)
{
    
   
}

void errorMenuRight(void)
{
   
}

void errorMenuOnOk(void)
{
  if(win->lock_flag==0)
  {
   systems.bat_count = 1000;
   win->curfous = win->oldCurfous;
   win->oldCurfous = NULL;
   win->curfous->show();
  }
  else
  {
      win->lockOldCurfous=win->curfous;
      win->curfous=win->lockMenu;
      win->curfous->show();
  
  }
}
void errorMenuEsc(void)
{
  if(win->lock_flag==0)
  {
    systems.bat_count = 1000;
    win->curfous = win->oldCurfous;
    win->oldCurfous = NULL;
    win->curfous->show();

  }
  else
  {
    win->lockOldCurfous=win->curfous;
    win->curfous=win->lockMenu;
    win->curfous->show();
  
  }


}


static void twoErrMenuShow(void)
{
        clr_lcd();
        draw_rect( 0, 0, 128, 64 );
       
            switch ( systems.mod )
            {
            case HAND_MOVE_MODE:
			case CAP_MODE:
                print_ch_str( 38, 26, "采样完成", 0 );
                putchar( 87, 26, '!', 0 );
                draw_button( 93, 46, "确定", 1, CHINESE );
                break;
            case TIMING_MODE:
                print_ch_str( 10, 25, "所有组已采样完成", 0 );
                draw_button( 93, 44, "确定", 1 , CHINESE );
                break;
            case CIRCLE_MODE:
                print_ch_str( 10, 25, "所有循环已采样完成", 0 );
                draw_button( 93, 44, "确定", 1, CHINESE );
                break;
            
            default: break;
            }

    
}

 void twoErrMenuOkKey(void)
{
   if(win->lock_flag==0)
   {    
    win->curfous = win->twoOldCurfous;
    win->twoOldCurfous = NULL; 
    win->curfous->show();
   }
   else
   {
      win->lockOldCurfous=win->curfous;
      win->curfous=win->lockMenu;
      win->curfous->show();
       
   }
         
}
void errMenuLongOkKey(void)
{
   if(win->lock_flag==1)
   {
     win->lock_flag=0;
    
     win->lockOldCurfous=win->curfous;
     win->curfous=win->lockMenu;
     win->curfous->show();
   }
}

void cleanMenuShow(void)
{
	clr_lcd();
  	draw_rect(15,19,91,20);
  	print_ch_str(48,40,"清洁中",0);  
  	startPump(ORIGIN_FLOW);	
}

void cleanMenuExec(void)
{
	static unsigned int time = 0;
	static unsigned int old_sec = 0;
	
	if(old_sec!=systems.sys_time.sec)
	{
		old_sec = systems.sys_time.sec;
		time++;
		fill_rect(15,19,time,20);		
	}
	if(time == 90)
	{
		stopPump();
		time++;
 		clr_rect(48,40,36,12);
		print_ch_str(48,40,"完  成",0);
		
	}	
	else if(time == 92)
	{
		time = 0;
		win->curfous = win->oldCurfous;
		win->oldCurfous = NULL;
		win->curfous->show();
	}
}

void cleanMenuOnEsc(void)
{
	stopPump();
	win->curfous = win->oldCurfous;
	win->oldCurfous = NULL;
	win->curfous->show();
}
void lockMenuShow(void)
{
  clr_lcd();
  draw_rect(0,0,128,64);
  
  switch(win->lock_flag)
  {
  case 0:
      
      draw_symbol(42,3,34,32,unlock,0);
     
        draw_symbol(29,41,62,13,unlock_zi,0);      
      
      break;
  case 1:
      draw_symbol(47,3,34,32,maxlock,0);
    
        draw_symbol(29,37,75,24,lock_zi,0);

      break;
      
  default :
      break;

  
  }
 
}
 
void lockMenuLongOkKey(void)
{
  // lockMenuShow();
  // lock_cnt=0;
    
}
 void lockMenuOkKey(void)
{ 
   if(win->lock_flag==1)
   {
      
     lock_cnt=0;
   
   } 
   else
   {
     lock_cnt=60;
   
   }
   
   
   
}
void lockMenuExec(void)
{
 
 lock_cnt++;
 
 if(lock_cnt>20)
 {
   lock_cnt=0;
   win->curfous = win->lockOldCurfous;
   win->lockOldCurfous = NULL; 
   win->curfous->show();
 }    
    
} 

void batShow( int mod)    //电池电量显示函数，mod=1为强行显示
{                        //mod = 0为只有电量变化时才显示
    static int oldLevel = 0;		//上一次电量显示
    static int batCount = 0;		//速度
    static int modInter = 0;		//
    
    if(isCharge())     //处于充电状态
    {
        
        if(mod == 1)
        {
            batCount = 0;
            draw_symbol( 109, 1, 18, 8, battery_icn+win->batLevel*18, 0 );
        }
        
        if(win->batLevel == 0)
        {
            if(batCount == 100)
            {
                draw_symbol( 109, 1, 18, 8, battery_icn+4*18, 0 );
                batCount = 0;
            }else if(batCount == 80)
            {
                draw_symbol( 109, 1, 18, 8, battery_icn+3*18, 0 );
            }else if(batCount == 60)
            {
                draw_symbol( 109, 1, 18, 8, battery_icn+2*18, 0 );
            }else if(batCount == 40)
            {
                draw_symbol( 109, 1, 18, 8, battery_icn+18, 0 );
            }else if(batCount == 20)
            {
                draw_symbol( 109, 1, 18, 8, battery_icn, 0 );
            }
        }else if(win->batLevel == 1)
        {
            if(batCount == 80)
            {
                draw_symbol( 109, 1, 18, 8, battery_icn+4*18, 0 );
                batCount = 0;
            }else if(batCount == 60)
            {
                draw_symbol( 109, 1, 18, 8, battery_icn+3*18, 0 );
            }else if(batCount == 40)
            {
                draw_symbol( 109, 1, 18, 8, battery_icn+2*18, 0 );
            }else if(batCount == 20)
            {
                draw_symbol( 109, 1, 18, 8, battery_icn+18, 0 );
            }
        }else if(win->batLevel == 2)
        {
            if(batCount == 60)
            {
                draw_symbol( 109, 1, 18, 8, battery_icn+4*18, 0 );
                batCount = 0;
            }else if(batCount == 40)
            {
                draw_symbol( 109, 1, 18, 8, battery_icn+3*18, 0 );
            }else if(batCount == 20)
            {
                draw_symbol( 109, 1, 18, 8, battery_icn+2*18, 0 );
            }
        }else if(win->batLevel == 3)
        {
            if(batCount == 40)
            {
                draw_symbol( 109, 1, 18, 8, battery_icn+4*18, 0 );
                batCount = 0;
            }else if(batCount == 20)
            {
                draw_symbol( 109, 1, 18, 8, battery_icn+3*18, 0 );
            }
        }else if(oldLevel != win->batLevel)
        {
            oldLevel = win->batLevel;
            draw_symbol( 109, 1, 18, 8, battery_icn+4*18, 0 );
        }
        modInter = 1;
        batCount ++;
        
    }else
    {
        batCount = 0;
        if( mod == 0 && oldLevel != win->batLevel )  //如果新的电量值不等于旧的电量值
        {
            oldLevel = win->batLevel;
            draw_symbol( 109, 1, 18, 8, battery_icn+oldLevel*18, 0 );
        }
        else if( mod || modInter)
        {
            oldLevel = win->batLevel;
            modInter = 0;
            draw_symbol( 109, 1, 18, 8, battery_icn+oldLevel*18, 0 );
        }  
    }
    
}

/*void show_actual_vol( double sumflow, unsigned char mod)
{
	static float vol = 0.0;
	unsigned long temp = 0;
	
	if(mod)
	{
		temp = (unsigned long)sumflow/10;
		vol = sumflow / 1000;
		if(temp)
		{
   			print_float(VOL_X+30,VOL_Y,vol,2,0);
		}
		else
		{
			print_int(VOL_X+60,VOL_Y,vol,0);
		}
	}
	else
	{
		if(temp !=  (unsigned long)sumflow/10)
		{
			temp = (unsigned long)sumflow/10;
			vol = sumflow / 1000;   
			clr_rect(VOL_X+30,VOL_Y,48,12);
			if(temp)
			{
				print_float(VOL_X+30,VOL_Y,vol,2,0);
			}
			else
			{
				print_int(VOL_X+60,VOL_Y,vol,0);
			}
		}
	}
}

*/
void topMenu_handshow(struct MENU *lp)
{
	//draw_symbol( STAT_X, STAT_Y, 12, 12, status_icn[systems.status], 0 );		//运行图标
	print_ch_str( MOD_X, MOD_Y, "手动", 0);
	
	//draw_line( 33, 12, 33, 63 );		// 打印一条竖线	

	//显示设置流量
    draw_symbol( S_FLOW_X, S_FLOW_Y+2, 6, 9, jian_mu, 0 );
    draw_symbol( S_FLOW_X+32, S_FLOW_Y+2, 6, 9, plus_mu, 0 );
    if ( systems.manu.flow >= 1000 )
    {
        print_int( S_FLOW_X+7 , S_FLOW_Y, systems.manu.flow, 0 );
    }
    else if ( systems.manu.flow >= 100 )
    {
         print_int( S_FLOW_X+10 , S_FLOW_Y, systems.manu.flow, 0 );
    }
    else if ( systems.manu.flow > 10 )
    {
         print_int( S_FLOW_X+13 , S_FLOW_Y, systems.manu.flow, 0 );
    }	
	
	
	
	//显示采样时长
	print_ch_str( USED_X, USED_Y, "时长", 0 );    // 显示已采样字样 
	putchar( USED_X+24, USED_Y, ':', 0 );     		// 显示已采样时间min字样	
	show_samp_time(1);    
}

void topMenu_timeshow(struct MENU *lp)
{
	print_ch_str( MOD_X, MOD_Y, "定时", 0 );
	print_str( FLOW_X+54, FLOW_Y, "ml/min", 0 );

	print_ch_str( USED_X, USED_Y, "时长", 0 );    // 显示已采样字样 
	putchar( USED_X+24, USED_Y, ':', 0 );     		// 显示已采样时间min字样	
	show_samp_time(1);
}


void topMenu_circleshow(struct MENU *lp)
{	
	print_ch_str( MOD_X, MOD_Y, "循环", 0 );
	
	print_ch_str( USED_X, USED_Y, "时长", 0 );    // 显示已采样字样 
	putchar( USED_X+24, USED_Y, ':', 0 );     		// 显示已采样时间min字样	
	show_samp_time(1); 
	print_str(FLOW_X+54,FLOW_Y,"ml/min",0);
}

void topMenu_capshow(struct MENU *lp)
{
	print_ch_str( MOD_X, MOD_Y, "定容", 0 );
	print_str(FLOW_X+54,FLOW_Y,"ml/min",0);
	draw_rect( USED_X, USED_Y+1, 62, 10 );            // 画一个进度条	
	putchar( 116, USED_Y, '%', 0 );        // 显示%字样
	//print_int(108, USED_Y, 0, 0 );		// 打印百分比
	percent_show( systems.percent );
}

void press_temp_show(int x, int y, float temp,float pre, char mod)
{
	static float s_temp = 0.0;
	static float s_press = 0.0;
	
	if(mod)
	{
		clr_rect(x,y,84,12);
		s_temp = temp;
		s_press = pre;
		print_str( x, y, "T:      P:     ", 0 );    // 显示已采样字样 
		print_float( x+12, y,s_temp,1,0);	
		print_float( x+60, y,s_press,1,0);	
		draw_symbol(x+36,y+2,8,8,temp_dram,0);
	}
	else
	{
		if(s_temp != temp)
		{
			s_temp = temp;
			clr_rect(x+12,y,24,12);
			print_float( x+12, y,s_temp,1,0);
		}
		
		if(s_press != pre)
		{
			s_press = pre;
			clr_rect(x+60,y,30,12);
			print_float( x+60, y,s_press,1,0);
		}	
	}
}

void topMenuShow(void)
{
    struct MENU *lp;
    lp = win->curfous;
    win->mLevel = 0;
    if(win->no_clr==0) 
	{
		clr_lcd();
	}
    else 
    {
		win->no_clr=0; 
    } 
    
    if(win->lock_flag==1)
    {
		draw_symbol(38,0,18,12,lock_flag,0);
    }
    else
    {
		draw_symbol(38,0,18,12,unlock_flag,0);
    
    }
    
	switch ( lp->status1 )			//左侧图标
	{
		 case 0:
		 {              
              if((COMPLETE != systems.status)&&(STOP != systems.status))  
			  {
				draw_button( 0, 30, "停止", 1, CHINESE );
			  }
			  else
			  {
				  draw_button( 0, 30, "启动", 1, CHINESE );  
			
			  }
			draw_button( 0, 12, "设置", 0, CHINESE );
			draw_button( 0, 48, "维护", 0, CHINESE );
		}
		break;
		
		case 1:
		{   
			  if((COMPLETE != systems.status)&&(STOP != systems.status)) 
			draw_button( 0, 30, "停止", 0, CHINESE );
             else
            draw_button( 0, 30, "启动", 0, CHINESE ); 
           
			draw_button( 0, 12, "设置", 1, CHINESE );
			draw_button( 0, 48, "维护", 0, CHINESE );
		}
		break;
		
		case 2:
		{
			 
			  if((COMPLETE != systems.status)&&(STOP != systems.status)) 
			draw_button( 0, 30, "停止", 0, CHINESE );
             else
            draw_button( 0, 30, "启动", 0, CHINESE);
						
			draw_button( 0, 12, "设置", 0, CHINESE );
			draw_button( 0, 48, "维护", 1, CHINESE ); 
		}
		break;
		
		default:
		break;
	}
	draw_symbol( STAT_X, STAT_Y, 12, 12, status_icn[systems.status], 0 );		//运行图标
	draw_line( 33, 12, 33, 63 );		// 打印一条竖线	
	//显示实际流量
	print_ch_str( FLOW_X, FLOW_Y, "流量", 0 );
	putchar(FLOW_X+24, FLOW_Y,':',0);
	flow_show( (unsigned int)( systems.dynamic_flow ) );    // 动态流量显示
	
	print_ch_str(VOL_X, VOL_Y, "实体", 0 );    // 显示已采样字样 
	print_str( VOL_X+24, VOL_Y, ":       L", 0 );	
	used_volume_show(systems.dynamic_volume,VOL_X+30,VOL_Y);
	
    show_time(1);	//显示时间	
	batShow(1);    	//显示电池图标	
	press_temp_show(P_T_X,P_T_Y,systems.envir_temp,systems.bar_pre,1);
	
	switch ( systems.mod )
	{
		case HAND_MOVE_MODE:
			topMenu_handshow(lp); 
			break;
			  
        case TIMING_MODE:
		 	topMenu_timeshow(lp);  
			break;
			   
        case CIRCLE_MODE:
		 	topMenu_circleshow(lp);
			break;
			
        case CAP_MODE:
		 	topMenu_capshow(lp);
			break;
		default: 
		 	break;
     }   
}
void topMenuLongOkKey(void)
{
   if(win->lock_flag==0)
   {
      win->lock_flag=1;//shangsuo 
      
   } 
  else 
  {
     win->lock_flag=0;//jie suo zhuangtai 
    
  }
   win->lockOldCurfous=win->curfous;
   win->curfous=win->lockMenu;
   win->curfous->show(); 

}
void topMenuUpKey(void)
{
	struct MENU *lp;
    lp=win->curfous;
	 if(win->lock_flag==0)   
	 {
		 switch ( lp->status1 )
		 {
			 case 2:
				lp->status1 = 0;
				
				if((COMPLETE != systems.status)&&(STOP != systems.status))  				
					draw_button( 0, 30, "停止", 1, CHINESE );
				else 
					draw_button( 0, 30, "启动", 1, CHINESE );
				draw_button( 0, 48, "维护", 0, CHINESE );
				break;
				
			 case 1:
				lp->status1 = 2;
				draw_button( 0, 48, "维护", 1, CHINESE );
				draw_button( 0, 12, "设置", 0, CHINESE );
				break;
				
			 case 0:
				lp->status1 = 1;
				
				draw_button( 0, 12, "设置", 1, CHINESE );
				if((COMPLETE != systems.status)&&(STOP != systems.status))            				
					draw_button( 0, 30, "停止", 0, CHINESE );
				else 
					draw_button( 0, 30, "启动", 0, CHINESE );
				break;
			 default: 
				break;
		 } 	 
	 }
	 else
	 {
	   win->lockOldCurfous = win->curfous;
	   win->curfous=win->lockMenu;
	   win->curfous->show();
	   
	 }

}



void topMenuDownKey(void)
{
  struct MENU *lp;
  lp=win->curfous; 
  
  if(win->lock_flag==0)  
  {  
	  switch ( lp->status1 )
	  {
		  case 0:
			lp->status1 = 2;
			if((COMPLETE != systems.status)&&(STOP != systems.status))  
			{
				draw_button( 0, 30, "停止", 0, CHINESE );
			}
			else
			{
				draw_button( 0, 30, "启动", 0, CHINESE );
			}     
			draw_button( 0, 48, "维护", 1, CHINESE );
			break;
			
		  case 1:
			lp->status1 = 0;			
			if((COMPLETE != systems.status)&&(STOP != systems.status))   
			{					
				draw_button( 0, 30, "停止", 1, CHINESE );
			}
			else
			{
				draw_button( 0, 30, "启动", 1, CHINESE );
			} 
			draw_button( 0, 12, "设置", 0, CHINESE );
			break;
		  
		  case 2:
			lp->status1 = 1;
			draw_button( 0, 12, "设置", 1, CHINESE );
			draw_button( 0, 48, "维护", 0, CHINESE );
			break;            
		  default: 
		  break;
	  }
  }
 else
 {
    win->lockOldCurfous=win->curfous;
    win->curfous=win->lockMenu;
    win->curfous->show(); 
 }
}

void topMenuOkKey(void)
{
   struct MENU *lp;
   static unsigned char flag=0;
   unsigned int grp_start_time;    //组的启动时间
   unsigned int sys_time;
   unsigned int min_time = 2880;     // 距离最近将要进行采样组所需时间
   unsigned int index;
   
   lp = win->curfous; 
   if(win->lock_flag==0)
   {  	   
	    switch(lp->status1)
		{
			case 0:
				switch ( systems.mod )
				{
					case HAND_MOVE_MODE:
						if(SAMPLING == systems.status) //采样时进入停止告警			  
					  	{ 						  
						  	win->curfous = lp->subMenu->rMenu->rMenu;
						  	win->curfous->show();						  
					  	}
						else if(PAUSE == systems.status)	//暂停状态
						{
							systems.status = SAMPLING;	// 启动采样	
						  	systems.presure = getPFromFlow( systems.manu.flow );						  
						  	startPump( systems.manu.flow );  						  
						  	draw_button( 0, 30, "停止", 1, CHINESE );
						  	draw_symbol( STAT_X, STAT_Y, 12, 12, status_icn[systems.status], 0 ); 
						}
						else		//停止状态
						{						  
						  	systems.status = SAMPLING;	// 启动采样		
							systems.used_time = 0;
							systems.used_sec = 0;	
							lastStoreNum++;		
							
							if(lastStoreNum >= STORE_NUM)
							{
								lastStoreNum = 0;
							}
							storeData.curTime.year = systems.sys_time.year;
							storeData.curTime.month = systems.sys_time.month;
							storeData.curTime.date = systems.sys_time.date;
							storeData.curTime.hour = systems.sys_time.hour;
							storeData.curTime.min = systems.sys_time.min;
							storeData.curTime.sec = systems.sys_time.sec;
							storeData.envP.f = systems.bar_pre;
							
							storeData.flow = systems.manu.flow;
							
							if(systems.manu.temp_flag)
							{
								storeData.envT.f= (float)systems.manu.temperature;
							}
							else
							{
								storeData.envT.f = systems.envir_temp;
							}
						  	systems.presure = getPFromFlow( systems.manu.flow );						  
						  	startPump( systems.manu.flow );  						  
						  	draw_button( 0, 30, "停止", 1, CHINESE );
						  	draw_symbol( STAT_X, STAT_Y, 12, 12, status_icn[systems.status], 0 );  
					  	}
						break;
						
					case TIMING_MODE:
						if(SAMPLING == systems.status)//采样时进入停止告警							
						{ 									
							win->curfous = lp->subMenu->rMenu->rMenu;
							win->curfous->show();	
						}	
						else if(PAUSE == systems.status)	//暂停状态
						{
							systems.status = systems.oldstatus;
							systems.oldstatus = systems.status;
							
							if(systems.status == SAMPLING)
							{
								startPump( systems.groups[systems.cur_group].flow ); 
							}
						}
						else		//停止状态
						{
							flag=0;                                
							for(index = 0; index < GROUP_NUM; index++)
							{
								if(1 == systems.groups[index].srb)
								{
									flag = 1;	// 已有组启用
									break;
								}
							}
						
							if(flag==1)//     有组进行采样 
							{							
								systems.used_time = 0;
								systems.used_sec = 0;
								sys_time = systems.sys_time.hour * 60;
								sys_time += systems.sys_time.min;	// 当前系统时间
								
								for ( index=0; index<10; index++ )
								{
									if ( 1 == systems.groups[index].srb )
									{
									// 索引到的组的启动时间
										grp_start_time = systems.groups[index].start_time.hour * 60;
										grp_start_time += systems.groups[index].start_time.min;
										
										if ( grp_start_time <= sys_time )    //启动时间在后一天
										{	// 索引到的组的启动时间  <= 当前系统时间
											grp_start_time += 1440;
										}
										
										if ( min_time > grp_start_time )
										{
											min_time = grp_start_time;
											systems.cur_group = index;
										}
									}
								}		
								
								systems.status = WAITING;  								
								
								draw_button( 0, 30, "停止", 1, CHINESE );
								draw_symbol( STAT_X, STAT_Y, 12, 12, status_icn[systems.status], 0 );
							}
							else// 无设置组态
							{                               
								win->curfous = lp->subMenu;//进入startwarn菜单
								win->curfous->show();
							}							
						}
						break;
						
					case CIRCLE_MODE:
						if((SAMPLING == systems.status)||(WAITING == systems.status)) //采样时进入停止告警							
						{ 									
							win->curfous = lp->subMenu->rMenu->rMenu;
							win->curfous->show();	
						}	
						else if(PAUSE == systems.status)	//暂停状态
						{
							systems.status = systems.oldstatus;
							systems.oldstatus = systems.status;
							if(systems.status == SAMPLING)
							{
								startPump( systems.cyls.flow ); 
							}
						}
						else		//停止状态
						{
							systems.status = SAMPLING;
							systems.used_time = 0;
							systems.start_sec = 0;
							lastStoreNum++;
							if(lastStoreNum >= STORE_NUM)
							{
								lastStoreNum = 0;
							}	
							
							systems.presure = getPFromFlow( systems.cyls.flow ); 
							startPump( systems.cyls.flow );
							storeData.curTime.year = systems.sys_time.year;
							storeData.curTime.month = systems.sys_time.month;
							storeData.curTime.date = systems.sys_time.date;
							storeData.curTime.hour = systems.sys_time.hour;
							storeData.curTime.min = systems.sys_time.min;
							storeData.curTime.sec = systems.sys_time.sec;
							storeData.envP.f = systems.bar_pre;		
							storeData.envT.f = systems.envir_temp;
							storeData.flow = systems.cyls.flow;      
							draw_button( 0, 30, "停止", 1, CHINESE );
                            //缺少采样图标的变化 
                            draw_symbol( STAT_X, STAT_Y, 12, 12, status_icn[systems.status], 0 );
						}
						break;
						
						
					case CAP_MODE:
						if((SAMPLING == systems.status)||(WAITING == systems.status)) //采样时进入停止告警							
						{ 									
							win->curfous = lp->subMenu->rMenu->rMenu;
							win->curfous->show();	
						}	
						else if(PAUSE == systems.status)	//暂停状态
						{
							systems.status = systems.oldstatus;
							systems.oldstatus = systems.status;
							if(systems.status == SAMPLING)
							{
								startPump( systems.constant_v.flow );
							}
						}
						else		//停止状态
						{
							systems.status = SAMPLING;

							systems.percent = 0;
							
							lastStoreNum++;
							if(lastStoreNum >= STORE_NUM)
							{
								lastStoreNum = 0;
							}
							storeData.curTime.year = systems.sys_time.year;
							storeData.curTime.month = systems.sys_time.month;
							storeData.curTime.date = systems.sys_time.date;
							storeData.curTime.hour = systems.sys_time.hour;
							storeData.curTime.min = systems.sys_time.min;
							storeData.curTime.sec = systems.sys_time.sec;
							storeData.envP.f = systems.bar_pre;		
							storeData.envT.f = systems.envir_temp;
							storeData.flow = systems.constant_v.flow;
							systems.presure = getPFromFlow( systems.constant_v.flow ); 

                             startPump( systems.constant_v.flow );
                                                       
                             draw_button( 0, 30, "停止", 1, CHINESE );
                            //缺少采样图标的变化 
                             draw_symbol( STAT_X, STAT_Y, 12, 12, status_icn[systems.status], 0 );
						}
						break;
				}
				break;
				
			case 1:
				win->curfous = lp->subMenu->rMenu;//进入allset设置
				win->curfous->show();
				break;
				
			case 2:
				win->curfous = lp->subMenu->rMenu->rMenu->rMenu;//进入维护
				win->curfous->show();
				break;
			default:
				break;
		}
    
  }//lock
  else
  {
    win->lockOldCurfous=win->curfous;
    win->curfous=win->lockMenu;
    win->curfous->show();
      
  
  }
  
}






void topMenuLeftKey(void)
{  
	if(win->lock_flag==0)
	{
   		switch ( systems.mod )
        {
          case HAND_MOVE_MODE:

               if ( systems.manu.flow > MIN_FLOW )
               {
                   systems.manu.flow -= MID_FLOW;
               }                
			   else
			   {
				   systems.manu.flow = MIN_FLOW;           
			   }
               systems.a_flow = systems.manu.flow;
			   
               if(SAMPLING == systems.status)                
               { 
				   changeFlow( systems.manu.flow );
               }   
			   
			   clr_rect( S_FLOW_X+7, S_FLOW_Y, 24, 12 );
			   
			   if ( systems.manu.flow >= 1000 )
			   {
				   print_int( S_FLOW_X+7, S_FLOW_Y, systems.manu.flow, 0 );
			   }
			   else if ( systems.manu.flow >= 100 )
			   {
				   print_int( S_FLOW_X+10,  S_FLOW_Y, systems.manu.flow, 0 );
			   }
			   else if ( systems.manu.flow > 10 )
			   {
				   print_int( S_FLOW_X+13, S_FLOW_Y, systems.manu.flow, 0 );
			   }			   
			   break;
        default:
			break;
        }   
	}
	else
	{
		win->lockOldCurfous=win->curfous;
		win->curfous=win->lockMenu;
		win->curfous->show();   
	}    
}
void topMenuRightKey(void)
{
	if(win->lock_flag==0) 
	{
		switch(systems.mod)
        {
          case HAND_MOVE_MODE:
		  	if(systems.manu.flow < MAX_FLOW)
			{
				systems.manu.flow += MID_FLOW;
			}
			else
			{
				systems.manu.flow = MAX_FLOW;
			}
			systems.a_flow = systems.manu.flow;
			
			if(SAMPLING == systems.status) 				
			{
				changeFlow(systems.manu.flow);
			}
			
			clr_rect( S_FLOW_X+7, S_FLOW_Y, 24, 12 );
			if(systems.manu.flow >= 1000)
			{
				print_int(S_FLOW_X+7 , S_FLOW_Y, systems.manu.flow, 0);
			}
			else if ( systems.manu.flow >= 100 )
			{
				print_int(S_FLOW_X+10 , S_FLOW_Y, systems.manu.flow, 0);
			}
			else if ( systems.manu.flow > 10 )
			{
                print_int(S_FLOW_X+13 , S_FLOW_Y, systems.manu.flow, 0);
			}
			break;
         default:
		 	break;
        }  
		
	}
	else
	{
		win->lockOldCurfous=win->curfous;
		win->curfous=win->lockMenu;
		win->curfous->show();   		
	}
}


void topMenuEsc(void)
{   
    
   if(win->lock_flag==0)
   {  if(systems.status!= STOP)
     {
		 if(systems.status == PAUSE)
		 {
			writeReviewData(lastStoreNum);
			writeReviewNum();
			systems.status = STOP;
			
			systems.percent = 0;
			systems.used_time = 0;
			systems.start_min = 0;
			systems.used_sec = 0;
			
			systems.dynamic_volume = 0;
			systems.dynamic_cail_volume = 0;
			
			if(systems.mod!=HAND_MOVE_MODE)
			clr_rect( 35, 50, 61, 12 );
		 }
		 else
		 {
			 systems.oldstatus = systems.status;		
			 if(getFault())
			 {
				systems.status = STOP;
				
				systems.percent = 0;
				systems.used_time = 0;
				systems.start_min = 0;
				systems.used_sec = 0;
				
				systems.dynamic_volume = 0;
				systems.dynamic_cail_volume = 0;
				
				if(systems.mod!=HAND_MOVE_MODE)
				clr_rect( 35, 50, 61, 12 );
			 }
			 else
			 {
			   systems.status = PAUSE;				 
			 }
			 stopPump();
		 }
		 systems.dynamic_flow = 0;		 
		 win->no_clr=1;
		 win->curfous->show();
    
    }
		
   }
   else
   {
     win->lockOldCurfous=win->curfous;
     win->curfous=win->lockMenu;
     win->curfous->show();
       
   }
}




void manuMenuExec(void)
{
	static unsigned char old_status = 0;    // 旧的系统当前状态
	static unsigned int old_flow = 0;       // 旧的流量
	static unsigned int old_used_time = 0;  // 旧的已经采样时间
	static unsigned int old_used_sec = 0;  // 旧的开始采样时间 
	
	static double old_vol = 0.0;
	
	press_temp_show(P_T_X,P_T_Y,systems.envir_temp,systems.bar_pre,0);
    
	if ( systems.status == SAMPLING )       // 当前工作状态为正在采样
	{
		 if ( old_flow != systems.dynamic_flow )
		{
			old_flow = (unsigned int)systems.dynamic_flow;
			flow_show( old_flow );	// 显示动态流量
		}

		if ( old_used_time != systems.used_time
				|| old_used_sec != systems.used_sec )
		{
			old_used_time = systems.used_time;
			old_used_sec = systems.used_sec;
			show_samp_time(0);
		}	
		if(old_vol != systems.dynamic_volume)
		{
			old_vol = systems.dynamic_volume;
			used_volume_show(systems.dynamic_volume,VOL_X+30,VOL_Y);
		}
	}
	else
	{
		old_flow = 0;       // 旧的流量
		old_used_time = 0;  // 旧的已经采样时间
		old_used_sec = 0;  // 旧的开始采样时间 
	}	
	
    if ( old_status != systems.status )
	{
		old_status = systems.status;
		status_show( old_status );
	}

}

unsigned int test1 = 0;

void timeMenuExec(void)
{
	static unsigned int old_dynamic_flow = 0;
	static unsigned int old_used_time = 0;
	//static unsigned int old_sec = 0;    // 旧的秒
    static unsigned int sec = 0;
	static unsigned int old_used_sec = 0;   // 旧的已采样时间的秒
	static unsigned int old_status = 0;
	static double old_vol = 0.0;

	if ( old_status != systems.status )
	{
		old_status = systems.status;
		status_show( old_status );	// 更新状态显示
	}
	test1 = systems.status;
	switch(systems.status)
	{
		case SAMPLING:
		case WAITING:
		case PAUSE:		
			if ( old_dynamic_flow != systems.dynamic_flow )
			{
				old_dynamic_flow = (unsigned int)systems.dynamic_flow;
				flow_show( old_dynamic_flow );	// 更新动态流量显示
			}
		
			if ( old_used_time != systems.used_time
					|| old_used_sec != systems.used_sec )
			{
				old_used_time = systems.used_time;
				old_used_sec = systems.used_sec;
				show_samp_time(0);
			}
			
			if(old_vol != systems.dynamic_volume)
			{
				old_vol = systems.dynamic_volume;
				used_volume_show(systems.dynamic_volume,VOL_X+30,VOL_Y);
			}						
			if (sec != systems.sys_time.sec)
			{
				sec = systems.sys_time.sec;
				groupMoveFont();		
			}
			break;
			
		case STOP:
			old_dynamic_flow = 0;    // 旧的流量
			old_used_time = 0;  // 旧的已经采样时间
			old_used_sec = 0;  // 旧的开始采样时间 
			old_vol = 0.0;
			
		default:
			press_temp_show(P_T_X,P_T_Y,systems.envir_temp,systems.bar_pre,0);
			break;
	}

}

void cylMenuExec(void)
{
	static unsigned int old_dynamic_flow = 0;
	static unsigned int old_used_time = 0;
	static unsigned int old_used_sec = 0;
	static unsigned int old_status = 0 ;
    static unsigned int old_sec = 0;
	static double old_vol = 0.0;
	

    if ( old_status != systems.status )
	{
		old_status = systems.status;
		status_show( old_status );	// 更新状态显示
	}
	
	switch(systems.status)
	{
		case SAMPLING:
		case WAITING:
		case PAUSE:		
			if ( old_dynamic_flow != systems.dynamic_flow )
			{
				old_dynamic_flow = (unsigned int)systems.dynamic_flow;
				flow_show( old_dynamic_flow );	// 更新动态流量显示
			}
		
			if ( old_used_time != systems.used_time
					|| old_used_sec != systems.used_sec )
			{
				old_used_time = systems.used_time;
				old_used_sec = systems.used_sec;
				show_samp_time(0);
			}
			
			if(old_vol != systems.dynamic_volume)
			{
				old_vol = systems.dynamic_volume;
				used_volume_show(systems.dynamic_volume,VOL_X+30,VOL_Y);
			}
			
			if ( old_sec != systems.sys_time.sec )
			{
				old_sec = systems.sys_time.sec;
				cylsMoveFont();
			}
			break;
			
		case STOP:			
			old_dynamic_flow = 0;       // 旧的流量
			old_used_time = 0;  // 旧的已经采样时间
			old_used_sec = 0;  // 旧的开始采样时间 
			old_vol = 0.0;
		default:
			press_temp_show(P_T_X,P_T_Y,systems.envir_temp,systems.bar_pre,0);
			break;
	}	
}
void capMenuExec(void)
{
	static unsigned int old_dynamic_flow = 0;
	static unsigned int old_status = 0 ;
    static unsigned int old_sec = 0;
	static double old_vol = 0.0;
	static double old_percent = 0.0;

	switch(systems.status)
	{
		case SAMPLING:
		case WAITING:	
		case PAUSE:		
			if ( old_dynamic_flow != systems.dynamic_flow )
			{
				old_dynamic_flow = (unsigned int)systems.dynamic_flow;
				flow_show( old_dynamic_flow );	// 更新动态流量显示
			}	
			
			if(old_vol != systems.dynamic_volume)
			{
				old_vol = systems.dynamic_volume;
				used_volume_show(systems.dynamic_volume,VOL_X+30,VOL_Y);
			}
			
			if(old_percent != systems.percent)
			{
				old_percent = systems.percent;
				percent_show( old_percent );	// 更新进度条显示
			}
			if(old_sec != systems.sys_time.sec)
			{
				old_sec = systems.sys_time.sec;
				cylsMoveFont();
			}			
			break;

		case STOP:			
			old_dynamic_flow = 0;       // 旧的流量
			old_percent = 0;
			old_vol = 0.0;
		default:
			press_temp_show(P_T_X,P_T_Y,systems.envir_temp,systems.bar_pre,0);
			break;
	}	
    if ( old_status != systems.status )
	{
		old_status = systems.status;
		status_show( old_status );	// 更新状态显示
	}
}
void topMenuExec()
{    
	batShow(0);
    show_time(0);
		
    switch(systems.mod)
      {
          case HAND_MOVE_MODE: 		//手动模式
		  	manuMenuExec();              
          	break;
		  
         case TIMING_MODE:			//定时模式
            timeMenuExec(); 
			break;
		  
         case CIRCLE_MODE: 			//循环模式
          	cylMenuExec();   
          	break;
		  
         case CAP_MODE: 			//循环模式
 			capMenuExec();
          	break;
         default: 
		 	break;
      }   
}

/////启动警告菜单///////////////////////////////////////////////////////////

void StartWarnMenuShow(void)
{ 
    
    clr_lcd();
    win->mLevel = 1;
    switch ( systems.mod )
      {
          case HAND_MOVE_MODE:
              {
              
              
              }
      
          break;
         case TIMING_MODE:
            {
              draw_rect( 0, 0, 128, 64 );
              draw_line( 15, 15, 112, 15 );
             
                 print_ch_str( 52, 3, "警告", 0 );
                 putchar( 82, 3, '!', 0 );
                 print_ch_str( 20, 18, "无定时组被启用", 0 );
                 putchar( 110, 18, '!', 0 );
                 print_ch_str( 22, 33, "请设置一个组", 0 );
                 draw_button( 93, 46, "退出", 1, CHINESE );
             }
 
          break;
         case CIRCLE_MODE:
             {
             
             
             
             }    
            
          break;
         case CAP_MODE:
             {
             
             
             
             }    
            
          break;
         default: break;
      }   




}

void StartWarnMenuOkKey(void)// 返回顶层菜单
{
  
    win->curfous = win->curfous->topMenu;
    win->curfous->show();

}

void StartWarnMenuEsc(void)
{

  win->curfous = win->curfous->topMenu;
  win->curfous->show();
    
}



void setTVWarnMenuShow(void)
{
    struct MENU *lp;
    lp = win->curfous;
    win->mLevel = 2;
    clr_lcd();
    draw_rect( 0, 0, 128, 64 );
    draw_line( 15, 15, 112, 15 );    
    
	print_ch_str( 40, 3, "警告", 0 );
	putchar( 70, 3, '!', 0 );
        
	print_ch_str( 15, 20, "是否要取消定时", 0 );
	print_ch_str( 15, 32, "间模式采样", 0 );
         
	
	putchar( 75, 32, '?', 0 );
	if(lp->status1==0)
	{
		draw_button( 26, 45, "确定", 1, CHINESE );
		draw_button( 73, 45, "取消", 0, CHINESE );		
	}
	else
	{
		draw_button( 26, 45, "确定", 0, CHINESE );
		draw_button( 73, 45, "取消", 1, CHINESE );
	}
  
}

void setTVWarnMenuLeftKey(void)
{
   struct MENU *lp;
   lp = win->curfous;
   if(lp->status1==0)
   {
      lp->status1=1;
     
       draw_button( 26, 45, "确定", 0, CHINESE );
       draw_button( 73, 45, "取消", 1, CHINESE );

   
   
   }
   else
   {
     lp->status1=0;
    
       draw_button( 26, 45, "确定", 1, CHINESE );
       draw_button( 73, 45, "取消", 0, CHINESE );

   }
    


}





void setTVWarnMenuOkKey(void)
{
    struct MENU *lp;
    lp = win->curfous;
    
    if(lp->status1==0)
    {
      systems.status = STOP;
      stopPump(); 
    //  systems.manu.tim_vol_status = !systems.manu.tim_vol_status;
      
      
    }
   
       
    if ( lp->topMenu )
     {
          win->curfous = lp->topMenu;
          win->curfous->show();
     }
        
    
  
     
  
}



//  手动错误提醒菜单
void setTVWrongMenuShow(void)
{
    win->mLevel = 2;
    clr_lcd();
	draw_rect( 0, 0, 128, 64 );
	draw_line( 15, 15, 112, 15 );    

	print_ch_str( 40, 3, "警告", 0 );
	putchar( 70, 3, '!', 0 );
	
	print_ch_str( 15, 20, "采样时长不能为", 0 );
	
	print_str( 99, 20 , "0!", 0 );
	print_ch_str( 15, 33, "请重新设置", 0 );
	draw_button( 85, 45, "确定", 1, CHINESE );
}

void setTVWrongMenuOkKey(void)
{
    struct MENU *lp;
    lp = win->curfous;
    if ( lp->topMenu )
    {
        win->curfous = lp->topMenu;
        win->curfous->show();
    }
}






void StopWarnMenuShow(void)
{
    static unsigned char groups=0,index=0; 
    struct MENU *lp;
	lp = win->curfous;
    win->mLevel = 1;
	clr_lcd();
	draw_rect( 0, 0, 128, 64 );
	draw_line( 15, 15, 112, 15 );

	
		print_ch_str( 40, 3, "警告", 0 );
		putchar( 70, 3, '!', 0 );
        
         switch ( systems.mod )
       {
          case HAND_MOVE_MODE:
              {
                
		        print_ch_str( 26, 18, "还未完成采样", 0 );
                print_ch_str( 20, 32, "是否停止采样", 0 );
              }
      
          break;
         case TIMING_MODE:
            {
                 groups = 0;
			     for ( index = 0; index < 10; index++ )       //找出还有几组未采样
			      {
				        if ( systems.groups[index].srb
						&& !( systems.group_status & ( 0x00000002 << ( index*2 ))))
				    {
					    groups++;
				    }
			      }
			    print_int( 34, 18, groups, 0 );
			    print_ch_str( 22, 18, "有", 0 );
			    print_ch_str( 46, 18, "组未采样", 0 );
			    putchar( 100, 18, '!', 0 );
			    print_ch_str( 30, 33, "是否停止采样", 0 );
			    putchar( 108, 33, '?', 0 );                
                
             }
 
           break;
           case CIRCLE_MODE:
             {
               
                print_ch_str( 14, 18, "有", 0 );
			    print_ch_str( 36, 18, "次循环未采样", 0 );

			    // 显示还有多少次没有采样
			    print_int( 28, 18, ( systems.cyls.cyl_num - systems.cur_cyls ), 0 );
			    print_ch_str( 20, 32, "是否停止采样", 0 );
			    putchar( 94, 32, '?', 0 );
    
             }    
			 break;
           case CAP_MODE:
             {
               
				if(systems.constant_v.cyl_num - systems.cur_cyls >0)
				{
					print_ch_str( 14, 18, "有", 0 );
					print_ch_str( 36, 18, "次采样未完成", 0 );
	
					// 显示还有多少次没有采样
					print_int( 28, 18, ( systems.constant_v.cyl_num - systems.cur_cyls ), 0 );
					print_ch_str( 20, 32, "是否停止采样", 0 );
					putchar( 94, 32, '?', 0 );
				}
				else
				{
		        	print_ch_str( 17, 18, "定容采样未完成", 0 );
                	print_ch_str( 20, 32, "是否停止采样", 0 );
				}
    
             }  
           break;
          default: break;
      }   
        
		
		switch ( lp->status1 )
		{
		case 0:
			draw_button( 15, 45, "确定", 1, CHINESE );
			draw_button( 80, 45, "取消", 0, CHINESE );
			break;
		case 1:
			draw_button( 15, 45, "确定", 0, CHINESE );
			draw_button( 80, 45, "取消", 1, CHINESE );
			break;
		default: break;
		}

    
    

}
void StopWarnMenuEsc(void)
{
   struct MENU *lp;
   lp = win->curfous;
   
       if ( lp->topMenu )
		{
		 win->curfous = lp->topMenu;
		 win->curfous->show();
		}
       
    
}

void StopWarnMenuOkKey(void)
{
   struct MENU *lp;
   lp = win->curfous;
   if(lp->status1==0)
   {
  	    writeReviewData(lastStoreNum);			
		writeReviewNum();
        systems.status = STOP;        //进入停止状态
		systems.used_time = 0;
		systems.used_sec = 0;
		systems.percent = 0;
        stopPump();
        systems.dynamic_flow = 0;
        systems.dynamic_volume = 0;
		systems.dynamic_cail_volume = 0;
   
   }
   
    if ( lp->topMenu )                //返回上一层
    {
		win->curfous = lp->topMenu;
		win->curfous->show();
    }
      
  
    
}

void StopWarnMenuLeftKey(void)
{
   struct MENU *lp;
   lp = win->curfous;  
   if(lp->status1==0)
   {
    lp->status1=1;  
    draw_button( 15, 45, "确定", 0, CHINESE );
	draw_button( 80, 45, "取消", 1, CHINESE );
   }
   else
   {
     lp->status1=0;
     draw_button( 15, 45, "确定", 1, CHINESE );
	 draw_button( 80, 45, "取消", 0, CHINESE );
   }

}


void timeViewMenuShow(void)
{
	struct MENU *lp;
	lp = win->curfous;
    win->mLevel = 1;

	clr_lcd();
	draw_rect( 0, 0, 127, 63 );
	draw_line( 15, 15, 112, 15 );

	print_str( 15, 18, "1.", 0 );
	print_str( 15, 33, "2.", 0 );

		print_ch_str( 23, 3, "请选择查看选项", 0 );
		switch ( lp->status1 )
		{
		case 0:
			fill_rect( 29, 17, 74, 14 );
			print_ch_str( 30, 18, "定时模式参数", 1 );
			print_ch_str( 30, 33, "定时模式状态", 0 );
			print_ch_str( 54, 48, "退出", 0 );
			break;
		case 1:
			print_ch_str( 30, 18, "定时模式参数", 0 );
			fill_rect( 29, 32, 74, 14 );
			print_ch_str( 30, 33, "定时模式状态", 1 );
			print_ch_str( 54, 48, "退出", 0 );
			break;
		case 2:
			print_ch_str( 30, 18, "定时模式参数", 0 );
			print_ch_str( 30, 33, "定时模式状态", 0 );
			fill_rect( 53, 47, 26, 14 );
			print_ch_str( 54, 48, "退出", 1 );
			break;
		default: break;
		}
	//}
}

void timeViewMenuUpKey(void)
{
	struct MENU *lp;
	lp = win->curfous;
	
		switch ( lp->status1 )
		{
		case 0:
			lp->status1 = 2;
			clr_rect( 29, 17, 74, 14 );
			print_ch_str( 30, 18, "定时模式参数", 0 );
			print_ch_str( 30, 33, "定时模式状态", 0 );
			fill_rect( 53, 47, 26, 14 );
			print_ch_str( 54, 48, "退出", 1 );
			break;
		case 1:
			lp->status1 = 0;
			fill_rect( 29, 17, 74, 14 );
			print_ch_str( 30, 18, "定时模式参数", 1 );
			clr_rect( 29, 32, 74, 14 );
			print_ch_str( 30, 33, "定时模式状态", 0 );
			print_ch_str( 54, 48, "退出", 0 );
			break;
		case 2:
			lp->status1 = 1;
			print_ch_str( 30, 18, "定时模式参数", 0 );
			fill_rect( 29, 32, 74, 14 );
			print_ch_str( 30, 33, "定时模式状态", 1 );
			clr_rect( 53, 47, 26, 14 );
			print_ch_str( 54, 48, "退出", 0 );
			break;
		default: break;
		}

}

void timeViewMenuDownKey(void)
{
	struct MENU *lp;
	lp = win->curfous;
	
		switch ( lp->status1 )
		{
		case 0:
			lp->status1 = 1;
			clr_rect( 29, 17, 74, 14 );
			print_ch_str( 30, 18, "定时模式参数", 0 );
			fill_rect( 29, 32, 74, 14 );
			print_ch_str( 30, 33, "定时模式状态", 1 );
			print_ch_str( 54, 48, "退出", 0 );
			break;
		case 1:
			lp->status1 = 2;
			print_ch_str( 30, 18, "定时模式参数", 0 );
			clr_rect( 29, 32, 74, 14 );
			print_ch_str( 30, 33, "定时模式状态", 0 );
			fill_rect( 53, 47, 26, 14 );
			print_ch_str( 54, 48, "退出", 1 );
			break;
		case 2:
			lp->status1 = 0;
			fill_rect( 29, 17, 74, 14 );
			print_ch_str( 30, 18, "定时模式参数", 1 );
			print_ch_str( 30, 33, "定时模式状态", 0 );
			clr_rect( 53, 47, 26, 14 );
			print_ch_str( 54, 48, "退出", 0 );
			break;
		default: break;
		}

}

void timeViewMenuOkKey(void)
{
	struct MENU *lp;
	lp = win->curfous;

    switch ( lp->status1 )
    {
    case 0:
        if ( lp->subMenu )
        {
            win->curfous = lp->subMenu;
            win->curfous->show();
        }
        break;
    case 1:
        if ( lp->subMenu )
        {
            win->curfous = lp->subMenu->rMenu;
            win->curfous->show();
        }
        break;
    case 2:
        if ( lp->topMenu )
        {
           
                win->curfous = lp->topMenu;
                win->curfous->show();
            
        }
        break;
    default: break;
    }
}

void timeViewMenuEsc(void)
{
	struct MENU *lp;
	lp = win->curfous;

    if ( lp->topMenu )
    {
        
            win->curfous = lp->topMenu;
            win->curfous->show();
        
    }
}

void timeViewMenuExec(void)
{

}






/*
 * 定时模式各组参数查看菜单
 */
void timeViewParaMenuShow(void)
{
	struct MENU *lp;
	lp = win->curfous;
    win->mLevel = 2;
	clr_lcd();
	draw_rect( 0, 0, 128, 64 );
	draw_line( 15, 13, 112, 13 );
	draw_symbol( 2, 1, 12, 12, left_smu, 0 );
	draw_symbol( 114, 1, 12, 12, right_smu, 0 );

	
		print_ch_str( 37, 1, "组", 0 );
		print_ch_str( 73, 1, "参数", 0 );

		print_int( 55, 1, lp->status1+1, 0 );	// 打印组号

		print_ch_str( 2, 14, "启动时间", 0 );
		putchar( 50, 14, ':', 0 );

		print_ch_str( 2, 26, "采样时长", 0 );
		putchar( 50, 26, ':', 0 );

		print_ch_str( 2, 38, "流量", 0 );
		putchar( 26, 38, ':', 0 );
		print_str( 56, 38, "mL/min", 0 );

		print_ch_str( 2, 50, "启用", 0 );
		putchar( 26, 50, ':', 0 );

		if ( lp->status1 <= 9 )
		{
			print_time( systems.groups[lp->status1].start_time.hour,
						systems.groups[lp->status1].start_time.min,
						56, 14, HOUR_MIN );

			print_int( 56, 26, systems.groups[lp->status1].smp_time, 0 );
			print_str( 74, 26, "min", 0 );

			print_int( 32, 38, systems.groups[lp->status1].flow, 0 );

			if ( systems.groups[lp->status1].srb )
			{
				print_ch_str( 32, 50, "已启用", 0 );
			}
			else
			{
				print_ch_str( 32, 50, "未启用", 0 );
			}
		}
		draw_button( 93, 46, "退出", 1, CHINESE );

}

void timeViewParaMenuLeftKey(void)
{
	struct MENU *lp;
	lp = win->curfous;
	if ( 0 == lp->status1 )
	{
		lp->status1 = 9;
	}
	else
	{
		lp->status1--;
	}
	clr_rect( 56, 26, 18, 12 );   // 清空采样时长显示区
	clr_rect( 32, 38, 24, 12 );   // 清空流量显示区
	clr_rect( 44, 50, 18, 12 );   // 清空启用状态显示区
	
		clr_rect( 55, 1, 12, 12 );   // 清空组号显示区
		print_int( 55, 1, lp->status1+1, 0 );
		if ( lp->status1 <= 9 )
		{
			print_time( systems.groups[lp->status1].start_time.hour,
						systems.groups[lp->status1].start_time.min,
						56, 14, HOUR_MIN );

			print_int( 56, 26, systems.groups[lp->status1].smp_time, 0 );

			print_int( 32, 38, systems.groups[lp->status1].flow, 0 );

			if ( systems.groups[lp->status1].srb )
			{
				print_ch_str( 32, 50, "已启用", 0 );
			}
			else
			{
				print_ch_str( 32, 50, "未启用", 0 );
			}
		}

}

void timeViewParaMenuRightKey(void)
{
	struct MENU *lp;
	lp = win->curfous;
	if ( 9 == lp->status1 )
	{
		lp->status1 = 0;
	}
	else
	{
		lp->status1++;
	}

	clr_rect( 56, 26, 18, 12 );   // 清空采样时长显示区
	clr_rect( 32, 38, 24, 12 );   // 清空流量显示区
	clr_rect( 44, 50, 18, 12 );   // 清空启用状态显示区
	
		clr_rect( 55, 1, 12, 12 );   // 清空组号显示区
		print_int( 55, 1, lp->status1+1, 0 );	// 打印组号
		if ( lp->status1 <= 9 )
		{
			print_time( systems.groups[lp->status1].start_time.hour,
						systems.groups[lp->status1].start_time.min,
						56, 14, HOUR_MIN );

			print_int( 56, 26, systems.groups[lp->status1].smp_time, 0 );

			print_int( 32, 38, systems.groups[lp->status1].flow, 0 );

			if ( systems.groups[lp->status1].srb )
			{
				print_ch_str( 32, 50, "已启用", 0 );
			}
			else
			{
				print_ch_str( 32, 50, "未启用", 0 );
			}
		}
}

void timeViewParaMenuOkKey(void)
{
	struct MENU *lp;
	lp = win->curfous;

    if ( lp->topMenu )
	{
			win->curfous = lp->topMenu;
            
			win->curfous->show();
	}
	
}

void timeViewParaMenuEsc(void)
{
	struct MENU *lp;
	lp = win->curfous;
	if ( lp->topMenu )
	{
		win->curfous = lp->topMenu;
		win->curfous->show();
	}
}

void timeViewParaMenuExec(void)
{

}


/*
 * 定时模式各组状态查看菜单
 */
/*void timeViewStatusMenuShow(void)
{
	struct MENU *lp;
	int group;
	lp = win->curfous;
    win->mLevel = 2;
	clr_lcd();
	draw_line( 0, 15, 127, 15 );

	
		print_ch_str( 0, 3, "组", 0 );
		print_ch_str( 30, 3, "动态流量", 0 );
		print_ch_str( 90, 3, "完成量", 0 );
		if ( STOP != systems.status )
		{
			for ( group = lp->status1; group < lp->status1+3; group++ )
			{
				if( 9 == group )
				{
					print_int( 0, 18+( group-lp->status1 )*15, group+1, 0 );
				}
				else
				{
					print_int( 6, 18+( group-lp->status1 )*15, group+1, 0 );
				}

				// 显示动态流量
				if( systems.cur_group == group
					    && ( systems.group_status
						     & ( 0x00000001 << (group*2)) ))     // group正是在采样的组
				{
				    print_float( 36, 18+(group-lp->status1)*15,
				    		systems.dynamic_flow, 2, 0 );
				}
				else
				{
				    print_float( 36, 18+(group-lp->status1)*15, 0, 2, 0 );
				}

	            if( systems.groups[group].srb ) // 判断组的状态是否启用
	            {
					if( !( systems.group_status & ( 0x00000003 << (group*2))))
					{	// 就绪中
						print_ch_str( 90, 18+(group-lp->status1)*15, "就绪", 0 );
					}
					else if((systems.group_status & ( 0x00000001 << (group*2))))
					{	// 正在采样，显示完成量
						print_int( 90, 18+(group-lp->status1)*15,
								(unsigned int)( systems.percent*100), 0 );
						putchar( 108, 18+(group-lp->status1)*15, '%', 0 );
					}
					else if(( systems.group_status & ( 0x00000002 << (group*2))))
					{
						print_ch_str( 90, 18+(group-lp->status1)*15, "已完成", 0 );
					}
	            }
	            else  // 当前组没有启用
	            {
	              print_ch_str( 90, 18+(group-lp->status1)*15, "未启用", 0 );
	            }
			}
		}
		else     // 系统处于停止状态（没有启动采样）
		{
			print_ch_str( 20, 30, "未启动采样", 0 );
			draw_button( 93, 46, "退出", 1, CHINESE );
		}
}

void timeViewStatusMenuUpKey(void)
{
	struct MENU *lp;
	int group;
	lp = win->curfous;

	if ( 0 == lp->status1 )
	{
		lp->status1 = 7;
	}
	else
	{
		lp->status1--;
	}
	clr_rect( 0, 18, 128, 46 );
	
		if ( STOP != systems.status )
		{
			for ( group = lp->status1; group < lp->status1+3; group++ )
			{
				if( 9 == group )
				{
					print_int( 0, 18+( group-lp->status1 )*15, group+1, 0 );
				}
				else
				{
					print_int( 6, 18+( group-lp->status1 )*15, group+1, 0 );
				}

				// 显示动态流量
				if( systems.cur_group == group
					    && ( systems.group_status
						     & ( 0x00000001 << (group*2)) ))     // group正是在采样的组
				{
				    print_float( 36, 18+(group-lp->status1)*15,
				    		systems.dynamic_flow, 2, 0 );
				}
				else
				{
				    print_float( 36, 18+(group-lp->status1)*15, 0, 2, 0 );
				}

	            if( systems.groups[group].srb ) // 判断组的状态是否启用
	            {
					if( !( systems.group_status & ( 0x00000003 << (group*2))))
					{	// 就绪中
						print_ch_str( 90, 18+(group-lp->status1)*15, "就绪", 0 );
					}
					else if((systems.group_status & ( 0x00000001 << (group*2))))
					{	// 正在采样，显示完成量
						print_int( 90, 18+(group-lp->status1)*15,
								(unsigned int)( systems.percent*100), 0 );
						putchar( 108, 18+(group-lp->status1)*15, '%', 0 );
					}
					else if(( systems.group_status & ( 0x00000002 << (group*2))))
					{
						print_ch_str( 90, 18+(group-lp->status1)*15, "已完成", 0 );
					}
	            }
	            else  // 当前组没有启用
	            {
	              print_ch_str( 90, 18+(group-lp->status1)*15, "未启用", 0 );
	            }
			}
		}
		else     // 系统处于停止状态（没有启动采样）
		{
			print_ch_str( 20, 30, "未启动采样", 0 );
			draw_button( 93, 46, "退出", 1, CHINESE );
		}

}

void timeViewStatusMenuDownKey(void)
{
	struct MENU *lp;
	int group;
	lp = win->curfous;

	if ( 7 == lp->status1 )
	{
		lp->status1 = 0;
	}
	else
	{
		lp->status1++;
	}
	clr_rect( 0, 18, 128, 46 );
	
		if ( STOP != systems.status )
		{
			for ( group = lp->status1; group < lp->status1+3; group++ )
			{
				if( 9 == group )
				{
					print_int( 0, 18+( group-lp->status1 )*15, group+1, 0 );
				}
				else
				{
					print_int( 6, 18+( group-lp->status1 )*15, group+1, 0 );
				}

				// 显示动态流量
				if( systems.cur_group == group
					    && ( systems.group_status
						     & ( 0x00000001 << (group*2)) ))     // group正是在采样的组
				{
				    print_float( 36, 18+(group-lp->status1)*15,
				    		systems.dynamic_flow, 2, 0 );
				}
				else
				{
				    print_float( 36, 18+(group-lp->status1)*15, 0, 2, 0 );
				}

	            if( systems.groups[group].srb ) // 判断组的状态是否启用
	            {
					if( !( systems.group_status & ( 0x00000003 << (group*2))))
					{	// 就绪中
						print_ch_str( 90, 18+(group-lp->status1)*15, "就绪", 0 );
					}
					else if((systems.group_status & ( 0x00000001 << (group*2))))
					{	// 正在采样，显示完成量
						print_int( 90, 18+(group-lp->status1)*15,
								(unsigned int)( systems.percent*100), 0 );
						putchar( 108, 18+(group-lp->status1)*15, '%', 0 );
					}
					else if(( systems.group_status & ( 0x00000002 << (group*2))))
					{
						print_ch_str( 90, 18+(group-lp->status1)*15, "已完成", 0 );
					}
	            }
	            else  // 当前组没有启用
	            {
	              print_ch_str( 90, 18+(group-lp->status1)*15, "未启用", 0 );
	            }
			}
		}
		else     // 系统处于停止状态（没有启动采样）
		{
			print_ch_str( 20, 30, "未启动采样", 0 );
			draw_button( 93, 46, "退出", 1, CHINESE );
		}

}

void timeViewStatusMenuOkKey(void)
{
	struct MENU *lp;
	lp = win->curfous;
	
	if ( lp->topMenu )
	{
		win->curfous = lp->topMenu;
		win->curfous->show();
	}
}

void timeViewStatusMenuEsc(void)
{
	struct MENU *lp;
	lp = win->curfous;
	if ( lp->topMenu )
	{
		win->curfous = lp->topMenu;
		win->curfous->show();
	}
}

void timeViewStatusMenuExec(void)
{
	int group;
	static double old_dynamic_flow = 0;
	static double old_percent = 0;
	struct MENU *lp;
	lp = win->curfous;
    
//    systems.dynamic_flow = getCurFlow();

	if ( old_dynamic_flow != systems.dynamic_flow
			|| old_percent != systems.percent )
	{
		for ( group = lp->status1; group < lp->status1+3; group++ )
		{
			// 显示动态流量
			if( systems.cur_group == group
					&& ( systems.group_status & ( 0x00000001 << (group*2)) ))
			{     // group正是在采样的组
				old_dynamic_flow = systems.dynamic_flow;
				clr_rect( 36, 18+(group-lp->status1)*15, 42, 12 );
				print_float( 36, 18+(group-lp->status1)*15,
						old_dynamic_flow, 2, 0 );
			}

			if( systems.groups[group].srb // 判断组的状态是否启用
				   && (systems.group_status & ( 0x00000001 << (group*2))) )
			{
				// 正在采样，显示完成量
				old_percent = systems.percent;
				clr_rect( 90, 18+(group-lp->status1)*15, 18, 12 );
				print_int( 90, 18+(group-lp->status1)*15,
						(unsigned int)( old_percent*100), 0 );
				putchar( 108, 18+(group-lp->status1)*15, '%', 0 );
			}
		}
	}
}
*/

/*
 * 各种设置菜单
 */
void allSetMenuShow(void)
{
	struct MENU *lp;
	int index_icn;			// 用于打印图标的索引
	lp = win->curfous;
    win->mLevel = 1;
	clr_lcd();
    
    draw_line(32,15,122,15);
    draw_rectmod();

	draw_symbol(7,4,14,27,canshu_mu,0);
	draw_symbol(7,33,14,27,shezhi_mu,0);
	
    for(index_icn=0;index_icn<4;index_icn++)
    {  
       if(index_icn==lp->status1)
        {
            draw_symbol( 37+index_icn*22-3, 23, 21,21,
						set_Mbar_mu[index_icn], 1 );
            draw_rect(37+index_icn*22-5,23-2,25,25);
        }
        else
        {
			draw_symbol( 37+index_icn*22, 19+4, 16,16,
						set_bar_mu[index_icn], 0 );
       }
    }
	print_ch_str( 53, 3, set_bar_text_chn[lp->status1], 0 );
}

/*void allSetMenuUpKey(void)
{
	struct MENU *lp;
	int before_index_icn;	// 当前焦点改变前位置
	int after_index_icn;	// 当前焦点改变后位置
	lp = win->curfous;

	before_index_icn = lp->status1;	// 当前焦点改变前位置

    if( before_index_icn < 4 )
    {   
        clr_rect(37+before_index_icn*22-5,23-6,25,25);
        draw_symbol( 37+before_index_icn*22, 19, 16,16,
        		set_bar_mu[before_index_icn], 0 );
        
    }
    else
    {   clr_rect(37+(before_index_icn-4)*22-5,43-6,25,25);
        draw_symbol( 37+(before_index_icn-4)*22, 43, 16,16,
        		set_bar_mu[before_index_icn], 0 );
    }

    
    
    
    
    
    
    
    
	if ( lp->status1 <4 )
	{
		lp->status1 += 4;	// 使焦点维持在0——5之间
        if(lp->status1>6)
        lp->status1=6;
	}
	else
	{
		lp->status1-=4;	// 改变当前焦点
        
        
	}
	after_index_icn = lp->status1;	// 当前焦点改变后位置

    // 当前焦点改变后位置,需要打印
    if( after_index_icn < 4 )
    {
    	//反白打印焦点图标
        draw_symbol( 37+after_index_icn*22-3, 23-4, 21,21,
        		set_Mbar_mu[after_index_icn], 1 );
        draw_rect(37+after_index_icn*22-5,23-6,25,25);
    }
    else
    {
        draw_symbol( 37+(after_index_icn-4)*22-3, 43-4, 21,21,
        		set_Mbar_mu[after_index_icn], 1 );
        draw_rect(37+(after_index_icn-4)*22-5,43-6,25,25);
    }

    if ( ENGLISH == systems.language )
    {
		//打印当前焦点菜单的英文文本
		print_str( 51, 2, set_bar_text[after_index_icn], 0 );
     
    }
    else
    {
		//打印当前焦点菜单的中文文本
		print_ch_str( 53, 3, set_bar_text_chn[after_index_icn], 0 );
    }  
    
    
}

void allSetMenuDownKey(void)
{
    
    
    
}
*/
void allSetMenuLeftKey(void)
{
	struct MENU *lp;
	int before_index_icn;	// 当前焦点改变前位置
	int after_index_icn;	// 当前焦点改变后位置
	lp = win->curfous;

	before_index_icn = lp->status1;	// 当前焦点改变前位置
	
	clr_rect(37+before_index_icn*22-5,23-2,25,25);
	draw_symbol( 37+before_index_icn*22, 19+4, 16,16,
        		set_bar_mu[before_index_icn], 0 );

	if ( lp->status1 == 0 )
	{
		lp->status1 = 3;	// 使焦点维持在0——5之间
	}
	else
	{
		lp->status1--;	// 改变当前焦点
	}
	after_index_icn = lp->status1;	// 当前焦点改变后位置

    // 当前焦点改变后位置,需要打印

	//反白打印焦点图标
	draw_symbol( 37+after_index_icn*22-3, 23, 21,21,
        		set_Mbar_mu[after_index_icn], 1 );
	draw_rect(37+after_index_icn*22-5,23-2,25,25);

	//打印当前焦点菜单的中文文本
	print_ch_str( 53, 3, set_bar_text_chn[after_index_icn], 0 );
}

void allSetMenuRightKey(void)
{
	struct MENU *lp;
	int before_index_icn;	// 当前焦点改变前位置
	int after_index_icn;	// 当前焦点改变后位置
	lp = win->curfous;

	before_index_icn = lp->status1;	// 当前焦点改变前位置

	clr_rect(37+before_index_icn*22-5,23-2,25,25);
	draw_symbol( 37+before_index_icn*22, 19+4, 16,16,
        		set_bar_mu[before_index_icn], 0 );
	
	if ( lp->status1 == 3 )
	{
		lp->status1 = 0;	// 使焦点维持在0——5之间
	}
	else
	{
		lp->status1++;	// 改变当前焦点
	}
	after_index_icn = lp->status1;	// 当前焦点改变后位置

	//反白打印焦点图标
	draw_symbol( 37+after_index_icn*22-3, 23, 21,21,
        		set_Mbar_mu[after_index_icn], 1 );
	draw_rect(37+(after_index_icn)*22-5,23-2,25,25);
	
	print_ch_str( 53, 3, set_bar_text_chn[after_index_icn], 0 );
}

void allSetMenuOkKey(void)
{
	struct MENU *lp;
	lp = win->curfous;

	switch ( lp->status1 )
	{
	case 0:	// 获取模式设置焦点
		if ( lp->subMenu )
		{   
            systems.old_mod = systems.mod;
			win->curfous = lp->subMenu;
			win->curfous->show();
		}
		break;
		
	case 1:	// 获取参数设置焦点
		{
            switch(systems.mod)
            { 
             case HAND_MOVE_MODE:
                {
                    if ( lp->subMenu )
		             {
			             win->curfous = lp->subMenu->subMenu;
			             win->curfous->show();
		             }
                
                }
             break;
            
             case TIMING_MODE:
                {
                   if ( lp->subMenu )
		             {
			             win->curfous = lp->subMenu->rMenu;
			             win->curfous->show();
		             }
                }
             break;
             
             case CIRCLE_MODE:
                {
                  if ( lp->subMenu )
		             {
			             win->curfous = lp->subMenu->subMenu->rMenu->rMenu;
			             win->curfous->show();
		             }
                  }
             break;
            
             case CAP_MODE:
                {
                  if ( lp->subMenu )
		             {
			             win->curfous = lp->subMenu->subMenu->rMenu->rMenu->rMenu;
			             win->curfous->show();
		             }
                  }
             break;            
            }        
        }
        
        
		break;
	case 2:	// 获取关机焦点
		if ( lp->subMenu )
		{
			win->curfous = lp->subMenu->rMenu->rMenu;
			win->curfous->show();
		}
		break;
	case 3:	// 获取系统时间设置焦点
        if ( lp->topMenu )
        {			
			win->curfous = lp->topMenu;
			win->curfous->show();
        }
		break;
	default: break;
	}
}



void allSetMenuLongKey(void)
{  
   
    struct MENU *lp;
	lp = win->curfous;
   if ( lp->subMenu&&lp->status1==3  )
   {
		 win->curfous = lp->subMenu->lMenu;
		 win->curfous->show();
   } 
}

void allSetMenuEsc(void)
{
	struct MENU *lp;
	lp = win->curfous;
	
    if ( lp->topMenu )
     {
           win->curfous = lp->topMenu;
           win->curfous->show();
     }
        
}

void allSetMenuExec(void)
{
	
}



void cacuTBMenuShow(void)
{
    struct MENU *lp;
	lp = win->curfous;
    
    
   clr_lcd();
    
   
   print_str(0,0,"TB:",0);
   print_str(64,0,"FB:",0);
   print_str(40,20,"NUM:",0);
   print_str(40,35,"SMT:",0);
   stopPump();
   lp->status2=0;
   
   
   switch(lp->status1)
   {
   case 0:
       {
         
   
         print_str(10,50,"L  set NUM   R",0);
       
       
       }
       break;
   case 1:
       {
         print_str(10,50,"L   set TB   R",0);
       
       }
       break;
   case 2:
       {
         print_str(10,50,"L BN SET R  BS",0);
   
       }
       break;
   case 3:
       {
        
        print_str(10,50, "L   SAMP_T   R",0);
       }
       break;
   default:
       break;
   
   } 
       
}

void cacuTBMenuOnUp(void)
{ 
    struct MENU *lp;
	lp = win->curfous;
 
     switch(lp->status1)
   {
   case 0:
       {
         lp->status1=3;
   
         print_str(10,50, "L   SAMP_T   R",0);
       
       
       }
       break;
   case 1:
       {  
           lp->status1=0;
           print_str(10,50,"L  set NUM   R",0);
        
       
       }
       break;
   case 2:
       {    
            lp->status1=1;
            print_str(10,50,"L   set TB   R",0); 
   
       }
       break;
   case 3:
       {
        lp->status1=2;
        print_str(10,50,"L BN SET R  BS",0);
       
        
       }
       break;
   default:
       break;
   
   } 
    
    
}
void cacuTBMenuOnDown(void)
{ 
    struct MENU *lp;
	lp = win->curfous;
    
     switch(lp->status1)
   {
   case 0:
       {
           lp->status1=1;
            print_str(10,50,"L   set TB   R",0);
        
       
       
       }
       break;
   case 1:
       {  
          lp->status1=2;
         print_str(10,50,"L BN SET R  BS",0);
          
        
       
       }
       break;
   case 2:
       {    
            lp->status1=3;
   
          print_str(10,50, "L   SAMP_T   R",0); 
         
   
        }
       break;
   case 3:
       {
         
        lp->status1=0;
        print_str(10,50,"L  set NUM   R",0);
        
       }
       break;
   default:
       break;
   
   }
    
}
void cacuTBMenuOnLeft(void)
{ 
    struct MENU *lp;
	lp = win->curfous;
    unsigned int kk;
    kk=get_SAMP_T();
   switch(lp->status1)
   {
   case 0:
       {
           if(NUM>0)
           NUM--;

       }
       break;
   case 1:
       {  
          
        if(TB0CCR1>NUM)
         TB0CCR1-=NUM;
       
       }
       break;
   case 2:
       {    
         set_PWM_MIN(TB0CCR1);
         clr_lcd(); 
   
       }
       break;
   case 3:
       {
         if(kk>1)
         {
             kk--;
            set_SAMP_T(kk);
         }
       }
       break;
   default:
       break;
   
  }
         
    
}
void cacuTBMenuOnRight(void)
{ 
    struct MENU *lp;
	lp = win->curfous;
    
    
     unsigned int kk;
    kk=get_SAMP_T();
   switch(lp->status1)
   {
   case 0:
       {
           NUM++;

       }
       break;
   case 1:
       {  
          
        if(TB0CCR1<TB0CCR0-NUM)
        TB0CCR1+=NUM;
       
       }
       break;
   case 2:
       {    
         set_PWM_START(TB0CCR1);
         clr_lcd();
       }
       break;
   case 3:
       {
         
         
             kk++;
            set_SAMP_T(kk);
         
       }
       break;
   default:
       break;
   
  }
    
}
void cacuTBMenuOnOk(void)
{
    
    struct MENU *lp;
	lp = win->curfous;
      
switch(lp->status1==1)   
{
   case 0:{
             if(lp->status2==0)
            {
              lp->status2=1;
    
                TB0CCTL1 |= OUTMOD_7;     //启动PWM1宏
    
              }
            else
             {
              lp->status2=0;
               TB0CCTL1 &= ~OUTMOD_7;     //启动PWM1宏
   
            }
   
           }
       
      break;
   case  1:
           {
           
              if(lp->status2==0)
              {
                 lp->status2=1;
    
                TB0CCTL1 |= OUTMOD_7;     //启动PWM1宏
    
              }
              else
               {
               lp->status2=0;
               TB0CCTL1 &= ~OUTMOD_7;     //启动PWM1宏
   
               }

           
           }

     break;
case 2:
    {
        if(lp->status2==0)
              {
                 lp->status2=1;
    
                 TB0CCTL1 |= OUTMOD_7;     //启动PWM1宏
    
              }
              else
               {
                 lp->status2=0;
                 TB0CCTL1 &= ~OUTMOD_7;     //启动PWM1宏
   
               }
    
    
    
    }
   break;
case 3:
    {
    
           if(lp->status2==0)
              {
                 lp->status2=1;
    
                 TB0CCTL1 |= OUTMOD_7;     //启动PWM1宏
    
              }
              else
               {
                 lp->status2=0;
                 TB0CCTL1 &= ~OUTMOD_7;     //启动PWM1宏
   
               }
    
    }
    break;
   default:
    break;



}  
  
}
void cacuTBMenuOnEsc(void)
{
   struct MENU *lp;
	lp = win->curfous;
    
  if ( lp->topMenu )
     {
           win->curfous = lp->topMenu;
           win->curfous->show();
     }
  TB0CCTL1 &= ~OUTMOD_7;  
    
    
}
void cacuTBMenuExec(void)
{
    struct MENU *lp;
	lp = win->curfous;
  clr_rect(20,0,30,12);
  print_int(20,0,TBCCR1,0);
  clr_rect(64+18,0,30,12);
  print_int(64+18,0,getPress(),0);
  clr_rect(64,20,30,12);
  print_int(40+24,20,NUM,0);
  clr_rect(64,35,35,12);
  print_int(40+24,35,get_SAMP_T(),0);
    
  if(lp->status2==0)
  {
   
    print_str(110,50,"P",0);
    
  }
  else
  {
   
   print_str(110,50,"S",0);
  }
   
  
  
}












/*
 * 模式设置菜单
 */
void modSetMenuShow(void)
{
	struct MENU *lp;
	lp = win->curfous;
    win->mLevel = 2;	

	clr_lcd();
    
    draw_rectmod();
    draw_symbol( 110, 3,7, 58, gundong, 0 );  
	
    switch(systems.mod)
	{
		case HAND_MOVE_MODE:
			draw_rect(87,4 ,12,12);
			draw_symbol(88,5,10,10,gou_mu,0);
			
			draw_rect(87,18,12,12);
			draw_rect(87,32,12,12);
			draw_rect(87,46,12,12);
			break;
			
		case TIMING_MODE:	
			draw_rect(87,4,12,12);
			
			draw_rect(87,18,12,12);
			draw_symbol(88,19,10,10,gou_mu,0);
			
			draw_rect(87,32,12,12);
			draw_rect(87,46,12,12);
			break;
			
		case CIRCLE_MODE:			
			draw_rect(87,4,12,12);
			draw_rect(87,18,12,12);
			
			draw_rect(87,32,12,12);
			draw_symbol(88,33,10,10,gou_mu,0);
			
			draw_rect(87,46,12,12);			
			break;
			
		case CAP_MODE:
			draw_rect(87,4,12,12);
			draw_rect(87,18,12,12);
			draw_rect(87,32,12,12);
			
			draw_rect(87,46,12,12);
			draw_symbol(88,47,10,10,gou_mu,0);
			break;
		default:
			break;
	}	
		
	draw_symbol(7,4,14,27,moshi_mu,0);
	draw_symbol(7,33,14,27,xuanxiang_mu,0);
	
	switch(lp->status1)
	{
		case 0:		
			fill_rect(110+1,3+7,5,11);						
			print_ch_str( 35, 4, "手动模式", 1);
			print_ch_str( 35, 18, "定时模式", 0);					
			print_ch_str( 35, 32, "循环模式", 0 );		
			print_ch_str( 35, 46, "定容模式", 0 );	
			break;
			
		 case 1:
		 	fill_rect(110+1,3+7+11,5,11);
			print_ch_str( 35, 4, "手动模式", 0);
			print_ch_str( 35, 18, "定时模式", 1);	
			print_ch_str( 35, 32, "循环模式", 0);
			print_ch_str( 35, 46, "定容模式", 0);				 
			break;
			
		case 2:
			fill_rect(110+1,3+7+22,5,11);
			print_ch_str( 35, 4, "手动模式", 0);
			print_ch_str( 35, 18, "定时模式", 0);					
			print_ch_str( 35, 32, "循环模式", 1);					
			print_ch_str( 35, 46, "定容模式", 0);	
			break;
            
        case 3 :
			fill_rect(110+1,3+7+33,5,11);
			print_ch_str( 35, 4, "手动模式", 0);
			print_ch_str( 35, 18, "定时模式", 0);				
			print_ch_str( 35, 32, "循环模式", 0);				
			print_ch_str( 35, 46, "定容模式", 1);	
            break;
			
		default:
			break;
	}
}

void modSetMenuUpKey(void)
{
	struct MENU *lp;

	lp = win->curfous;

	switch(lp->status1)
	{
		case 0:
			lp->status1=3;
			print_ch_str( 35,4, "手动模式", 0 );
			print_ch_str( 35,46, "定容模式", 1);  				
			clr_rect(110+1,3+7,5,11);			
			fill_rect (110+1,3+7+33,5,11);
			break;
			
		case 1:
			lp->status1=0;
			print_ch_str( 35,4, "手动模式", 1 );
			print_ch_str( 35, 18, "定时模式", 0 );	
			fill_rect(110+1,3+7,5,11);
			clr_rect (110+1,3+7+11,5,11);
			break;
			
         case 2:
		 	lp->status1=1;			
			print_ch_str( 35, 18, "定时模式", 1 );				
			print_ch_str( 35, 32, "循环模式", 0 );		
			fill_rect (110+1,3+7+11,5,11);
			clr_rect (110+1,3+7+22,5,11);
			break;
			
         case 3:
		 	lp->status1=2;
			print_ch_str( 35, 32, "循环模式", 1 );		
			print_ch_str( 35, 46, "定容模式", 0 );
			clr_rect(110+1,3+7+33,5,11);
			fill_rect (110+1,3+7+22,5,11);
			break;
		default:
	        break;        
	}
}

void modSetMenuDownKey(void)
{
	struct MENU *lp;
	
	lp = win->curfous;

	switch(lp->status1)
	{
		case 0:
			lp->status1=1;
			print_ch_str( 35,4, "手动模式", 0 );
			print_ch_str( 35, 18, "定时模式", 1 );				
			clr_rect(110+1,3+7,5,11);
			fill_rect (110+1,3+7+11,5,11);            
			break;
			
         case 1:
		 	lp->status1=2;			
			print_ch_str( 35, 18, "定时模式", 0 );			
			print_ch_str( 35, 32, "循环模式", 1 );	
			clr_rect (110+1,3+7+11,5,11);
			fill_rect (110+1,3+7+22,5,11);
			break;
			
         case 2:
		 	lp->status1=3;			
			print_ch_str( 35, 32, "循环模式", 0 );				
			print_ch_str( 35, 46, "定容模式", 1); //打印取消按钮			
			clr_rect (110+1,3+7+22,5,11);
			fill_rect (110+1,3+7+33,5,11);
			break;
			
         case 3:
		 	lp->status1=0;
			print_ch_str( 35, 4, "手动模式", 1 );
			print_ch_str( 35, 46, "定容模式", 0); //打印取消按钮	
			fill_rect(110+1,3+7,5,11);
			clr_rect(110+1,3+7+33,5,11);
			break;
			
		default:
        	break;
	}  
}

void modSetMenuOkKey(void)
{
	struct MENU *lp;
	lp = win->curfous;
    
    switch(lp->status1)
    {
     case 0:
	 
           if(systems.mod==HAND_MOVE_MODE)
           {
               if ( lp->subMenu )
               {
                 win->curfous = lp->subMenu;//手动设置
                 win->curfous->show();
               }    
           }
           else
           {           
			  clr_rect(88,33 ,10,10);
              clr_rect(88,19,10,10);
			  clr_rect(88,47,10,10);
              draw_rect(87,4 ,12,12);
              draw_symbol(88,5,10,10,gou_mu,0);
              clr_rect(88,19,10,10);
              clr_rect(88,33,10,10);
              systems.mod = HAND_MOVE_MODE;
			  eeprom_WriteMod(systems.mod);
              systems.status = STOP;
		      systems.percent = 0;
		      systems.used_time = 0;     
              stopPump();
              systems.dynamic_flow = 0;
           
           }
     break;
	 
    case 1:
           if(systems.mod==TIMING_MODE)
           {
               if ( lp->rMenu )
               {
                 win->curfous = lp->rMenu;//dignshi she zhi
                 win->curfous->show();
               }    
           }
           else
           {
			  clr_rect(88,33 ,10,10);
			  clr_rect(88,47,10,10);
              clr_rect(88,5 ,10,10);
              draw_rect(87,18,12,12);
              draw_symbol(88,19,10,10,gou_mu,0);
              clr_rect(88,33,10,10);
              systems.mod = TIMING_MODE;
			  eeprom_WriteMod(systems.mod);
              systems.status = STOP;
		      systems.used_time = 0;     
              stopPump();
              systems.dynamic_flow = 0;
           
           }
       break;
    case 2:
          if(systems.mod==CIRCLE_MODE)
           {
               if ( lp->subMenu )
               {
                 win->curfous = lp->subMenu->rMenu->rMenu; //xunhuan she zhi 
                 win->curfous->show();
               }    
           }
           else
           {           
			  clr_rect(88,5 ,10,10);
              clr_rect(88,19,10,10);
			  clr_rect(88,47,10,10);
              draw_rect(87,32,12,12);
              draw_symbol(88,33,10,10,gou_mu,0);
              
              systems.mod = CIRCLE_MODE;
			  eeprom_WriteMod(systems.mod);
              systems.status = STOP;
		      systems.used_time = 0;     
              stopPump();
              systems.dynamic_flow = 0;
           
           }
        break;
    case 3:
          if(systems.mod==CAP_MODE)
           {
               if ( lp->subMenu )
               {
                 win->curfous = lp->subMenu->rMenu->rMenu->rMenu; //xunhuan she zhi 
                 win->curfous->show();
               }    
           }
           else
           {
			  clr_rect(88,5 ,10,10);
			  clr_rect(88,19,10,10);
              clr_rect(88,33,10,10);
              draw_rect(87,46,12,12);
              draw_symbol(88,47,10,10,gou_mu,0);
              
              systems.mod = CAP_MODE;
			  eeprom_WriteMod(systems.mod);
              systems.status = STOP;
		      systems.used_time = 0;     
              stopPump();
              systems.dynamic_flow = 0;
           
           }
      break;
        
    default:
      break;  
        

    }
    
    
	
	
}

void modSetMenuEsc(void)
{
	struct MENU *lp;
	lp = win->curfous;
	
	if ( lp->topMenu )
	{
		win->curfous = lp->topMenu;
		win->curfous->show();
	}
}
/*
void modSetMenuExec(void)
{
	static unsigned int old_sec = 0;    // 旧的秒

	switch ( systems.status )
	{
	case SAMPLING:
		if ( old_sec != systems.sys_time.sec )
		{
			old_sec = systems.sys_time.sec;
			systems.used_sec++;     //工作时间加一秒
			
			if ( systems.used_sec == 60 )     //计满一分钟
			{
				systems.used_sec = 0;
				systems.used_time++;       //已采样时间加1
			}
		}
		break;

	case WAITING:
		if ( old_sec != systems.sys_time.sec )
		{
			old_sec = systems.sys_time.sec;
			if ( systems.wait_sec == 0 )
			{
				systems.wait_sec = 59;
				if ( systems.wait_time == 0 )
				{
					systems.wait_sec = 0;
					systems.status = SAMPLING;         // 进入采样状态
					//start_flu( systems.cyls.flow );    // 启动采样
				}
				else
				{
					systems.wait_time--;       //等待时间自减1
				}
			}
			else
			{
				systems.wait_sec--;
			}
        }
		break;
	}
}

 
*/




/*
 * 手动模式流量设置菜单
 */
void manuParaSetMenuShow(void)
{
	struct MENU *lp;
	
	lp = win->curfous;	
	lp->status1 = 0;
    win->mLevel=2;
	
	systems.a_flow = systems.manu.flow;	// 设置流量 默认值
	systems.a_hour = systems.manu.smp_time / 60;
	systems.a_min = systems.manu.smp_time % 60;
	
	flowSetShow(lp);	// 显示设置流量

}

void manuParaSetMenuOnUp(void)
{
	struct MENU *lp;
	lp = win->curfous;

	flowSetUp( lp );	// 设置流量值
	switch(lp->status1)
	{
		case 0:
			lp->status1 = 3;
			clr_rect(32,4,24,12);
			print_ch_str(32,4,"流量",0);	
			print_ch_str(32,46,"输入确认",1);
			break;
			
		case 1:
			lp->status1 = 0;
			clr_rect(32,18,24,12);			
			print_ch_str(32,4,"流量",1);
			print_ch_str(32,18,"时长",0);
			break;
			
		case 2:
			lp->status1 = 1;
			clr_rect(32,32,24,12);
			print_ch_str(32,18,"时长",1);
			print_ch_str(32,32,"温度输入",0);	
			//draw_button(  43-3, 43, "确定", 0, CHINESE );  //打印确定按钮
			break;
	
		case 3:
			lp->status1 = 2;
			print_ch_str(32,32,"温度输入",1);	
			print_ch_str(32,46,"输入确定",0);
			break;
		default:
			break;
	}
	
}

void manuParaSetMenuOnDown(void)
{
	struct MENU *lp;
	lp = win->curfous;

	flowSetDown( lp );	// 设置流量值
	switch(lp->status1)
	{
		case 0:
			lp->status1 = 1;
			clr_rect(32,4,24,12);
			print_ch_str(32,4,"流量",0);	
			print_ch_str(32,18,"时长",1);	
			break;
			
		case 1:
			lp->status1 = 2;
			clr_rect(32,18,24,12);
			print_ch_str(32,18,"时长",0);	
			print_ch_str(32,32,"温度输入",1);				
			break;
	
		case 2:
			lp->status1 = 3;
			clr_rect(32,32,24,12);			
			print_ch_str(32,32,"温度输入",0);	
			print_ch_str(32,46,"输入确认",1);	
			break;
			
		case 3:
			lp->status1 = 0;
			print_ch_str(32,4,"流量",1);
			print_ch_str(32,46,"输入确认",0);
			break;
		default:
			break;
	}
}

void manuParaSetMenuOnLeft(void)
{
	struct MENU *lp;
	
	lp = win->curfous;

	flowSetLeft( lp );	// 设置流量值
}

void manuParaSetMenuOnRight(void)
{
	struct MENU *lp;
	lp = win->curfous;

	flowSetRight( lp );	// 设置流量值
}

void manuParaSetMenuOnOk(void)
{
	struct MENU *lp;
	
	lp = win->curfous;
	
	flowSetOk(lp);  
    
}

void manuParaSetMenuOnEsc(void)
{
	struct MENU *lp;
	
	lp = win->curfous;

	//systems.a_flow = systems.manu.flow;	// 取消设置流量
	systems.manu.flow = systems.a_flow;	// 取消设置流量
	systems.manu.temperature = systems.a_temp;
	systems.manu.temp_flag = systems.set_temp_flag;
	systems.manu.smp_time = systems.a_hour*60 + systems.a_min;
	eeprom_WriteManuSetPara(systems.manu);
	
	if ( lp->topMenu )
	{
		
			win->curfous = lp->topMenu->topMenu;
			win->curfous->show();
		
	}
}

void manuParaSetMenuExec(void)
{
   
    
}
















/*
 * 语言设置菜单
 *//*
void languageSetMenuShow(void)
{
	struct MENU *lp;
	lp = win->curfous;
	clr_lcd();
	
    
    draw_rectmod();
    draw_symbol( 117, 3,7, 58, gundong, 0 );  //右三角 
    
    draw_rect(87,5,14,14);
    draw_rect(87,25,14,14);
    
   
	
    
	if ( ENGLISH == systems.language )	// 当前语言为英文
	{
        draw_symbol(3,8,22,16,yuyan_eu,0);
        draw_symbol(3,40,22,16,shezhi_eu,0);
        draw_symbol( 89, 28, 10, 10, gou_mu, 0 );
        
		switch ( lp->status1 )
		{
		  case 0:
                   
             fill_rect(117+1,3+7,5,15);
           //  fill_rect(117+1,3+7+15,5,15);
           //  fill_rect(117+1,3+7+30,5,15);
           
            print_str(37, 6, "Chinese", 1 );   //打印选择栏处
			print_str(37, 26, "English", 0 );   //打印选择栏处
           
			
			draw_button( 38, 44, "Esc", 0, ENGLISH );    //打印Esc按钮
			break;
		  case 1:
            //  fill_rect(117+1,3+7,5,15);
             fill_rect(117+1,3+7+15,5,15);
           //  fill_rect(117+1,3+7+30,5,15);
			  print_str(37, 6, "Chinese", 0 );   //打印选择栏处
			print_str(37, 26, "English", 1 );   //打印选择栏处
			
			draw_button( 38, 44, "Esc", 0, ENGLISH );    //打印Esc按钮
			break;
		  case 2:
             //fill_rect(117+1,3+7,5,15);
            // fill_rect(117+1,3+7+15,5,15);
             fill_rect(117+1,3+7+30,5,15);
			  print_str(37, 6, "Chinese", 0 );   //打印选择栏处
			print_str(35, 26, "English", 0 );   //打印选择栏处
			
			draw_button( 38, 44, "Esc", 1, ENGLISH );    //打印Esc按钮
			break;
		default: break;
		}
	}
	else	// 否则当前语言为中文
	{
        
        draw_symbol(7,4,14,27,yuyan_mu,0);
         draw_symbol(7,33,14,27,shezhi_mu,0);
        draw_symbol( 89, 8, 10, 10, gou_mu, 0 ); 
		
		switch ( lp->status1 )
		{
		case 0:
           fill_rect(117+1,3+7,5,15);
           //  fill_rect(117+1,3+7+15,5,15);
            // fill_rect(117+1,3+7+30,5,15);
			draw_symbol(37,4,35,18,zhongwen_mu, 1 );   //打印选择栏处
			draw_symbol(37,23,35,18,yingwen_mu, 0 );   // 打印选择栏处
            draw_button( 38, 44, "退出", 0, CHINESE );    //打印取消按钮
			break;
		case 1:
         // fill_rect(117+1,3+7,5,15);
             fill_rect(117+1,3+7+15,5,15);
          //   fill_rect(117+1,3+7+30,5,15);
			draw_symbol(37,4,35,18,zhongwen_mu, 0 );   //打印选择栏处
			draw_symbol(37,23,35,18,yingwen_mu, 1 );   // 打印选择栏处
			draw_button( 38, 44, "退出", 0, CHINESE );    //打印取消按钮
			break;
		case 2:
           //fill_rect(117+1,3+7,5,15);
           //  fill_rect(117+1,3+7+15,5,15);
             fill_rect(117+1,3+7+30,5,15);
			draw_symbol(37,4,35,18,zhongwen_mu, 0 );   //打印选择栏处
			draw_symbol(37,23,35,18,yingwen_mu, 0 );   // 打印选择栏处
			draw_button(  38, 44, "退出", 1, CHINESE );    //打印取消按钮
			break;
		default: break;
		}
	}
}

void languageSetMenuOnUp(void)
{
	struct MENU *lp;
	lp = win->curfous;

	if( ENGLISH == systems.language )	// 当前语言为英文
	{
		
        
		switch ( lp->status1 )
		{
		case 0:
			lp->status1 = 2;
             clr_rect(117+1,3+7,5,15);
             //fill_rect(117+1,3+7+15,5,15);
             fill_rect(117+1,3+7+30,5,15);
            
            print_str(37, 6, "Chinese", 0 );   //打印选择栏处
			print_str(37, 26, "English", 0 );   //打印选择栏处
		    draw_button( 38, 44, "Esc", 1, ENGLISH );    //打印Esc按钮
			break;
		case 1:
            fill_rect(117+1,3+7,5,15);
             clr_rect(117+1,3+7+15,5,15);
             
			lp->status1 = 0;
           print_str(37, 6, "Chinese", 1 );   //打印选择栏处
			print_str(37, 26, "English", 0 );   //打印选择栏处
			
			draw_button( 38, 44, "Esc", 0, ENGLISH );    //打印Esc按钮
           
			break;
		case 2:
            //fill_rect(117+1,3+7,5,15);
             fill_rect(117+1,3+7+15,5,15);
             clr_rect(117+1,3+7+30,5,14);
			lp->status1 = 1;
			print_str(37, 6, "Chinese", 0 );   //打印选择栏处
			print_str(37, 26, "English", 1 );   //打印选择栏 
			draw_button( 38, 44, "Esc", 0, ENGLISH );    //打印Esc按钮
			break;
		default: break;
		}
        
        
        
        
	}
	else	// 否则当前语言为中文
	{
		
		switch ( lp->status1 )
		{
		case 0:
            clr_rect(117+1,3+7,5,15);
            // fill_rect(117+1,3+7+15,5,15);
             fill_rect(117+1,3+7+30,5,15);
			lp->status1 = 2;
            
            draw_symbol(37,4,35,18,zhongwen_mu, 0 );   // 打印选择栏处
            
			draw_button(  38, 44, "退出", 1, CHINESE );    //打印取消按钮
			break;
		case 1:
            fill_rect(117+1,3+7,5,15);
             clr_rect(117+1,3+7+15,5,15);
            // fill_rect(117+1,3+7+30,5,15);
			lp->status1 = 0;
            draw_symbol(37,4,35,18,zhongwen_mu, 1 );   // 打印选择栏处
          
            draw_symbol(37,23,35,18,yingwen_mu, 0 );   // 打印选择栏处
          
           
			break;
		case 2:
            //fill_rect(117+1,3+7,5,15);
             fill_rect(117+1,3+7+15,5,15);
             clr_rect(117+1,3+7+30,5,14);
			lp->status1 = 1;
			
            draw_symbol(37,23,35,18,yingwen_mu, 1 );   // 打印选择栏处
			draw_button(  38, 44, "退出", 0, CHINESE );    //打印取消按钮
			break;
		default: break;
		}
	}
}

void languageSetMenuOnDown(void)
{
	struct MENU *lp;
	lp = win->curfous;

	if( ENGLISH == systems.language )	// 当前语言为英文
	{
		switch ( lp->status1 )
		{
		case 0:
            clr_rect(117+1,3+7,5,15);
            fill_rect(117+1,3+7+15,5,15);
            // fill_rect(117+1,3+7+30,5,15); 
			lp->status1 = 1;
			 print_str(37, 6, "Chinese", 0 );   //打印选择栏处
			 print_str(37, 26, "English", 1 );   //打印选择栏处
			 
			 draw_button( 38, 44, "Esc", 0, ENGLISH );    //打印Esc按钮
			break;
		case 1:
			lp->status1 = 2;
             //fill_rect(117+1,3+7,5,15);
             clr_rect(117+1,3+7+15,5,15);
             fill_rect(117+1,3+7+30,5,15);
             
		    print_str(37, 6, "Chinese", 0 );   //打印选择栏处
			print_str(37, 26, "English", 0 );   //打印选择栏处
			draw_button( 38, 44, "Esc", 1, ENGLISH );    //打印Esc按钮
			break;
		case 2:
             fill_rect(117+1,3+7,5,15);
             //fill_rect(117+1,3+7+15,5,15);
             clr_rect(117+1,3+7+30,5,14);
			lp->status1 = 0;
			 print_str(37, 6, "Chinese", 1 );   //打印选择栏处
			 print_str(37, 26, "English", 0 );   //打印选择栏处
			 draw_button( 38, 44, "Esc", 0, ENGLISH );    //打印Esc按钮
			break;
		default: break;
        
        }
    
    
		
	}
	else	// 否则当前语言为中文
	{
		
		switch ( lp->status1 )
		{
		case 0:
			lp->status1 = 1;
            clr_rect(117+1,3+7,5,15);
            fill_rect(117+1,3+7+15,5,15);
             
			draw_symbol(37,4,35,18,zhongwen_mu, 0);   // 打印选择栏处
            draw_symbol(37,23,35,18,yingwen_mu, 1 );   // 打印选择栏处
			draw_button(  38, 44, "退出", 0, CHINESE );    //打印取消按钮
			break;
		case 1:
             clr_rect(117+1,3+7+15,5,15);
             fill_rect(117+1,3+7+30,5,15);
			lp->status1 = 2;
			draw_symbol(37,4,35,18,zhongwen_mu, 0 );   // 打印选择栏处
            draw_symbol(37,23,35,18,yingwen_mu, 0 );   // 打印选择栏处
			draw_button(  38, 44, "退出", 1, CHINESE );    //打印取消按钮
			break;
		case 2:
             fill_rect(117+1,3+7,5,15);
             clr_rect(117+1,3+7+30,5,14);
			lp->status1 = 0;
			draw_symbol(37,4,35,18,zhongwen_mu, 1 );   // 打印选择栏处
           draw_symbol(37,23,35,18,yingwen_mu, 0 );   // 打印选择栏处
			draw_button(  38, 44, "退出", 0, CHINESE );    //打印取消按钮
			break;
		default: break;
		}
	}
}

void languageSetMenuOnLeft(void)
{
}

void languageSetMenuOnRight(void)
{
	
}

void languageSetMenuOnOk(void)
{
	struct MENU *lp;
	lp = win->curfous;
	
	switch(lp->status1)
    {
     case 0:
        
         if(systems.language==ENGLISH)
         { 
            clr_rect(89,28,10,10);
           draw_symbol( 89, 8, 10, 10, gou_mu, 0 );
           systems.language= CHINESE;
           win->curfous->show();
         }
        break;
     case 1:
        
        if(systems.language==CHINESE)
        { 
           clr_rect(89,8,10,10);
           draw_symbol( 89, 28, 10, 10, gou_mu, 0 );
          systems.language= ENGLISH;
          win->curfous->show();
          
        } 
        break;
     case 2:
        if ( lp->topMenu )
	    {
		  win->curfous = lp->topMenu;          
		  win->curfous->show();          
	    }
 
        break;
    
    
    
    }
    
    
    
}

void languageSetMenuOnEsc(void)
{
	struct MENU *lp;
	lp = win->curfous;

	if ( lp->topMenu )
	{
		win->curfous = lp->topMenu;
		win->curfous->show();
	}
}

void languageSetMenuExec(void)
{

}
*/
//=========================================================================
//函数名称：int checkTime(char year, char month, char data )
//输入参数：年月
//输出参数：天数
//功能描述：检测年月日是否正确
//其它说明：        
//==========================================================================
char checkTime(char year, char month )
{
    if(month > 0 && month < 13)
    {
        switch(month)
        {
            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 12:
                return  31;
                //break;
                
            case 2:
                if(year % 4 == 0 && year % 100 != 0) // 判断是否为闰年
                {
                    return 29;
                }
                else
                {
                    return 28;
                }
                
            case 4:
            case 6:
            case 9:
            case 11:
                return 30;
                
            default:
                return 0;
        }
    }
    else
    {
        return 0;
    }
}

/*
 * 系统时间设置菜单
 */
void sysTimeSetMenuShow(void)
{
	struct MENU *lp;
	lp = win->curfous;
	win->mLevel = 2;
	clr_lcd();
	
    
    draw_line(32,15,122,15);
    draw_rectmod();
	
	systems.temp_year = systems.sys_time.year;
	systems.temp_month = systems.sys_time.month;
	systems.temp_day = systems.sys_time.date;
	systems.temp_hour = systems.sys_time.hour;
	systems.temp_min = systems.sys_time.min;
	systems.temp_sec = systems.sys_time.sec;
	
	draw_symbol(7,4,14,27,xitong_mu,0);
	draw_symbol(7,33,14,27,shijian_mu,0);
	print_ch_str( 42, 3, "设置系统时间", 0 );
				 
	switch(lp->status1)
    {
        case 0:
            print_int(34,18+2,systems.temp_year + 2000,1);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000
            //显示年
            print_ch_str(58,18+2,"年",0);
            //显示月
            if(systems.temp_month >= 10)
            {
                print_int(71,18+2,systems.temp_month ,0);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000
            }
            else
            {
                putchar(71,18+2,'0',0);
                print_int(77,18+2,systems.temp_month ,0);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000
            }
            print_ch_str(84,18+2,"月",0);
            //显示日            
            if(systems.temp_day >= 10)
            {
                print_int(97,18+2,systems.temp_day ,0);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000           
            }
            else
            {
                putchar(97,18+2,'0',0);
                print_int(103,18+2,systems.temp_day ,0);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000
            }
            print_ch_str(109,18+2,"日",0);
            //显示时
            if(systems.temp_hour >= 10)
            {
                print_int(50,30+2,systems.temp_hour ,0);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000
            }
            else
            {
                putchar(50,30+2,'0',0);
                print_int(56,30+2,systems.temp_hour,0);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000
            }
            putchar(62,30+2,':',0);
            //显示分            
            if(systems.temp_min >= 10)
            {
                print_int(68,30+2,systems.temp_min ,0);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000           
            }
            else
            {
                putchar(68,30+2,'0',0);
                print_int(74,30+2,systems.temp_min ,0);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000
            }
            putchar(80,30+2,':',0);
            //显示秒
            if(systems.temp_sec >= 10)
            {
                print_int(86,30+2,systems.temp_sec ,0);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000           
            }
            else
            {
                putchar(86,30+2,'0',0);
                print_int(92,30+2,systems.temp_sec ,0);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000
            }  
            //draw_button32(52,42,"确定",0,1);    
            break;          
            
        case 1:
           print_int(34,18+2,systems.temp_year + 2000,0);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000
            //显示年
            print_ch_str(58,18+2,"年",0);
            //显示月
            if(systems.temp_month >= 10)
            {
                print_int(71,18+2,systems.temp_month ,1);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000
            }
            else
            {
                putchar(71,18+2,'0',1);
                print_int(77,18+2,systems.temp_month ,1);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000
            }
            print_ch_str(84,18+2,"月",0);
            //显示日            
            if(systems.temp_day >= 10)
            {
                print_int(97,18+2,systems.temp_day ,0);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000           
            }
            else
            {
                putchar(97,18+2,'0',0);
                print_int(103,18+2,systems.temp_day ,0);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000
            }
            print_ch_str(109,18+2,"日",0);
            //显示时
            if(systems.temp_hour >= 10)
            {
                print_int(50,30+2,systems.temp_hour ,0);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000
            }
            else
            {
                putchar(50,30+2,'0',0);
                print_int(56,30+2,systems.temp_hour,0);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000
            }
            putchar(62,30+2,':',0);
            //显示分            
            if(systems.temp_min >= 10)
            {
                print_int(68,30+2,systems.temp_min ,0);             
            }
            else
            {
                putchar(68,30+2,'0',0);
                print_int(74,30+2,systems.temp_min ,0);  
            }
            putchar(80,30+2,':',0);
            ////显示秒   
            if(systems.temp_sec >= 10)
            {
                print_int(86,30+2,systems.temp_sec ,0);             
            }
            else
            {
                putchar(86,30+2,'0',0);
                print_int(92,30+2,systems.temp_sec ,0);  
            }  
           // draw_button32(52,42,"确定",0,1);
        break;
        
        case 2:        
           print_int(34,18+2,systems.temp_year + 2000,0);  
            //显示年
            print_ch_str(58,18+2,"年",0);
            //显示月
            if(systems.temp_month >= 10)
            {
                print_int(71,18+2,systems.temp_month ,0);  
            }
            else
            {
                putchar(71,18+2,'0',0);
                print_int(77,18+2,systems.temp_month ,0); 
            }
            print_ch_str(84,18+2,"月",0);
            //显示日            
            if(systems.temp_day >= 10)
            {
                print_int(97,18+2,systems.temp_day ,1);            
            }
            else
            {
                putchar(97,18+2,'0',0);
                print_int(103,18+2,systems.temp_day ,1);  
            }
            print_ch_str(109,18+2,"日",0);
            //显示时
            if(systems.temp_hour >= 10)
            {
                print_int(50,30+2,systems.temp_hour ,0);  
            }
            else
            {
                putchar(50,30+2,'0',0);
                print_int(56,30+2,systems.temp_hour,0);  
            }
            putchar(62,30+2,':',0);
            //显示分            
            if(systems.temp_min >= 10)
            {
                print_int(68,30+2,systems.temp_min ,0);           
            }
            else
            {
                putchar(68,30+2,'0',0);
                print_int(74,30+2,systems.temp_min ,0);  
            }
            putchar(80,30+2,':',0);
            //显示秒
            if(systems.temp_sec >= 10)
            {
                print_int(86,30+2,systems.temp_sec ,0);         
            }
            else
            {
                putchar(86,30+2,'0',0);
                print_int(92,30+2,systems.temp_sec ,0);  
            }  
           // draw_button32(52,42,"确定",0,1);
            break;   
            
        case 3:
           print_int(34,18+2,systems.temp_year + 2000,0);  
            //显示年
            print_ch_str(58,18+2,"年",0);
            //显示月
            if(systems.temp_month >= 10)
            {
                print_int(71,18+2,systems.temp_month ,0); 
            }
            else
            {
                putchar(71,18+2,'0',0);
                print_int(77,18+2,systems.temp_month ,0);  
            }
            print_ch_str(84,18+2,"月",0);
            //显示日            
            if(systems.temp_day >= 10)
            {
                print_int(97,18+2,systems.temp_day ,0);        
            }
            else
            {
                putchar(97,18+2,'0',0);
                print_int(103,18+2,systems.temp_day ,0);  
            }
            print_ch_str(109,18+2,"日",0);
            //显示时
            if(systems.temp_hour >= 10)
            {
                print_int(50,30+2,systems.temp_hour ,1); 
            }
            else
            {
                putchar(50,30+2,'0',1);
                print_int(56,30+2,systems.temp_hour,1);
            }
            putchar(62,30+2,':',0);
            //显示分            
            if(systems.temp_min >= 10)
            {
                print_int(68,30+2,systems.temp_min ,0);            
            }
            else
            {
                putchar(68,30+2,'0',0);
                print_int(74,30+2,systems.temp_min ,0);  
            }
            putchar(80,30+2,':',0);
            //显示秒
            if(systems.temp_sec >= 10)
            {
                print_int(86,30+2,systems.temp_sec ,0);            
            }
            else
            {
                putchar(86,30+2,'0',0);
                print_int(92,30+2,systems.temp_sec ,0);  
            } 
            //draw_button32(52,42,"确定",0,1);
        break; 
        
        case 4:
           print_int(34,18+2,systems.temp_year + 2000,0);  
            //显示年
            print_ch_str(58,18+2,"年",0);
            //显示月
            if(systems.temp_month >= 10)
            {
                print_int(71,18+2,systems.temp_month ,0);
            }
            else
            {
                putchar(71,18+2,'0',0);
                print_int(77,18+2,systems.temp_month ,0);  
            }
            print_ch_str(84,18+2,"月",0);
            //显示日            
            if(systems.temp_day >= 10)
            {
                print_int(97,18+2,systems.temp_day ,0);        
            }
            else
            {
                putchar(97,18+2,'0',0);
                print_int(103,18+2,systems.temp_day ,0);  
            }
            print_ch_str(109,18+2,"日",0);
            //显示时
            if(systems.temp_hour >= 10)
            {
                print_int(50,30+2,systems.temp_hour ,0);  
            }
            else
            {
                putchar(50,30+2,'0',0);
                print_int(56,30+2,systems.temp_hour,0); 
            }
            putchar(62,30+2,':',0);
            //显示分            
            if(systems.temp_min >= 10)
            {
                print_int(68,30+2,systems.temp_min ,1);           
            }
            else
            {
                putchar(68,30+2,'0',1);
                print_int(74,30+2,systems.temp_min ,1);  
            }
            putchar(80,30+2,':',0);
            //显示秒
            if(systems.temp_sec >= 10)
            {
                print_int(86,30+2,systems.temp_sec ,0);         
            }
            else
            {
                putchar(86,30+2,'0',0);
                print_int(92,30+2,systems.temp_sec ,0); 
            } 
            //draw_button32(52,42,"确定",0,1);
            break;
        
        case 5:
           print_int(34,18+2,systems.temp_year + 2000,0); 
            //显示年
            print_ch_str(58,18+2,"年",0);
            //显示月
            if(systems.temp_month >= 10)
            {
                print_int(71,18+2,systems.temp_month ,0);  
            }
            else
            {
                putchar(71,18+2,'0',0);
                print_int(77,18+2,systems.temp_month ,0);  
            }
            print_ch_str(84,18+2,"月",0);
            //显示日            
            if(systems.temp_day >= 10)
            {
                print_int(97,18+2,systems.temp_day ,0);           
            }
            else
            {
                putchar(97,18+2,'0',0);
                print_int(103,18+2,systems.temp_day ,0);  
            }
            print_ch_str(109,18+2,"日",0);
            //显示时
            if(systems.temp_hour >= 10)
            {
                print_int(50,30+2,systems.temp_hour ,0);  
            }
            else
            {
                putchar(50,30+2,'0',0);
                print_int(56,30+2,systems.temp_hour,0); 
            }
            putchar(62,30+2,':',0);
            //显示分            
            if(systems.temp_min >= 10)
            {
                print_int(68,30+2,systems.temp_min ,0);          
            }
            else
            {
                putchar(68,30+2,'0',0);
                print_int(74,30+2,systems.temp_min ,0); 
            }
            putchar(80,30+2,':',0);
            //显示秒
            if(systems.temp_sec >= 10)
            {
                print_int(86,30+2,systems.temp_sec ,1);        
            }
            else
            {
                putchar(86,30+2,'0',1);
                print_int(92,30+2,systems.temp_sec ,1);  
            } 
        break;        
    default:
        break;
    } 
		
	//}
}

void sysTimeSetMenuOnUp(void)
{
	struct MENU *lp;
	lp = win->curfous;
	 switch(lp->status1)
    {
        case 0:
            systems.temp_year++;
            if(systems.temp_year == 100)
            {
                systems.temp_year = 0;
            }
            
            print_int(34,18+2,systems.temp_year + 2000,1);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000        
            break;
            
        case 1:
            systems.temp_month++;
            if(systems.temp_month == 13)
            {
                systems.temp_month = 1;
            }
            if(systems.temp_month >= 10)
            {
                print_int(71,18+2,systems.temp_month ,1);  
            }
            else
            {
                putchar(71,18+2,'0',1);
                print_int(77,18+2,systems.temp_month ,1);  
            }
            break;
            
        case 2:
            systems.temp_day++;
            if((checkTime(systems.temp_year,systems.temp_month) > 0) 
               && (systems.temp_day > checkTime(systems.temp_year,systems.temp_month)))
            {
                systems.temp_day = 1;
            }  
            if(systems.temp_day >= 10)
            {
                print_int(97,18+2,systems.temp_day ,1);        
            }
            else
            {
                putchar(97,18+2,'0',1);
                print_int(103,18+2,systems.temp_day ,1);  
            }
            break;
            
        case 3:
            systems.temp_hour++;
            if(systems.temp_hour == 24)
            {
                systems.temp_hour = 0;
            }
            if(systems.temp_hour >= 10)
            {
                print_int(50,30+2,systems.temp_hour ,1);  
            }
            else
            {
                putchar(50,30+2,'0',1);
                print_int(56,30+2,systems.temp_hour,1);  
            }
            break;
            
        case 4:
            systems.temp_min++;
            if(systems.temp_min == 60)
            {
                systems.temp_min = 0;
            }
            if(systems.temp_min >= 10)
            {
                print_int(68,30+2,systems.temp_min ,1);           
            }
            else
            {
                putchar(68,30+2,'0',1);
                print_int(74,30+2,systems.temp_min ,1);  
            } 
            break;
            
        case 5:
            systems.temp_sec++;
            if(systems.temp_sec == 60)
            {
                systems.temp_sec = 0;
            }
            if(systems.temp_sec >= 10)
            {
                print_int(86,30+2,systems.temp_sec ,1);           
            }
            else
            {
                putchar(86,30+2,'0',1);
                print_int(92,30+2,systems.temp_sec ,1);  
            } 
            break; 
            
        default:   
            break;
    }
}

void sysTimeSetMenuOnDown(void)
{
	struct MENU *lp;
	lp = win->curfous;
	
	switch ( lp->status1 )
	{
        case 0:            
            if(systems.temp_year == 0)
            {
                systems.temp_year = 100;
            }
            systems.temp_year--;
            print_int(34,18+2,systems.temp_year + 2000,1);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000        
            break;
            
        case 1:
            systems.temp_month--;
            if(systems.temp_month == 0)
            {
                systems.temp_month = 12;
            }
            if(systems.temp_month >= 10)
            {
                print_int(71,18+2,systems.temp_month ,1);  
            }
            else
            {
                putchar(71,18+2,'0',1);
                print_int(77,18+2,systems.temp_month ,1);  
            }
            break;
            
        case 2:    
            systems.temp_day--;        
            if((checkTime(systems.temp_year+2000,systems.temp_month) > 0) 
               && (systems.temp_day == 0))
            {
                systems.temp_day = checkTime(systems.temp_year+2000,systems.temp_month);
            }  
            
            if(systems.temp_day >= 10)
            {
                print_int(97,18+2,systems.temp_day ,1);        
            }
            else
            {
                putchar(97,18+2,'0',1);
                print_int(103,18+2,systems.temp_day ,1);  
            }
            break;
            
        case 3:
            if(systems.temp_hour == 0)
            {
                systems.temp_hour = 24;
            }
            systems.temp_hour--;
            if(systems.temp_hour >= 10)
            {
                print_int(50,30+2,systems.temp_hour ,1);  
            }
            else
            {
                putchar(50,30+2,'0',1);
                print_int(56,30+2,systems.temp_hour,1);  
            }
            break;
            
        case 4:
            if(systems.temp_min == 0)
            {
                systems.temp_min = 60;
            }
            systems.temp_min--;
            if(systems.temp_min >= 10)
            {
                print_int(68,30+2,systems.temp_min ,1);           
            }
            else
            {
                putchar(68,30+2,'0',1);
                print_int(74,30+2,systems.temp_min ,1);  
            } 
            break;
            
        case 5:
            if(systems.temp_sec == 0)
            {
                systems.temp_sec = 60;
            }
            systems.temp_sec--;
            
            if(systems.temp_sec >= 10)
            {
                print_int(86,30+2,systems.temp_sec ,1);           
            }
            else
            {
                putchar(86,30+2,'0',1);
                print_int(92,30+2,systems.temp_sec ,1);  
            } 
            break; 
            
        default:   
            break;
	}
}

void sysTimeSetMenuOnLeft(void)
{
	struct MENU *lp;
	lp = win->curfous;
	switch ( lp->status1 )
	{
        case 0:
           lp->status1 = 5;       //ok      
           print_int(34,18+2,systems.temp_year + 2000,0);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000 
                       
           if(systems.temp_sec >= 10)
            {
                print_int(86,30+2,systems.temp_sec ,1);             
            }
            else
            {
                putchar(86,30+2,'0',1);
                print_int(92,30+2,systems.temp_sec ,1);  
            } 
           //draw_button32(52,42,"确定",1,1);
           break;

        case 1:
           lp->status1 = 0;           //OK
           print_int(34,18+2,systems.temp_year + 2000,1);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000 
            if(systems.temp_month >= 10)
            {
                print_int(71,18+2,systems.temp_month ,0);  
            }
            else
            {
                putchar(71,18+2,'0',0);
                print_int(77,18+2,systems.temp_month ,0);  
            }
           
           break;
           
        case 2:
           lp->status1 = 1;       //OK
            if(systems.temp_month >= 10)
            {
                print_int(71,18+2,systems.temp_month ,1);  
            }
            else
            {
                putchar(71,18+2,'0',1);
                print_int(77,18+2,systems.temp_month ,1);  
            }
            if(systems.temp_day >= 10)
            {
                print_int(97,18+2,systems.temp_day ,0);         
            }
            else
            {
                putchar(97,18+2,'0',0);
                print_int(103,18+2,systems.temp_day ,0);  
            }
           break;
           
        case 3:
           lp->status1 = 2;
            if(systems.temp_day >= 10)
            {
                print_int(97,18+2,systems.temp_day ,1);         
            }
            else
            {
                putchar(97,18+2,'0',1);
                print_int(103,18+2,systems.temp_day ,1);  
            }
            if(systems.temp_hour >= 10)
            {
                print_int(50,30+2,systems.temp_hour ,0);  
            }
            else
            {
                putchar(50,30+2,'0',0);
                print_int(56,30+2,systems.temp_hour,0);  
            }
           break;
           
        case 4:
           lp->status1 = 3;           
            if(systems.temp_hour >= 10)
            {
                print_int(50,30+2,systems.temp_hour ,1);  
            }
            else
            {
                putchar(50,30+2,'0',1);
                print_int(56,30+2,systems.temp_hour,1);  
            }
            
            if(systems.temp_min >= 10)
            {
                print_int(68,30+2,systems.temp_min ,0);           
            }
            else
            {
                putchar(68,30+2,'0',0);
                print_int(74,30+2,systems.temp_min ,0);  
            }         
           break;           
           
       case 5:
           lp->status1 = 4;
            if(systems.temp_sec >= 10)
            {
                print_int(86,30+2,systems.temp_sec ,0);             
            }
            else
            {
                putchar(86,30+2,'0',0);
                print_int(92,30+2,systems.temp_sec ,0);  
            }   
            
            if(systems.temp_min >= 10)
            {
                print_int(68,30+2,systems.temp_min ,1);           
            }
            else
            {
                putchar(68,30+2,'0',1);
                print_int(74,30+2,systems.temp_min ,1);  
            }   
           break;            
    default:
        break;
	}
}


void sysTimeSetMenuOnRight(void)
{
	struct MENU *lp;
	lp = win->curfous;
	switch ( lp->status1 )
	{
		case 0:
			lp->status1 = 1;
            print_int(34,18+2,systems.temp_year + 2000,0);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000 

            if(systems.temp_month >= 10)
            {
                print_int(71,18+2,systems.temp_month ,1);  
            }
            else
            {
                putchar(71,18+2,'0',1);
                print_int(77,18+2,systems.temp_month ,1);  
            }
			break;
		
		case 1:
			lp->status1 = 2;
            if(systems.temp_month >= 10)
            {
                print_int(71,18+2,systems.temp_month ,0);  
            }
            else
            {
                putchar(71,18+2,'0',0);
                print_int(77,18+2,systems.temp_month ,0);  
            }
            if(systems.temp_day >= 10)
            {
                print_int(97,18+2,systems.temp_day ,1);         
            }
            else
            {
                putchar(97,18+2,'0',1);
                print_int(103,18+2,systems.temp_day ,1);  
            }
			break;
		
		case 2:
			lp->status1 = 3;
            if(systems.temp_day >= 10)
            {
                print_int(97,18+2,systems.temp_day ,0);         
            }
            else
            {
                putchar(97,18+2,'0',0);
                print_int(103,18+2,systems.temp_day ,0);  
            }
            if(systems.temp_hour >= 10)
            {
                print_int(50,30+2,systems.temp_hour ,1);  
            }
            else
            {
                putchar(50,30+2,'0',1);
                print_int(56,30+2,systems.temp_hour,1);  
            }
			break;
			
		case 3:
			lp->status1 = 4;
            if(systems.temp_hour >= 10)
            {
                print_int(50,30+2,systems.temp_hour ,0);  
            }
            else
            {
                putchar(50,30+2,'0',0);
                print_int(56,30+2,systems.temp_hour,0);  
            }
            if(systems.temp_min >= 10)
            {
                print_int(68,30+2,systems.temp_min ,1);           
            }
            else
            {
                putchar(68,30+2,'0',1);
                print_int(74,30+2,systems.temp_min ,1);  
            } 
			break;
		
		case 4:
			lp->status1 = 5;
            if(systems.temp_min >= 10)
            {
                print_int(68,30+2,systems.temp_min ,0);           
            }
            else
            {
                putchar(68,30+2,'0',0);
                print_int(74,30+2,systems.temp_min ,0);  
            }  
           if(systems.temp_sec >= 10)
            {
                print_int(86,30+2,systems.temp_sec ,1);             
            }
            else
            {
                putchar(86,30+2,'0',1);
                print_int(92,30+2,systems.temp_sec ,1);  
            } 
			break;

		case 5:
			lp->status1 = 0;
           if(systems.temp_sec >= 10)
            {
                print_int(86,30+2,systems.temp_sec ,0);             
            }
            else
            {
                putchar(86,30+2,'0',0);
                print_int(92,30+2,systems.temp_sec ,0);  
            } 
            print_int(34,18+2,systems.temp_year + 2000,1);  //因为从实时时钟芯片里获取的数据是只有年份的十位和个位，所以显示时要加2000 

			break;
		default: 
			break;
	}
}

void sysTimeSetMenuOnOk(void)
{
	struct MENU *lp;
	
	lp = win->curfous;
	
	Set_time(systems.temp_hour,systems.temp_min,systems.temp_sec,
    systems.temp_day,systems.temp_month,systems.temp_year);  //将时间信息更新到实时时钟芯片
	
   	if(lp->topMenu)
	{
		win->curfous = lp->topMenu;
		win->curfous->show();
	}
}

void sysTimeSetMenuOnEsc(void)
{
	struct MENU *lp;
	lp = win->curfous;
	if ( lp->topMenu )
	{
		win->curfous = lp->topMenu;
		win->curfous->show();
	}
}

/*void sysTimeSetMenuExec(void)
{

}
*/




/*
 * 定时关机设置菜单
 */
void timePowerOffSetMenuShow(void)
{
	struct MENU *lp;
	lp = win->curfous;
	clr_lcd();
    
	
   
    draw_rectmod();
    draw_symbol( 117, 3,7, 58, gundong, 0 );  //右三角  
    
  
     
    fill_rect(117+1,3+7+lp->status1*15,5,15);
    
 

   
    	 draw_symbol(7,4,14,27,guanji_mu,0);
         draw_symbol(7,33,14,27,shezhi_mu,0);
    	switch ( lp->status1 )
    	{
    	case 0:          //在0处，也即是在设置"关机时间"处
            fill_rect( 36, 10, 76, 14 );    //填充第一行，也即第一行的位置获得焦点
            print_ch_str( 38,11, "关机时间", 1 );
            print_ch_str( 38, 26, "启用", 0 );
            print_ch_str( 38, 42, "退出", 0 );
    		break;
    	case 1:    //在1处，也即是在"启用"位置处
    		print_ch_str( 38,11, "关机时间", 0 );
            fill_rect( 36, 25,76, 14 );    //在第二行处填充，也即第二行位置获得焦点
            print_ch_str( 38, 26, "启用", 1 );
            print_ch_str( 38, 42, "退出", 0 );
    		break;
    	case 2:   //在2处，也即是在"退出"位置处
    		print_ch_str( 38,11, "关机时间", 0 );
    		print_ch_str( 38, 26, "启用", 0 );
            fill_rect( 36, 41, 76, 14 );     //填充第三行，也即第三行位置获得焦点
            print_ch_str( 38, 42, "退出", 1 );
    		break;
    	default: break;
    	}


    power_status_show(lp);
}

void timePowerOffSetMenuOnUp(void)
{
	struct MENU *lp;
	lp = win->curfous;
	
	if(lp->status1==2) 
    {
     clr_rect(117+1,3+7+30,5,14);
    }
    else
    clr_rect(117+1,3+7+lp->status1*15,5,15);
   
    	
    	switch ( lp->status1 )
    	{   
    	  case 0:          //在0处，也即是在设置"关机时间"处
    		lp->status1 = 2;
            clr_rect( 36, 10, 76, 14 );    //填充第一行，也即第一行的位置获得焦点
          
            fill_rect( 36, 41, 76, 14 );  
    		print_ch_str( 38,11, "关机时间", 0 );
    		print_ch_str( 38, 26, "启用", 0 );
            
            print_ch_str( 38, 42, "退出", 1 );
    		break;
    	  case 1:    //在1处，也即是在"启用"位置处
    		lp->status1 = 0;
            fill_rect( 36, 10, 76, 14 );    //填充第一行，也即第一行的位置获得焦点
         
            clr_rect( 36, 25,76, 14 );    //在第二行处填充，也即第二行位置获得焦点
           
            print_ch_str( 38,11, "关机时间", 1 );
            print_ch_str( 38, 26, "启用", 0 );
            print_ch_str( 38, 42, "退出", 0 );
    		break;
    	  case 2:   //在2处，也即是在"退出"位置处
    	 	lp->status1 = 1;
    		
            fill_rect( 36, 25,76, 14 );    //在第二行处填充，也即第二行位置获得焦点
            clr_rect( 36, 41, 76, 14 );  
            print_ch_str( 38,11, "关机时间", 0 );
            print_ch_str( 38, 26, "启用", 1 );
            print_ch_str( 38, 42, "退出", 0 );
    		break;
    	  default: break;
    	}

    power_status_show(lp);
    fill_rect(117+1,3+7+lp->status1*15,5,15);
}

void timePowerOffSetMenuOnDown(void)
{
	struct MENU *lp;
	lp = win->curfous;
	if(lp->status1==2) 
    {
     clr_rect(117+1,3+7+30,5,14);
    }
    else
    clr_rect(117+1,3+7+lp->status1*15,5,15);
    
	
  
    	switch ( lp->status1 )
    	{
    	case 0:          //在0处，也即是在设置"关机时间"处
    		lp->status1 = 1;
            clr_rect( 36, 10, 76, 14 );    //填充第一行，也即第一行的位置获得焦点
            fill_rect( 36, 25,76, 14 );    //在第二行处填充，也即第二行位置获得焦点
             
    		print_ch_str( 38,11, "关机时间", 0 );
           
            print_ch_str( 38, 26, "启用", 1 );
            print_ch_str( 38, 42, "退出", 0 );
    		break;
    	case 1:    //在1处，也即是在"启用"位置处
    		lp->status1 = 2;
           
            clr_rect( 36, 25, 76, 14 );    //在第二行处填充，也即第二行位置获得焦点
            fill_rect( 36, 41, 76, 14 );  
    		print_ch_str( 38,11, "关机时间", 0 );
    		print_ch_str( 38, 26, "启用", 0 );
           
            print_ch_str( 38, 42, "退出", 1 );
    		break;
    	case 2:   //在2处，也即是在"退出"位置处
    		lp->status1 = 0;
            fill_rect( 36, 10, 76, 14 );    //填充第一行，也即第一行的位置获得焦点
            clr_rect( 36, 41, 76, 14 );    
            print_ch_str( 38,11, "关机时间", 1 );
            print_ch_str( 38, 26, "启用", 0 );
            print_ch_str( 38, 42, "退出", 0 );
    		break;
    	default: break;
    	}
    power_status_show(lp);
    fill_rect(117+1,3+7+lp->status1*15,5,15);
}

void timePowerOffSetMenuOnLeft(void)
{
}

void timePowerOffSetMenuOnRight(void)
{
}

void timePowerOffSetMenuOnOk(void)
{
	struct MENU *lp;
	lp = win->curfous;
	switch ( lp->status1 )
	{
	case 0:		// 获取设置关机时间焦点
		if ( lp->subMenu )
		{
			win->curfous = lp->subMenu;
			win->curfous->show();
		}
		break;
	case 1:		// 获取设置启用关机设置焦点
		if ( ON != systems.power_status )		// 当前没有启用关机设置
		{
			systems.power_status = ON;
			draw_symbol( 99, 27, 10, 10, gou_mu, 0 );
		}
		else		// 当前启用关机设置
		{
			systems.power_status = OFF;
			clr_rect( 99, 27, 10, 10 );
		}
		break;
	case 2:		// 获取退出设置焦点
		if ( lp->topMenu )
		{
			win->curfous = lp->topMenu;
			win->curfous->show();
		}
		break;
   
	default: break;
	}
}

void timePowerOffSetMenuOnEsc(void)
{
	struct MENU *lp;
	lp = win->curfous;
	if ( lp->topMenu )
	{
		win->curfous = lp->topMenu;
		win->curfous->show();
	}
}

void timePowerOffSetMenuExec(void)
{

}

void power_status_show(struct MENU* lp)
{
	clr_rect( 99, 27, 10, 10 );
	draw_rect( 98, 26, 12, 12 );
	if ( ON == systems.power_status )
	{
		draw_symbol( 99, 27, 10, 10, gou_mu, 0 );	// 打钩
	}
}

/*
 * 定时关机时间设置菜单
 */
void powerTimeSetMenuShow(void)
{
	struct MENU *lp;
	lp = win->curfous; 
    lp->status1=0;
	clr_lcd();
	
   
    draw_rectmod();
    draw_line(32,15,122,15);
   
    
    
    
   
    systems.a_hour = systems.power_time.hour;
    systems.a_min = systems.power_time.min;
   
         draw_symbol(7,4,14,27,guanji_mu,0);
         draw_symbol(7,33,14,27,shijian_mu,0);
    	print_ch_str( 42, 3, "设置关机时间", 0 );   // 打印中文标题
    	switch ( lp->status1 )
    	{
    	case 0:
			print_mtime( systems.a_hour,	systems.a_min, 59-7, 23,HOUR); // 打印时间，并使"时"获得焦点
			draw_button( 43, 43, "确定", 0, CHINESE );    //打印"确定"按钮
			draw_button( 87-4, 44, "退出", 0, CHINESE );   //打印"取消"按钮
    		break;
    	case 1:
			print_mtime( systems.a_hour,	systems.a_min, 59-7, 23,MIN); // 打印时间，并使"分"获得焦点
			draw_button( 43, 43, "确定", 0, CHINESE );    //打印"确定"按钮
			draw_button( 87-4, 44, "退出", 0, CHINESE );   //打印"取消"按钮
    		break;
    	case 2:
			print_mtime( systems.a_hour,	systems.a_min, 59-7, 23,HOUR_MIN); // 打印时间
			draw_button( 43, 43, "确定", 1, CHINESE );    //打印"确定"按钮,使确定获得焦点
			draw_button( 87-4, 44, "退出", 0, CHINESE );   //打印"取消"按钮
    		break;
    	case 3:
			print_mtime( systems.a_hour,	systems.a_min, 59-7, 23,HOUR_MIN); // 打印时间
			draw_button( 43, 43, "确定", 0, CHINESE );    //打印"确定"按钮
			draw_button( 87-4, 44, "退出", 1, CHINESE );   //打印"取消"按钮,使取消获得焦点
    		break;
    	default: break;
    	}

}

void powerTimeSetMenuOnUp(void)
{
	struct MENU *lp;
	lp = win->curfous;
	switch ( lp->status1 )
	{
	case 0:		// 时获得焦点
		if ( systems.a_hour == 23 )
		{
			systems.a_hour = 0;
		}
		else
		{
			systems.a_hour++;
		}
		print_mtime( systems.a_hour,	systems.a_min, 59-7, 23,HOUR); // 打印时间
		break;
	case 1:		// 分获得焦点
		if ( systems.a_min == 59 )
		{
			systems.a_min = 0;
		}
		else
		{
			systems.a_min++;
		}
		print_mtime( systems.a_hour,	systems.a_min,59-7, 23,MIN); // 打印时间
		break;
	default: break;
    
	}
}

void powerTimeSetMenuOnDown(void)
{
	struct MENU *lp;
	lp = win->curfous;
	switch ( lp->status1 )
	{
	case 0:		// 时获得焦点
		if ( systems.a_hour == 0 )
		{
			systems.a_hour = 23;
		}
		else
		{
			systems.a_hour--;
		}
		print_mtime( systems.a_hour,	systems.a_min, 59-7, 23,HOUR); // 打印时间
		break;
	case 1:		// 分获得焦点
		if ( systems.a_min == 0 )
		{
			systems.a_min = 59;
		}
		else
		{
			systems.a_min--;
		}
		print_mtime( systems.a_hour,	systems.a_min, 59-7, 23,MIN); // 打印时间
		break;
	default: break;
	}
}

void powerTimeSetMenuOnLeft(void)
{
	struct MENU *lp;
	lp = win->curfous;
	
		switch ( lp->status1 )
		{
		case 0:		// 当前时获得焦点
			lp->status1 = 3;
			print_mtime( systems.a_hour,	systems.a_min, 59-7, 23,HOUR_MIN); // 打印时间
			draw_button( 87-4, 44, "退出", 1, CHINESE );   //打印"取消"按钮,使取消获得焦点
			break;
		case 1:		// 当前分获得焦点
			lp->status1 = 0;
			print_mtime( systems.a_hour,	systems.a_min, 59-7, 23,HOUR); // 打印时间
			break;
		case 2:		// 当前确定获得焦点
			lp->status1 = 1;
			print_mtime( systems.a_hour,	systems.a_min, 59-7, 23,MIN); // 打印时间
			draw_button( 43, 43, "确定", 0, CHINESE );    //打印"确定"按钮
			break;
		case 3:		// 当前取消获得焦点
			lp->status1 = 2;
			draw_button( 43, 43, "确定", 1, CHINESE );    //打印"确定"按钮
			draw_button( 87-4, 44, "退出", 0, CHINESE );   //打印"取消"按钮,使取消获得焦点
			break;
		default: break;
		}
}
void powerTimeSetMenuOnRight(void)
{
	struct MENU *lp;
	lp = win->curfous;
	
		switch ( lp->status1 )
		{
		case 0:		// 当前时获得焦点
			lp->status1 = 1;
			print_mtime( systems.a_hour,	systems.a_min, 59-7, 23, MIN ); // 打印时间
			break;
		case 1:		// 当前分获得焦点
			lp->status1 = 2;
			print_mtime( systems.a_hour,	systems.a_min, 59-7, 23, HOUR_MIN ); // 打印时间
			draw_button( 43, 43, "确定", 1, CHINESE );    //打印"确定"按钮
			break;
		case 2:		// 当前确定获得焦点
			lp->status1 = 3;
			draw_button( 43, 43, "确定", 0, CHINESE );    //打印"确定"按钮
			draw_button( 87-4, 44, "退出", 1, CHINESE );   //打印"取消"按钮,使取消获得焦点
			break;
		case 3:		// 当前取消获得焦点
			lp->status1 = 0;
			print_mtime( systems.a_hour,	systems.a_min, 59-7, 23, HOUR ); // 打印时间
			draw_button( 87-4, 44, "退出", 0, CHINESE );   //打印"取消"按钮,使取消获得焦点
			break;
		default: break;
		}
}

void powerTimeSetMenuOnOk(void)
{
	struct MENU *lp;
	lp = win->curfous;
	if ( 2 == lp->status1 )
	{	// 如果当前焦点为“确定”焦点，并且系统当前启用自动关机

		systems.power_time.hour = systems.a_hour;
		systems.power_time.min  = systems.a_min;
		systems.power_time.sec  = systems.a_sec;
	}
	if ( lp->topMenu )
	{
		systems.a_hour = 0;
		systems.a_min = 1;
		win->curfous = lp->topMenu;
		win->curfous->show();
	}
}

void powerTimeSetMenuOnEsc(void)
{
	struct MENU *lp;
	lp = win->curfous;
	if ( lp->topMenu )
	{
		win->curfous = lp->topMenu;
		win->curfous->show();
	}
}

void powerTimeSetMenuExec(void)
{

}

/**
 ** 输入密码菜单
 **/
void inputNumberMenuShow(void)
{
    struct MENU *lp;
    lp = win->curfous;
    clr_lcd();
    draw_rect( 0, 0, 127, 63 );
    draw_rect( 40, 19, 54, 14 );
    draw_symbol( 40-12, 20, 12, 12, left_smu, 0 );
	draw_symbol( 94, 20, 12, 12, right_smu, 0 );
  
        if ( lp->status2 )
        {
            print_ch_str( 6, 5, "密码错误", 0 );
            putchar( 54, 5, '!', 0 );
            print_ch_str( 60, 5, "请重新输入", 0 );
        }
        else
        {
            print_ch_str( 37, 5, "请输入密码", 0 );
        }
           
    switch ( lp->status1 )
    {
    case 0:
        fill_rect( 43, 20, 8, 12 );
        print_int( 44, 20, password.one_bit, 1 );
        print_int( 52, 20, password.two_bit, 0 );
        print_int( 60, 20, password.three_bit, 0 );
        print_int( 68, 20, password.four_bit, 0 );
        print_int( 76, 20, password.five_bit, 0 );
        print_int( 84, 20, password.six_bit, 0 );
      
            draw_button( 30, 36, "确定", 0, CHINESE );
            draw_button( 75, 36, "取消", 0, CHINESE );
        
        break;
    case 1:
        print_int( 44, 20, password.one_bit, 0 );
        fill_rect( 51, 20, 8, 12 );
        print_int( 52, 20, password.two_bit, 1 );
        print_int( 60, 20, password.three_bit, 0 );
        print_int( 68, 20, password.four_bit, 0 );
        print_int( 76, 20, password.five_bit, 0 );
        print_int( 84, 20, password.six_bit, 0 );
     
            draw_button( 30, 36, "确定", 0, CHINESE );
            draw_button( 75, 36, "取消", 0, CHINESE );
         
        break;
    case 2:
        print_int( 44, 20, password.one_bit, 0 );
        print_int( 52, 20, password.two_bit, 0 );
        fill_rect( 59, 20, 8, 12 );
        print_int( 60, 20, password.three_bit, 1 );
        print_int( 68, 20, password.four_bit, 0 );
        print_int( 76, 20, password.five_bit, 0 );
        print_int( 84, 20, password.six_bit, 0 );

		draw_button( 30, 36, "确定", 0, CHINESE );
		draw_button( 75, 36, "取消", 0, CHINESE );

        break;
    case 3:
        print_int( 44, 20, password.one_bit, 0 );
        print_int( 52, 20, password.two_bit, 0 );
        print_int( 60, 20, password.three_bit, 0 );
        fill_rect( 67, 20, 8, 12 );
        print_int( 68, 20, password.four_bit, 1 );
        print_int( 76, 20, password.five_bit, 0 );
        print_int( 84, 20, password.six_bit, 0 );
      
		draw_button( 30, 36, "确定", 0, CHINESE );
		draw_button( 75, 36, "取消", 0, CHINESE );
          
        break;
    case 4:
        print_int( 44, 20, password.one_bit, 0 );
        print_int( 52, 20, password.two_bit, 0 );
        print_int( 60, 20, password.three_bit, 0 );
        print_int( 68, 20, password.four_bit, 0 );
        fill_rect( 75, 20, 8, 12 );
        print_int( 76, 20, password.five_bit, 1 );
        print_int( 84, 20, password.six_bit, 0 );
       
            draw_button( 30, 36, "确定", 0, CHINESE );
            draw_button( 75, 36, "取消", 0, CHINESE );
       
        break;
    case 5:
        print_int( 44, 20, password.one_bit, 0 );
        print_int( 52, 20, password.two_bit, 0 );
        print_int( 60, 20, password.three_bit, 0 );
        print_int( 68, 20, password.four_bit, 0 );
        print_int( 76, 20, password.five_bit, 0 );
        fill_rect( 83, 20, 8, 12 );
        print_int( 84, 20, password.six_bit, 1 );
      
            draw_button( 30, 36, "确定", 0, CHINESE );
            draw_button( 75, 36, "取消", 0, CHINESE );
        
        break;
    case 6:
        print_int( 44, 20, password.one_bit, 0 );
        print_int( 52, 20, password.two_bit, 0 );
        print_int( 60, 20, password.three_bit, 0 );
        print_int( 68, 20, password.four_bit, 0 );
        print_int( 76, 20, password.five_bit, 0 );
        print_int( 84, 20, password.six_bit, 0 );
       
            draw_button( 30, 36, "确定", 1, CHINESE );
            draw_button( 75, 36, "取消", 0, CHINESE );
         
        break;
    case 7:
        print_int( 44, 20, password.one_bit, 0 );
        print_int( 52, 20, password.two_bit, 0 );
        print_int( 60, 20, password.three_bit, 0 );
        print_int( 68, 20, password.four_bit, 0 );
        print_int( 76, 20, password.five_bit, 0 );
        print_int( 84, 20, password.six_bit, 0 );
      
            draw_button( 30, 36, "确定", 0, CHINESE );
            draw_button( 75, 36, "取消", 1, CHINESE );
         
        break;
    default : break;
    }
}

void inputNumberMenuOnUp(void)
{
    struct MENU *lp;
    lp = win->curfous;
    
    switch ( lp->status1 )
    {
    case 0:
        if ( 9 == password.one_bit )
        {
            password.one_bit = 0;
        }
        else
        {
            password.one_bit++;
        }
        print_int( 44, 20, password.one_bit, 1 );
        break;
    case 1:
        if ( 9 == password.two_bit )
        {
            password.two_bit = 0;
        }
        else
        {
            password.two_bit++;
        }
        print_int( 52, 20, password.two_bit, 1 );
        break;
    case 2:
        if ( 9 == password.three_bit )
        {
            password.three_bit = 0;
        }
        else
        {
            password.three_bit++;
        }
        print_int( 60, 20, password.three_bit, 1 );
        break;
    case 3:
        if ( 9 == password.four_bit )
        {
            password.four_bit = 0;
        }
        else
        {
            password.four_bit++;
        }
        print_int( 68, 20, password.four_bit, 1 );
        break;
    case 4:
        if ( 9 == password.five_bit )
        {
            password.five_bit = 0;
        }
        else
        {
            password.five_bit++;
        }
        print_int( 76, 20, password.five_bit, 1 );
        break;
    case 5:
        if ( 9 == password.six_bit )
        {
            password.six_bit = 0;
        }
        else
        {
            password.six_bit++;
        }
        print_int( 84, 20, password.six_bit, 1 );
        break;
    default: break;
    }
}

void inputNumberMenuOnDown(void)
{
    struct MENU *lp;
    lp = win->curfous;
    
    switch ( lp->status1 )
    {
    case 0:
        if ( 0 == password.one_bit )
        {
            password.one_bit = 9;
        }
        else
        {
            password.one_bit--;
        }
        print_int( 44, 20, password.one_bit, 1 );
        break;
    case 1:
        if ( 0 == password.two_bit )
        {
            password.two_bit = 9;
        }
        else
        {
            password.two_bit--;
        }
        print_int( 52, 20, password.two_bit, 1 );
        break;
    case 2:
        if ( 0 == password.three_bit )
        {
            password.three_bit = 9;
        }
        else
        {
            password.three_bit--;
        }
        print_int( 60, 20, password.three_bit, 1 );
        break;
    case 3:
        if ( 0 == password.four_bit )
        {
            password.four_bit = 9;
        }
        else
        {
            password.four_bit--;
        }
        print_int( 68, 20, password.four_bit, 1 );
        break;
    case 4:
        if ( 0 == password.five_bit )
        {
            password.five_bit = 9;
        }
        else
        {
            password.five_bit--;
        }
        print_int( 76, 20, password.five_bit, 1 );
        break;
    case 5:
        if ( 0 == password.six_bit )
        {
            password.six_bit = 9;
        }
        else
        {
            password.six_bit--;
        }
        print_int( 84, 20, password.six_bit, 1 );
        break;
    default: break;
    }
}

void inputNumberMenuOnLeft(void)
{
    struct MENU *lp;
    lp = win->curfous;
    
    switch ( lp->status1 )
    {
    case 0:
        lp->status1 = 7;
        clr_rect( 43, 20, 8, 12 );
        print_int( 44, 20, password.one_bit, 0 );
       
            draw_button( 75, 36, "取消", 1, CHINESE );
 
        break;
    case 1:
        lp->status1 = 0;
        fill_rect( 43, 20, 8, 12 );
        print_int( 44, 20, password.one_bit, 1 );
        clr_rect( 51, 20, 8, 12 );
        print_int( 52, 20, password.two_bit, 0 );
        break;
    case 2:
        lp->status1 = 1;
        fill_rect( 51, 20, 8, 12 );
        print_int( 52, 20, password.two_bit, 1 );
        clr_rect( 59, 20, 8, 12 );
        print_int( 60, 20, password.three_bit, 0 );
        break;
    case 3:
        lp->status1 = 2;
        fill_rect( 59, 20, 8, 12 );
        print_int( 60, 20, password.three_bit, 1 );
        clr_rect( 67, 20, 8, 12 );
        print_int( 68, 20, password.four_bit, 0 );
        break;
    case 4:
        lp->status1 = 3;
        fill_rect( 67, 20, 8, 12 );
        print_int( 68, 20, password.four_bit, 1 );
        clr_rect( 75, 20, 8, 12 );
        print_int( 76, 20, password.five_bit, 0 );
        break;
    case 5:
        lp->status1 = 4;
        fill_rect( 75, 20, 8, 12 );
        print_int( 76, 20, password.five_bit, 1 );
        clr_rect( 83, 20, 8, 12 );
        print_int( 84, 20, password.six_bit, 0 );
        break;
    case 6:
        lp->status1 = 5;
        fill_rect( 83, 20, 8, 12 );
        print_int( 84, 20, password.six_bit, 1 );
      
            draw_button( 30, 36, "确定", 0, CHINESE );
   
        break;
    case 7:
        lp->status1 = 6;
       
            draw_button( 30, 36, "确定", 1, CHINESE );
            draw_button( 75, 36, "取消", 0, CHINESE );

        break;
    default: break;
    }
}

void inputNumberMenuOnRight(void)
{
    struct MENU *lp;
    lp = win->curfous;
    
    switch ( lp->status1 )
    {
    case 0:
        lp->status1 = 1;
        clr_rect( 43, 20, 8, 12 );
        print_int( 44, 20, password.one_bit, 0 );
        fill_rect( 51, 20, 8, 12 );
        print_int( 52, 20, password.two_bit, 1 );
        break;
    case 1:
        lp->status1 = 2;
        fill_rect( 59, 20, 8, 12 );
        print_int( 60, 20, password.three_bit, 1 );
        clr_rect( 51, 20, 8, 12 );
        print_int( 52, 20, password.two_bit, 0 );
        break;
    case 2:
        lp->status1 = 3;
        fill_rect( 67, 20, 8, 12 );
        print_int( 68, 20, password.four_bit, 1 );
        clr_rect( 59, 20, 8, 12 );
        print_int( 60, 20, password.three_bit, 0 );
        break;
    case 3:
        lp->status1 = 4;
        fill_rect( 75, 20, 8, 12 );
        print_int( 76, 20, password.five_bit, 1 );
        clr_rect( 67, 20, 8, 12 );
        print_int( 68, 20, password.four_bit, 0 );
        break;
    case 4:
        lp->status1 = 5;
        fill_rect( 83, 20, 8, 12 );
        print_int( 84, 20, password.six_bit, 1 );
        clr_rect( 75, 20, 8, 12 );
        print_int( 76, 20, password.five_bit, 0 );
        break;
    case 5:
        lp->status1 = 6;
       
            draw_button( 30, 36, "确定", 1, CHINESE );
        
        clr_rect( 83, 20, 8, 12 );
        print_int( 84, 20, password.six_bit, 0 );
        break;
    case 6:
        lp->status1 = 7;
       
            draw_button( 30, 36, "确定", 0, CHINESE );
            draw_button( 75, 36, "取消", 1, CHINESE );
          
        break;
    case 7:
        lp->status1 = 0;
      
            draw_button( 75, 36, "取消", 0, CHINESE );
         
        fill_rect( 43, 20, 8, 12 );
        print_int( 44, 20, password.one_bit, 1 );
        break;
    default: break;
    }
}

void inputNumberMenuOnOk(void)
{
    struct MENU *lp;
    lp = win->curfous;
    
     if ( 7 == lp->status1 )
    {
        lp->status2 = 0;
        if ( lp->topMenu )
        {
            win->curfous = lp->topMenu;
            win->curfous->show();
        }
    }else
    {
        if ( password.one_bit == PASS_WORD_ONE && password.two_bit == PASS_WORD_TWO
             && password.three_bit == PASS_WORD_THREE && password.four_bit == PASS_WORD_FOUR
             && password.five_bit == PASS_WORD_FIVE && password.six_bit == PASS_WORD_SIX )
        {
            lp->status2 = 0;
            password.one_bit = 0;
            password.two_bit = 0;
            password.three_bit = 0;
            password.four_bit = 0;
            password.five_bit = 0;
            password.six_bit = 0;
            
            if ( lp->subMenu )
            {
                win->curfous = lp->subMenu;
                win->curfous->show();
            }
        }
        else
        {
            lp->status2++;
            if ( lp->subMenu )
            {
                win->curfous = lp->subMenu;
                lp = win->curfous;
                if ( lp->topMenu )
                {
                    win->curfous = lp->topMenu;
                    win->curfous->show();
                }
            }
        }
     
    
    }
}

void inputNumberMenuOnEsc(void)
{
    struct MENU *lp;
    lp = win->curfous;
    lp->status2 = 0;
    
    if ( lp->topMenu )
    {
        win->curfous = lp->topMenu;
        win->curfous->show();
    }
}

void inputNumberMenuExec(void)
{
    
}

void inputNumberMenuLongOkKey(void)
{
   
   win->show_flag ^= 0x01;
   win->curfous->show();


}


/**
 ** 提示菜单
 **/
void clewMenuShow(void)
{
    struct MENU *lp;
    lp = win->curfous;
    clr_lcd();
    draw_rect( 0, 0, 127, 63 );

   
   
        if (  lp->status2==0  )
        { //gang jin ru
            draw_rect(35-1,4,50,14);
            draw_rect(35-1,24,50,14);
            draw_rect(35-1,44,50,14);
            
           switch ( lp->status1 )
          {
            case 0:
            print_ch_str( 35, 5, "手动校准", 1 ) ;
            print_ch_str( 35, 25, "出厂默认", 0 );
            print_ch_str( 35, 45, "退出校准", 0 );
            break;
            case 1:
            print_ch_str( 35, 5, "手动校准", 0 ) ;
            print_ch_str( 35, 25, "出厂默认", 1 );
            print_ch_str( 35, 45, "退出校准", 0 );
            break;
            case 2:
            print_ch_str( 35, 5, "手动校准", 0 ) ;
            print_ch_str( 35, 25, "出厂默认", 0 );
            print_ch_str( 35, 45, "退出校准", 1 );
            break;    
            
            default: break;
           }
            
           
        }
        else // tan chuang
        {
            if(lp->status2==1 )//未保存 esc
            print_ch_str( 12, 18, "未保存  是否继续", 0 );
            else               //以保存
            print_ch_str( 12, 18, "已保存  是否继续", 0 );    
            putchar( 12+36, 18, '!', 0 );
            putchar( 110, 18, '?', 0 );
            if(lp->status1 ==0)
            {
            draw_button( 25, 36, "确定", 1, CHINESE);
            draw_button( 75, 36, "取消", 0, CHINESE );
            }
            else
            {
             draw_button( 25, 36, "确定", 0, CHINESE );
             draw_button( 75, 36, "取消", 1, CHINESE );
            }
            

      }
    
}

void clewMenuOnUp(void)
{
   struct MENU *lp;
   lp = win->curfous;
   
   if(lp->status2==0)
   {
      
   
       
         switch ( lp->status1 )
          {
           case 0:
            lp->status1=2; 
            print_ch_str( 35, 5, "手动校准", 0 ) ;
            print_ch_str( 35, 25, "出厂默认", 0 );
            print_ch_str( 35, 45, "退出校准", 1 );
            break;
           case 1:
            lp->status1=0; 
            print_ch_str( 35, 5, "手动校准", 1 ) ;
            print_ch_str( 35, 25, "出厂默认", 0 );
            print_ch_str( 35, 45, "退出校准", 0 );
            break;
           case 2:
            lp->status1=1; 
            print_ch_str( 35, 5, "手动校准", 0 ) ;
            print_ch_str( 35, 25, "出厂默认", 1 );
            print_ch_str( 35, 45, "退出校准", 0 );
            break;    
            
            default: break;
           }
    
       
   
   }
   else//zai tanchuang jiemianxia 
   {}
   



}
void clewMenuOnDown(void)
{
    struct MENU *lp;
    lp = win->curfous;
    if(lp->status2==0)
    {
       
    
        
       switch ( lp->status1 )
          {
          case 0:
            lp->status1=1; 
            print_ch_str( 35, 5, "手动校准", 0 ) ;
            print_ch_str( 35, 25, "出厂默认", 1 );
            print_ch_str( 35, 45, "退出校准", 0 );
            break;
          case 1:
            lp->status1=2; 
            print_ch_str( 35, 5, "手动校准", 0 ) ;
            print_ch_str( 35, 25, "出厂默认", 0 );
            print_ch_str( 35, 45, "退出校准", 1 );
            break;
           case 2:
            lp->status1=0; 
            print_ch_str( 35, 5, "手动校准", 1 ) ;
            print_ch_str( 35, 25, "出厂默认", 0 );
            print_ch_str( 35, 45, "退出校准", 0 );
            break;    
            
            default: break;
           }
    
    }
    else//zai tan chuang jiemian xia 
    {
    
        
    }

  

}








void clewMenuOnLeft(void)
{
    struct MENU *lp;
    lp = win->curfous;
    
    
    
    if(lp->status2==1)//弹窗左键
    {     
    
        switch ( lp->status1 )
        {
        case 0:
            lp->status1 = 1;
            
            draw_button( 25, 36, "确定", 0, CHINESE );
            draw_button( 75, 36, "取消", 1, CHINESE );
            break;
        case 1:
            lp->status1 = 0;
            draw_button( 25, 36, "确定", 1, CHINESE );
            draw_button( 75, 36, "取消", 0, CHINESE );
            break;
        default: break;
        }

    
  }
  else if (lp->status2==2)//初次界面左键
  {
    
        switch ( lp->status1 )
        {
        case 0:
            lp->status1 = 1;
            
            draw_button( 25, 36, "确定", 0, CHINESE );
            draw_button( 75, 36, "取消", 1, CHINESE );
            break;
        case 1:
            lp->status1 = 0;
            draw_button( 25, 36, "确定", 1, CHINESE );
            draw_button( 75, 36, "取消", 0, CHINESE );
            break;
        default: break;
        }
  
      
  
  }
}

void clewMenuOnRight(void)
{
    struct MENU *lp;
    lp = win->curfous;
    
    if(lp->status2!=0)//弹窗左键 
   { 
      
        switch ( lp->status1 )
        {
        case 0:
            lp->status1 = 1;
            draw_button( 25, 36, "确定", 0, CHINESE );
            draw_button( 75, 36, "取消", 1, CHINESE );
            break;
        case 1:
            lp->status1 = 0;
            draw_button( 25, 36, "确定", 1, CHINESE );
            draw_button( 75, 36, "取消", 0, CHINESE );
            break;
        default: break;
        }
    
  }
   else//初次界面 右键处理函数
   {}
}

void clewMenuOnOk(void)
{
    struct MENU *lp;
    lp = win->curfous;
    
    
    if ( 0 == lp->status1 )//进入手动校准模式
    {
        lp->status2 = 1;
        if ( lp->subMenu )
        {
            systems.status = STOP;
            startPump( systems.a_flow );
            systems.presure = getPFromFlow( systems.a_flow );
            
            if(systems.presure > MAX_PRESURE)
            {
                systems.presure = systems.a_flow;
                writePToFlow(systems.presure,systems.a_flow);
            }
            
            changePresure( systems.presure );
            
            win->curfous = lp->subMenu;
            win->curfous->show();
        }
    }    
    else if(1==lp->status1)  //刷flash 进入下层      
   {        
	   	if(lp->status2==0)				//处于告警界面
        {
            
                 set_default();
                 lp->status2 = 1;
                 if ( lp->subMenu )
                {
                 systems.status = STOP;
                 startPump( systems.a_flow );
                 systems.presure = getPFromFlow( systems.a_flow );
            
                 if(systems.presure > MAX_PRESURE)
                  {
                     systems.presure = systems.a_flow;
                     writePToFlow(systems.presure,systems.a_flow);
                 }
            
                 changePresure( systems.presure );
            
                 win->curfous = lp->subMenu;
                 win->curfous->show();
                 
                }
      
            else				//不可能进入
           {
               lp->status2 = 0;
              if ( lp->topMenu )
             {
                win->curfous = lp->topMenu;
                lp = win->curfous;
                if ( lp->topMenu )
                 {
                     win->curfous = lp->topMenu;
                     win->curfous->show();
               
                }
              }
		   }
      } 
      else//退出校准
      {
       
        lp->status2 = 0;
        if ( lp->topMenu )
        {
            win->curfous = lp->topMenu;
            lp = win->curfous;
            if ( lp->topMenu )
            {
                win->curfous = lp->topMenu;
                win->curfous->show();
            }
        }
      }   
        
   }//statu1=1
    else //statu1 =3 zhuangtai
    {  //回上层 取消
       lp->status2 = 0;
        if ( lp->topMenu )
        {
            win->curfous = lp->topMenu;
            lp = win->curfous;
            if ( lp->topMenu )
            {
                win->curfous = lp->topMenu;
                win->curfous->show();
            }
        }
    }
}

void clewMenuOnEsc(void)
{
    struct MENU *lp;
    lp = win->curfous;
    lp->status2 = 0;
    if ( lp->topMenu )
    {
        win->curfous = lp->topMenu;
        lp = win->curfous;
        if ( lp->topMenu )
        {
            win->curfous = lp->topMenu;
            win->curfous->show();
        }
    }
}

/*void clewMenuExec(void)
{

    
}
*/

/*
 * 校准流量菜单
 */
void calibrateFlowMenuShow(void)
{
	//struct MENU *lp;
	
	win->mLevel = 2;
	//lp = win->curfous;	
	clr_lcd();	

	draw_symbol(7,4,14,27,liuliang_mu,0);
	draw_symbol(7,33,14,27,jiaozhun_mu,0);   
    
    draw_rectmod();   
    
    draw_symbol(32,3,10,12,s_mu,0);
    draw_symbol(84,3,10,12,f_mu,0);
    draw_symbol(49,17,21,12,tb_mu,0);   
    
	print_str( 55, 51, "mL/min", 0 );    //打印"mL/min"字符在右边
	
	print_int_num( 54, 33, systems.a_flow, 1 );
	
	draw_symbol( 42, 35, 9, 12, jian_mu1, 0 );
    draw_line(48,34,51,34);
    draw_line(48,35+12,51,35+12);
    draw_line(51,34,51,35+12);
    draw_symbol( 93, 35, 9, 12, plus_mu1, 0 );
	draw_line(92,34,95,34);
    draw_line(92,35+12,95,35+12);
    draw_line(92,34,92,35+12); 
     
     
   
}

void calibrateFlowMenuOnUp(void)
{
	if ( systems.a_flow < MAX_FLOW )
	{
		systems.a_flow += MID_FLOW;
	}
	else
	{
		systems.a_flow = MIN_FLOW;
	}
    
    print_int_num( 54, 33, systems.a_flow, 1 );
    
	
    
    systems.presure = getPFromFlow( systems.a_flow );
    
    if( systems.presure > MAX_PRESURE )
    {
        systems.presure = systems.a_flow;
                    
        writePToFlow( systems.presure, systems.a_flow );
    }
    
    changePresure( systems.presure );   // 设置压力值
}

void calibrateFlowMenuOnDown(void)
{
	if ( systems.a_flow > MIN_FLOW )
	{
		systems.a_flow -= MID_FLOW;
	}
	else
	{
		systems.a_flow = MAX_FLOW;
	}
	
   
    
    print_int_num( 54, 33, systems.a_flow, 1 );
    
   
    
    systems.presure = getPFromFlow( systems.a_flow );
    
    if( systems.presure > MAX_PRESURE )
    {
        systems.presure = systems.a_flow;
                    
        writePToFlow( systems.presure, systems.a_flow );
    }
    
    changePresure( systems.presure );   //设置压力值
}

void calibrateFlowMenuOnLeft(void)
{
    struct MENU *lp;
    
    lp = win->curfous;
    
    lp->status2 = 0;
    
    if( systems.presure > MIN_PRESURE )
    {
        systems.presure--;
        
        changePresure( systems.presure );
    }
      clr_rect( 42, 34, 11, 14 );
  
}

void calibrateFlowMenuOnRight(void)
{
    struct MENU *lp;
    
    lp = win->curfous;    
 
    lp->status2 = 0;
    
    if( systems.presure < MAX_PRESURE )
    {
        systems.presure++;
        
        changePresure( systems.presure );
    } 
     
    
   
        clr_rect( 95, 34, 11, 14 );
  
}

void calibrateFlowMenuOnOk(void)
{
    struct MENU * lp;		// 保存当前级菜单  
    //unsigned long pumpIbuf = 0;    
    //unsigned int pumpI;
    
    lp = win->curfous;

    lp->status2 = 0;
    
    
    
    stopPump();     //停止泵
    
    writePToFlow( systems.presure, systems.a_flow );
    //writeIToFlow( pumpI, systems.a_flow );   //将电流值写入到表中
    
    if( lp->topMenu )
    {
        win->curfous = lp->topMenu;  //转移到上层菜单
        win->curfous->status2=2;
        win->curfous->show();   //当前菜单进行显示
    }
}

void calibrateFlowMenuOnEsc(void)
{
	struct MENU *lp;
	lp = win->curfous;
	if ( lp->topMenu )
	{
        stopPump();     //停止泵
		win->curfous = lp->topMenu;
        win->curfous->status2=1;
		win->curfous->show();
        
	}
}

void calibrateFlowMenuExec(void)
{
    struct MENU *lp;
    lp = win->curfous;
    
    if ( lp->status2 == 8 )
    {
        lp->status2 = 0;
     draw_symbol( 42, 35, 9, 12, jian_mu1, 0 );
     draw_line(48,34,51,34);
     draw_line(48,35+12,51,35+12);
     draw_line(51,34,51,35+12);
     draw_symbol( 93, 35, 9, 12, plus_mu1, 0 );
     draw_line(92,34,95,34);
     draw_line(92,35+12,95,35+12);
     draw_line(92,34,92,35+12); 
      
    }
    if ( lp->status2 < 8 )
    {
        lp->status2++;  
       
    }
    else
    {
        lp->status2 = 0;
    }
    
    
    
    
      clr_rect(46,3,24,12);
      print_int(46,3,get_smppresure(),0);
      clr_rect(97,3,24,12);
      print_int(97,3,getPress(),0);
      clr_rect(72,18,30,12);
      print_int(72,18,TB0CCR1,0);
      
    
    
    
}





/*
 * 定时模式组设置菜单
 */
void timeGroupSetMenuShow(void)
{
	struct MENU *lp;
	lp = win->curfous;
	clr_lcd();
    static unsigned int i=0;
   /* if ( ENGLISH == systems.language )
    {
       draw_symbol(3,8,22,16,shijian_eu,0);
       draw_symbol(3,40,22,16,moshi_eu,0);
    }
    else
    {*/
       draw_symbol(7,4,14,27,dingshi_mu,0);
        draw_symbol(7,33,14,27,canshu_mu,0);
    //}
      
   
    draw_rectmod(); 
   // draw_line( 32, 15, 122, 15 ); //打印一条在靠选择栏的直线
    draw_symbol(28,14,99,31,groupsetmod_mu,0);
	
	print_ch_str( 59, 2, "组选项", 0 );
	draw_button( 59, 46, "退出", 0, CHINESE );

    
    if(lp->status1<5)
     draw_round(32+lp->status1*19,16,lp->status1+1,1);
    else if(lp->status1<10)
    {
        draw_round(32+(lp->status1-5)*19,31,lp->status1+1,1);
    }
    else
    {   
       
            draw_button( 59, 46, "退出", 1, CHINESE );
  
        
    }
    
     for(i=0;i<10;i++)
    {
      if(systems.groups[i].srb ==1)
      {
        if(i<5)
        draw_rect(32+i*19,16,14,14);
        else 
        draw_rect(32+(i-5)*19,31,14,14);
      }
      
    
    };
    
}

void timeGroupSetMenuOnUp(void)
{
	struct MENU *lp;
	lp = win->curfous;
	 if(lp->status1<5)
     {
         draw_round(32+lp->status1*19,16,lp->status1+1,0);
         lp->status1=10;
     }
    else if(lp->status1<10)
    {
        draw_round(32+(lp->status1-5)*19,31,lp->status1+1,0); 
         lp->status1=lp->status1-5;
    }
    else
    {
       
        
            draw_button( 59, 46, "退出", 0, CHINESE );
   
        lp->status1=7;
    }
    ////////////////
 
    if(lp->status1<5)
     draw_round(32+lp->status1*19,16,lp->status1+1,1);
    else if(lp->status1<10)
    {
        draw_round(32+(lp->status1-5)*19,31,lp->status1+1,1);
    }
    else
    {
        
            draw_button( 59, 46, "退出", 1, CHINESE );

    }
    
     for(int i=0;i<10;i++)
    {
      if(systems.groups[i].srb ==1)
      {
        if(i<5)
        draw_rect(32+i*19,16,14,14);
        else 
        draw_rect(32+(i-5)*19,31,14,14);
      }
      
    
    };
}

void timeGroupSetMenuOnDown(void)
{
	struct MENU *lp;
	lp = win->curfous;
	 if(lp->status1<5)
     {
         draw_round(32+lp->status1*19,16,lp->status1+1,0);
         lp->status1+=5;
     }
     else if(lp->status1<10)
    {
        draw_round(32+(lp->status1-5)*19,31,lp->status1+1,0);
        lp->status1=10;
    
    }
    else
    {
        
            draw_button( 59, 46, "退出", 0, CHINESE );
   
        lp->status1=2;
    }
    ////////////////
    
    
    
    ////////////////
    if(lp->status1<5)
     draw_round(32+lp->status1*19,16,lp->status1+1,1);
    else if(lp->status1<10)
    {
        draw_round(32+(lp->status1-5)*19,31,lp->status1+1,1);
    }
    else
    {

            draw_button( 59, 46, "退出", 1, CHINESE );

    }
    
     for(int i=0;i<10;i++)
    {
      if(systems.groups[i].srb ==1)
      {
        if(i<5)
        draw_rect(32+i*19,16,14,14);
        else 
        draw_rect(32+(i-5)*19,31,14,14);
      }
      
    
    };
    
}

void timeGroupSetMenuOnLeft(void)
{
	struct MENU *lp;
	lp = win->curfous; 
    
    if(lp->status1<5)
     draw_round(32+lp->status1*19,16,lp->status1+1,0);
    else if(lp->status1<10)
    {
        draw_round(32+(lp->status1-5)*19,31,lp->status1+1,0);
    }
    else
    {

            draw_button( 59, 46, "退出", 0, CHINESE );

    }
    /////////////////////////////
    if(lp->status1>0)
    {
     lp->status1--;
    }
    else 
    lp->status1=10;
    /////////////////////////////
     if(lp->status1<5)
     draw_round(32+lp->status1*19,16,lp->status1+1,1);
    else if(lp->status1<10)
    {
        draw_round(32+(lp->status1-5)*19,31,lp->status1+1,1);
    }
    else
    {

            draw_button( 59, 46, "退出", 1, CHINESE );

    }
        
     for(int i=0;i<10;i++)
    {
      if(systems.groups[i].srb ==1)
      {
        if(i<5)
        draw_rect(32+i*19,16,14,14);
        else 
        draw_rect(32+(i-5)*19,31,14,14);
      }
      
    
    };
    
}

void timeGroupSetMenuOnRight(void)
{
	struct MENU *lp;
	lp = win->curfous;
	if(lp->status1<5)
     draw_round(32+lp->status1*19,16,lp->status1+1,0);
    else if(lp->status1<10)
    {
        draw_round(32+(lp->status1-5)*19,31,lp->status1+1,0);
    }
    else
    {
		draw_button( 59, 46, "退出", 0, CHINESE );
    }
    /////////////////////////////
    if(lp->status1<10)
    {
     lp->status1++;
    }
    else lp->status1=0;
    /////////////////////////////
     if(lp->status1<5)
     draw_round(32+lp->status1*19,16,lp->status1+1,1);
    else if(lp->status1<10)
    {
        draw_round(32+(lp->status1-5)*19,31,lp->status1+1,1);
    }
    else
    {    
		draw_button( 59, 46, "退出", 1, CHINESE );
    }
    
     for(int i=0;i<10;i++)
    {
      if(systems.groups[i].srb ==1)
      {
        if(i<5)
        draw_rect(32+i*19,16,14,14);
        else 
       draw_rect(32+(i-5)*19,31,14,14);
      }
      
    
    };
}

void timeGroupSetMenuOnOk(void)
{
	struct MENU *lp;
    lp = win->curfous;
     
      if(lp->status1<10)
    {
        if(lp->lMenu->subMenu->rMenu)
        { 
            systems.set_group = lp->status1;	// 确认设置的工作组
			win->curfous = lp->lMenu->subMenu->rMenu;		// 进入各个组菜单参数设置菜单
			win->curfous->show();    
        }
    }
    else
    {
        
       if ( lp->topMenu )
	  {
		 win->curfous = lp->topMenu;
         win->curfous->show();
		
	  } 
        
        
    }
    

  
    
}

void timeGroupSetMenuOnEsc(void)
{
	struct MENU *lp;
	
	lp = win->curfous;

	if ( lp->topMenu )
	{
		win->curfous = lp->topMenu;
        win->curfous->show();
        
		
	}
}

void timeGroupSetMenuExec(void)
{
    
    

}



/*
 * 定时模式定时组参数设置菜单
 */
void timeGroupParaSetMenuShow(void)
{
	struct MENU *lp;
	//unsigned char shi=0,ge=0;
	
	lp = win->curfous;
	lp->status1 = 0;
	clr_lcd();
	draw_symbol(7,4,14,27,zu_mu,0);    
   
    
    draw_round(7,3,systems.set_group + 1,0);
    draw_rectmod();
 
	draw_symbol(7,33,14,27,shezhi_mu,0); 
	
	putchar(58,4,':',0);
	putchar(58,18,':',0);
	putchar(58,32,':',0);
	putchar(64+12, 4, ':', 0 );	
	print_str(64+12, 32, "h  m", 0 );	
	draw_rect( 90, 46, 12, 12 );	
	print_str(64+24,18,"ml/min",0);
	
	if ( systems.groups[systems.set_group].flow )
	{
		systems.a_flow = systems.groups[systems.set_group].flow;
	}
	
	systems.temp_hour = systems.groups[systems.set_group].start_time.hour;	
	systems.temp_min = systems.groups[systems.set_group].start_time.min;
	
	if ( systems.groups[systems.set_group].smp_time )
	{
		systems.a_min = systems.groups[systems.set_group].smp_time % 60;
		systems.a_hour = systems.groups[systems.set_group].smp_time / 60;
	}
	else
	{
		systems.a_hour = 0;
		systems.a_min = ORIGIN_SMP_TIME;
	}
	setTime(64,4,systems.temp_hour,systems.temp_min,0);	

	setFlow(64,18,systems.a_flow,0);
	setTime(64,32,systems.a_hour,systems.a_min,0);
	switch ( lp->status1 )
	{
		case 0:
			print_ch_str( 34, 4, "定时", 1 );  			
			print_ch_str( 34, 18, "流量", 0 );
			print_ch_str( 34, 32, "时长", 0 );
			print_ch_str( 34, 46, "启用", 0 );			
			break;
			
		case 1:
			print_ch_str( 34, 4, "定时", 0 );   
			print_ch_str( 34, 18, "流量", 1 );
			print_ch_str( 34, 32, "时长", 0 );
			print_ch_str( 34, 46, "启用", 0 );
			break;
			
		case 2:
			print_ch_str( 34, 4, "定时", 0 );    			
			print_ch_str( 34, 18, "流量", 0 );
			print_ch_str( 34, 32, "时长", 1 );
			print_ch_str( 34, 46, "启用", 0 );
			break;
			
        case 3:
			print_ch_str( 34, 4, "定时", 0 );		
			print_ch_str( 34, 18, "流量", 0 );
			print_ch_str( 34, 32, "时长", 0 );
			print_ch_str( 34, 46, "启用", 1 );
            break;			
		default:
			break;
	}
	if( 1 == systems.groups[systems.set_group].srb )
	{
		draw_symbol( 91, 47, 10, 10, gou_mu, 0 );
	}
}

void timeGroupParaSetMenuOnUp(void)
{
	struct MENU *lp;
	lp = win->curfous;
	unsigned int temp = 0;
	unsigned int temp1 = 0;
	
	
	
	switch ( lp->status1 )
	{
		case 0:
			lp->status1=3;			
			print_ch_str( 34, 4, "定时", 0 );  			
			print_ch_str( 34, 18, "流量", 0 );
			print_ch_str( 34, 32, "时长", 0 );
			print_ch_str( 34, 46, "启用", 1 );
            break; 
			
		case 1:
			lp->status1=0;			
			print_ch_str( 34, 4, "定时", 1 );  			
			print_ch_str( 34, 18, "流量", 0 );
			print_ch_str( 34, 32, "时长", 0 );
			print_ch_str( 34, 46, "启用", 0 );				
			break;
		
		case 2:		
			lp->status1=1;			
			print_ch_str( 34, 4, "定时", 0 );    			
			print_ch_str( 34, 18, "流量", 1 );
			print_ch_str( 34, 32, "时长", 0 );
			print_ch_str( 34, 46, "启用", 0 );
			break;
			
		 case 3:
		 	lp->status1=2;
			print_ch_str( 34, 4, "定时", 0 );  			
			print_ch_str( 34, 18, "流量", 0 );
			print_ch_str( 34, 32, "时长", 1 );
			print_ch_str( 34, 46, "启用", 0 );
			break;
       
		 case 4:
			if ( systems.temp_hour >= 23 )
			{
				systems.temp_hour = 0;
			}
			else
			{
				systems.temp_hour++;
			}		 	
		 	setTime(64,4,systems.temp_hour,systems.temp_min,5);
			break;
			
		 case 5:
			if ( 59 == systems.temp_min )
			{
				systems.temp_min = 0;
			}
			else
			{
				systems.temp_min++;
			}	 	
		 	setTime(64,4,systems.temp_hour,systems.temp_min,6);
			break;
			
		case 6:
			temp = (unsigned int)(systems.a_flow / 1000);
			systems.a_flow -= temp * 1000;
			temp++;
			if(temp > MAX_FLOW/1000)
			{
				temp = 0;
			}			
			systems.a_flow  += temp * 1000;
			if( systems.a_flow > MAX_FLOW )
			{
				systems.a_flow = MAX_FLOW;				
			}
			
			if ( systems.a_flow < MIN_FLOW )
			{      
				systems.a_flow = MIN_FLOW;
			}
			setFlow(64,18,systems.a_flow,1);
			break;
			
		case 7:
			temp1 = (unsigned int)(systems.a_flow / 1000); 
			temp = (unsigned int)((systems.a_flow % 1000) / 100);
			systems.a_flow  -= temp * 100;
			
			if(temp1==MAX_FLOW/1000)					
			{
#if       		MID_FLOW <100				
				temp++;
				
#else        	
				temp+=(MID_FLOW/100);
				
#endif       				
          		if(temp>(MAX_FLOW%1000)/100)
          		{
           	 		temp=0;
              
          		}
			}
			else
			{
#if        		MID_FLOW <100  				
				temp++;
				
#else           
				temp+=(MID_FLOW/100);
					
#endif  
				if(10 == temp)
				{
					temp = 0;
				}	
			}
			
			systems.a_flow  += temp * 100;			
			
			if(systems.a_flow  < MIN_FLOW)
			{
				systems.a_flow  = MIN_FLOW;	
			}
        
			if(systems.a_flow  > MAX_FLOW)
			{            
				systems.a_flow  = MAX_FLOW;
			}	
			setFlow(64,18,systems.a_flow,2);
			break;	
	
		case 8:
#if  		MID_FLOW <100
		
			temp  = (unsigned int)((systems.a_flow % 100) / 10);
			systems.a_flow -= temp * 10;
			
			if(0 == temp)
			{
				temp = 5;
			}
			else
			{
				temp = 0;
			}
			
			systems.a_flow += temp * 10;
			
			if( systems.a_flow > MAX_FLOW )
			{
				systems.a_flow = MAX_FLOW;				
			}
			
			if ( systems.a_flow < MIN_FLOW )
			{      
				systems.a_flow = MIN_FLOW;
			}
			setFlow(64,18,systems.a_flow,3);        
#endif          
			break;
			
			
		case 9:
			temp = (unsigned char)(systems.a_hour/10);		
			systems.a_hour -= temp * 10;
			if(temp >= 9)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.a_hour = systems.a_hour + temp*10;	
            setTime(64,32,systems.a_hour,systems.a_min,1);
			break;
			
		case 10:	
			temp = (unsigned char)(systems.a_hour%10);
			systems.a_hour -= temp;
			if(temp >= 9)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.a_hour = temp+systems.a_hour;	
			setTime(64,32,systems.a_hour,systems.a_min,2);
			break;
			
		case 11:
			temp = (unsigned char)(systems.a_min/10);	
			systems.a_min -= temp * 10;
			if(temp >= 5)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.a_min = temp*10+systems.a_min;	
			setTime(64,32,systems.a_hour,systems.a_min,3);
			break;
		
		case 12:
			temp = (unsigned char)(systems.a_min%10);	
			systems.a_min -= temp;			
			if(temp >= 9)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.a_min = systems.a_min+temp;	
			setTime(64,32,systems.a_hour,systems.a_min,4);
			break;			
		 default: 
		 	break;
	}
}

void timeGroupParaSetMenuOnDown(void)
{
	struct MENU *lp;
	unsigned int temp =0, temp1 = 0;
	lp = win->curfous;
	
	switch ( lp->status1 )
	{
		case 0:
			lp->status1=1;
			print_ch_str( 34, 4, "定时", 0 ); 			
			print_ch_str( 34, 18, "流量", 1 );
			print_ch_str( 34, 32, "时长", 0 );
			print_ch_str( 34, 46, "启用", 0 );
            break;  
			
		case 1:
			lp->status1=2;
			print_ch_str( 34, 4, "定时", 0 );			
			print_ch_str( 34, 18, "流量", 0 );
			print_ch_str( 34, 32, "时长", 1 );
			print_ch_str( 34, 46, "启用", 0 );
			break;
			
		 case 2:		 
		 	lp->status1=3;
			print_ch_str( 34, 4, "定时", 0 );			
			print_ch_str( 34, 18, "流量", 0 );
			print_ch_str( 34, 32, "时长", 0 );
			print_ch_str( 34, 46, "启用", 1 );			
			break;
			
		 case 3:
		 	lp->status1=0;			
			print_ch_str( 34, 4, "定时", 1 );			
			print_ch_str( 34, 18, "流量", 0 );
			print_ch_str( 34, 32, "时长", 0 );
			print_ch_str( 34, 46, "启用", 0 );
			break;
			
		 case 4:
			if ( 0 == systems.temp_hour )
			{
				systems.temp_hour = 23;
			}
			else
			{
				systems.temp_hour--;
			}	
		 	setTime(64,4,systems.temp_hour,systems.temp_min,5);
			break;
			
		 case 5:
			if ( 0 == systems.temp_min )
			{
				systems.temp_min = 59;
			}
			else
			{
				systems.temp_min--;
			}
		 	setTime(64,4,systems.temp_hour,systems.temp_min,6);
			break;
	
		case 6:
           temp = (unsigned int)(systems.a_flow / 1000);
		   systems.a_flow -= temp * 1000;
		   if ( temp > 0 )
		   {
			   temp--;
		   }
		   else
		   {
			   temp=MAX_FLOW/1000;
			   
		   }
		   systems.a_flow += temp * 1000;
		   
		   if ( systems.a_flow < MIN_FLOW )
		   {
			   systems.a_flow = MIN_FLOW;			   
		   }
		   
		   if ( systems.a_flow > MAX_FLOW )
		   {
			   systems.a_flow=MAX_FLOW;            
		   }
           setFlow(64,18,systems.a_flow,1); 
		   break;
		   
		case 7:
	        temp1 = (unsigned int)(systems.a_flow / 1000); 
			temp  = (unsigned int)(( systems.a_flow % 1000 ) / 100 );
			systems.a_flow -= temp * 100;
			if(temp1==MAX_FLOW/1000)
			{				
				if(temp>MID_FLOW/100)
				{ 					
#if        			MID_FLOW <100    					
					temp--;
#else   			
					
					temp-=(MID_FLOW/100);
#endif        
					
				}
				else
				{
					temp=(MAX_FLOW%1000)/100;
				}
			}
			else
			{
				
				if(temp>MID_FLOW/100)
				{  					
#if        			MID_FLOW <100  					
					temp--;
#else             					
					temp-=(MID_FLOW/100);
#endif          
					
				}
				else
				{
					temp=(MAX_FLOW%1000)/100;
				}				
			} 
			systems.a_flow += temp * 100;
			
			if ( systems.a_flow < MIN_FLOW )
			{
				systems.a_flow = MIN_FLOW;
			}
			
			if ( systems.a_flow > MAX_FLOW )
			{            
				systems.a_flow = MAX_FLOW;           
			} 
			setFlow(64,18, systems.a_flow,2);
			break;
		
		case 8:
#if  		MID_FLOW <100    	
		
			temp  = (unsigned int)(( systems.a_flow % 100 ) / 10 );
			systems.a_flow -= temp * 10;
			
			if ( 0 == temp )
			{
				temp = 6;
			}
			temp--;

			systems.a_flow += temp * 10;
			
			if ( systems.a_flow > MAX_FLOW )
			{
				systems.a_flow = MAX_FLOW; 				
			}
        
			if ( systems.a_flow < MIN_FLOW )
			{     
				systems.a_flow = MIN_FLOW;            
			}
			setFlow(64,18, systems.a_flow,3);
        
#endif  
       		break; 
			
	case 9:
		temp = (unsigned char)(systems.a_hour/10);		
		systems.a_hour -= temp*10;
		if(temp == 0)
		{
			temp = 2;
		}
		temp--;
		
		systems.a_hour = temp*10+systems.a_hour;	
		setTime(64,32,systems.a_hour,systems.a_min,1);
		break;
			
	case 10:
		temp = (unsigned char)(systems.a_hour%10);
		systems.a_hour -= temp;
		if(temp == 0)
		{
			temp = 4;
		}	
		temp--;
		
		systems.a_hour = systems.a_hour+temp;	
		setTime(64,32,systems.a_hour,systems.a_min,2);
		break;
		
	case 11:
		temp = (unsigned char)(systems.a_min/10);
		systems.a_min -= 10*temp;
		if(temp == 0)
		{
			temp = 6;
		}
		temp--;
		systems.a_min = temp*10+systems.a_min;	
		setTime(64,32,systems.a_hour,systems.a_min,3);
		break;

	case 12:
		temp = (unsigned char)(systems.a_min%10);
		systems.a_min -= temp;
		if(temp == 0)
		{
			temp = 10;
		}
		temp--;
		
		systems.a_min =systems.a_min +temp;	
		setTime(64,32,systems.a_hour,systems.a_min,4);
		break;
		default: 
			break;
	}
}
void timeGroupParaSetMenuOnRight(void)
{
	struct MENU *lp;
	lp = win->curfous;
	
	switch ( lp->status1 )
	{
		case 4:
			lp->status1 = 5;			
			setTime(64,4,systems.temp_hour,systems.temp_min,6);
			break;
			
		case 6:
			lp->status1 = 7;
			setFlow(64,18,systems.a_flow,2);
			break;
			
		case 7:
#if  		MID_FLOW <100
			lp->status1 = 8;
			setFlow(64,18,systems.a_flow,3);
#endif
			break;
			
			
		case 9:
			lp->status1 = 10;			
			setTime(64,32,systems.a_hour,systems.a_min,2);
			break;
			
		case 10:
			lp->status1 = 11;
			setTime(64,32,systems.a_hour,systems.a_min,3);
			break;
		
		case 11:
			lp->status1 = 12;
			setTime(64,32,systems.a_hour,systems.a_min,4);
			break;	
			
		default:
			break;
	}
}

void timeGroupParaSetMenuOnLeft(void)
{
	struct MENU *lp;
	lp = win->curfous;
	
	switch ( lp->status1 )
	{		
		case 5:
			lp->status1 = 4; 
			setTime(64,4,systems.temp_hour,systems.temp_min,5);
			break;	

		case 7:
			lp->status1 = 6;
			setFlow(64,18,systems.a_flow,1);
			break;
			
#if  		MID_FLOW <100
			
		case 8:
			lp->status1 = 7;
			setFlow(64,18,systems.a_flow,3);
			break;			
#endif			
			
		case 10:
			lp->status1 = 9;
			setTime(64,32,systems.a_hour,systems.a_min,1);
			break;	
			
		case 11:
			lp->status1 = 10;			
			setTime(64,32,systems.a_hour,systems.a_min,2);
			break;
			
		case 12:
			lp->status1 = 11;
			setTime(64,32,systems.a_hour,systems.a_min,3);
			break;
		default:
			break;
	}

}

void timeGroupParaSetMenuOnOk(void)
{
	struct MENU *lp;
	int index;		// 索引组号
	unsigned int set_group_start_time;		// 当前组采样启动时间(min)
	unsigned int set_group_complete_time;	// 当前组采样完成时间(min)
	unsigned int index_group_start_time;	// 索引组采样启动时间(min)
	unsigned int index_group_complete_time;	// 索引组采样完成时间(min)
	unsigned char flag = 0;				// 冲突标志
	
	lp = win->curfous;	
	
	/*if ( systems.groups[systems.set_group].smp_time )
	{
		systems.a_min = systems.groups[systems.set_group].smp_time % 60;
		systems.a_hour = systems.groups[systems.set_group].smp_time / 60;
	}
	else
	{
		systems.a_hour = 0;
		systems.a_min = 10;
	}
		*/
	switch ( lp->status1 )
	{
	case 0:					// 获得“定时”焦点
		lp->status1 = 4;
		print_ch_str( 34, 4, "定时", 0 );	
		setTime(64,4,systems.temp_hour,systems.temp_min,5);
		break;
		
	case 1:					// 获得“流量”焦点
		lp->status1 = 6;
		print_ch_str( 34, 18, "流量", 0 );		
		setFlow(64,18,systems.a_flow,1);
		break;
		
	case 2:					// 获得“时间”焦点
		lp->status1 = 9;
		print_ch_str( 34, 32, "时长", 0 );		
		setTime(64,32,systems.a_hour,systems.a_min,1);
		break;
		
	case 3:	        
     if(systems.status == STOP)   
     {      // 获得“启用”焦点
		if ( 0 == systems.groups[systems.set_group].srb )	// 判断当前组是否已经启用
		{
			systems.groups[systems.set_group].start_time.hour = systems.temp_hour;  // 确认启动采样时间
			systems.groups[systems.set_group].start_time.min = systems.temp_min;
			systems.groups[systems.set_group].smp_time = (unsigned long)systems.a_hour*60 + systems.a_min;  // 确认启动采样时间
			
			// 计算当前组采样开始时间(min)
			set_group_start_time = systems.groups[systems.set_group].start_time.hour * 60
								+ systems.groups[systems.set_group].start_time.min;

			// 计算当前组采样完成时间(min)
			set_group_complete_time = systems.groups[systems.set_group].start_time.hour * 60
								   + systems.groups[systems.set_group].start_time.min
								   + systems.groups[systems.set_group].smp_time;

			if(set_group_complete_time <= set_group_start_time)
			{
				break;
			}
			for ( index=0; /*systems.set_group != index
							&&*/ index < GROUP_NUM; index++ )
			{
                if ( systems.set_group == index ) continue; //不能够和自己比较
				// 计算索引组采样开始时间(min)
				index_group_start_time = systems.groups[index].start_time.hour * 60
								+ systems.groups[index].start_time.min;

				// 计算索引组采样完成时间(min)
				index_group_complete_time = systems.groups[index].start_time.hour * 60
								+ systems.groups[index].start_time.min
								+ systems.groups[index].smp_time;

				// 判断当前组于其他的组是否有冲突
				if ( (( set_group_start_time <= index_group_start_time
					      && set_group_complete_time >= index_group_start_time )
                     || ( set_group_start_time >= index_group_start_time
					      && set_group_start_time <= index_group_complete_time ) )
					 && 1 == systems.groups[index].srb )
				{	// 发生冲突
					flag = 1;
					systems.err_group_num = index;		// 保存冲突组号
					break;
				}
			}            
			if ( 1 == flag )	// 发生冲突
			{
				if ( lp->subMenu )
				{
					win->curfous = lp->subMenu;
					win->curfous->show();
				}
			}
			else	// 没有组发生冲突
			{
				systems.groups[systems.set_group].srb = 1;	// 启用改组
				
				draw_symbol( 91, 47, 10, 10, gou_mu, 0 );				
			}
		}
		else		// 当前组已经启用
		{
			systems.groups[systems.set_group].srb = 0;	// 将当前组设置为“未启用”
            clr_rect( 90, 46, 12, 12 );
            draw_rect( 90, 46, 12, 12 );
		}
	 }
	 else 
	 {
         //启动状态 		 
		 if ( lp->subMenu )
		 {
			 win->curfous = lp->subMenu->rMenu;
			 win->curfous->show();
		 }
	 }     
		break;
	 
		case 4:
		case 5:
			lp->status1 = 1;
			systems.groups[systems.set_group].start_time.hour = systems.temp_hour;  // 确认启动采样时间
			systems.groups[systems.set_group].start_time.min = systems.temp_min;
			print_ch_str( 34, 18, "流量", 1 );		
			setTime(64,4,systems.temp_hour,systems.temp_min,0);
			break;
			
		case 6:
		case 7:
		case 8:		
			lp->status1 = 2;
			systems.groups[systems.set_group].flow = systems.a_flow;
			print_ch_str( 34, 32, "时长", 1 );	
			setFlow(64,18,systems.a_flow,0);
			break;
			
		case 9:
		case 10:
		case 11:
		case 12:
			lp->status1 = 3;
			systems.groups[systems.set_group].smp_time = (unsigned long)systems.a_hour*60 + systems.a_min;  // 确认启动采样时间
			print_ch_str( 34, 46, "启用", 1 );	
			setTime(64,32,systems.a_hour,systems.a_min,0);
			break;
			
		 default: 
		 	break;
	}
}

void timeGroupParaSetMenuOnEsc(void)
{
	struct MENU *lp;
	lp = win->curfous;
	eeprom_WriteGroupSetPara(systems.groups[systems.set_group],systems.set_group);
	
	if ( lp->topMenu->rMenu )
	{
		win->curfous = lp->topMenu->rMenu ;
		win->curfous->show();
   
	}
}


void groupStartWarnMenuShow(void)
{
     struct MENU *lp;
	 lp = win->curfous;
     clr_lcd();
	 draw_rect( 0, 0, 127, 63 );
	 print_ch_str( 26, 8, "定时模式已启动", 0 );
	 print_ch_str( 20, 22, "是否停止继续设置", 0 );
      	 
	 switch ( lp->status1 )
	 {
		case 0:
			draw_button( 15, 45, "确定", 1, CHINESE );
			draw_button( 80, 45, "取消", 0, CHINESE );
			break;
		case 1:
			draw_button( 15, 45, "确定", 0, CHINESE );
			draw_button( 80, 45, "取消", 1, CHINESE );
			break;
		default: break;
     }
}
void groupStartWarnMenuOkKey(void)
{
    struct MENU *lp;
   lp = win->curfous;
   if(lp->status1==0)
   {
        systems.status = STOP;        //进入停止状态
		systems.used_time = 0;
		systems.used_sec = 0;
		systems.percent = 0;
        
        stopPump();
        systems.dynamic_flow = 0;
        systems.dynamic_volume = 0;
		systems.dynamic_cail_volume = 0;
   
   }
   
    if ( lp->topMenu )                //返回上一层
    {
		win->curfous = lp->topMenu;
		win->curfous->show();
    }

}
void groupStartWarnMenuLeftKey(void)
{
   struct MENU *lp;
   lp = win->curfous;
    if(lp->status1==0)
   {
    lp->status1=1;  
    draw_button( 15, 45, "确定", 0, CHINESE );
	draw_button( 80, 45, "取消", 1, CHINESE );
   }
   else
   {
     lp->status1=0;
     draw_button( 15, 45, "确定", 1, CHINESE );
	 draw_button( 80, 45, "取消", 0, CHINESE );
   }
   
    
}
void groupStartWarnMenuRightKey(void)
{
  groupStartWarnMenuLeftKey();
    
}
void groupStartWarnMenuEsc(void)
{
   struct MENU *lp;
	lp = win->curfous;
	if ( lp->topMenu )
	{
		win->curfous = lp->topMenu;
		win->curfous->show();
   
	} 
    
    
}

/*
 * 定时模式定时组工作时间设置菜单
 */
void groupWorkTimeSetMenuShow(void)
{
	struct MENU *lp;
	unsigned int bai, shi, ge;		// 设置采样时长的百位十位个位

	lp = win->curfous;

	// 获取默认设置值
	systems.temp_hour = systems.groups[systems.set_group].start_time.hour;
	systems.temp_min = systems.groups[systems.set_group].start_time.min;
	if ( systems.groups[systems.set_group].smp_time )
	{
		systems.a_min = systems.groups[systems.set_group].smp_time;
	}
	else
	{
		systems.a_min = 10;
	}

	bai = (unsigned int)(systems.a_min / 100);
	shi = (unsigned int)(systems.a_min %100 / 10);
	ge = (unsigned int)(systems.a_min % 10);

	clr_lcd();
	draw_rect( 0, 0, 127, 63 );
	draw_line( 20, 15, 107, 15 );
	
		print_ch_str( 28, 3, "工作时间设置", 0 );
		putchar( 68, 31, ':', 0 );
		putchar( 68, 17, ':', 0 );
		print_str( 100, 31, "min", 0 );
		switch ( lp->status1 )
		{
		case 0:		// 启动时间文本获得焦点
			print_ch_str( 20, 17, "启动时间", 1 );
			print_time( systems.temp_hour, systems.temp_min, 76, 17, HOUR_MIN );
			print_ch_str( 20, 31, "采样时长", 0 );
			putchar( 76, 31, bai+'0', 0 );
			putchar( 84, 31, shi+'0', 0 );
			putchar( 92, 31, ge+'0', 0 );
			draw_button( 15, 45, "确定", 0, CHINESE );
			draw_button( 81, 45, "取消", 0, CHINESE );
			break;
		case 1:		// 启动时间的时获得焦点
			print_ch_str( 20, 17, "启动时间", 0 );
			print_time( systems.temp_hour, systems.temp_min, 76, 17, HOUR );
			print_ch_str( 20, 31, "采样时长", 0 );
			putchar( 76, 31, bai+'0', 0 );
			putchar( 84, 31, shi+'0', 0 );
			putchar( 92, 31, ge+'0', 0 );
			draw_button( 15, 45, "确定", 0, CHINESE );
			draw_button( 81, 45, "取消", 0, CHINESE );
			break;
		case 2:		// 启动时间的分获得焦点
			print_ch_str( 20, 17, "启动时间", 0 );
			print_time( systems.temp_hour, systems.temp_min, 76, 17, MIN );
			print_ch_str( 20, 31, "采样时长", 0 );
			putchar( 76, 31, bai+'0', 0 );
			putchar( 84, 31, shi+'0', 0 );
			putchar( 92, 31, ge+'0', 0 );
			draw_button( 15, 45, "确定", 0, CHINESE );
			draw_button( 81, 45, "取消", 0, CHINESE );
			break;
		case 3:		// 采样时长文本获得焦点
			print_ch_str( 20, 17, "启动时间", 0 );
			print_time( systems.temp_hour, systems.temp_min, 76, 17, HOUR_MIN );
			print_ch_str( 20, 31, "采样时长", 1 );
			putchar( 76, 31, bai+'0', 0 );
			putchar( 84, 31, shi+'0', 0 );
			putchar( 92, 31, ge+'0', 0 );
			draw_button( 15, 45, "确定", 0, CHINESE );
			draw_button( 81, 45, "取消", 0, CHINESE );
			break;
		case 4:		// 采样时长百位获得焦点
			print_ch_str( 20, 17, "启动时间", 0 );
			print_time( systems.temp_hour, systems.temp_min, 76, 17, HOUR_MIN );
			print_ch_str( 20, 31, "采样时长", 0 );
			putchar( 76, 31, bai+'0', 1 );
			putchar( 84, 31, shi+'0', 0 );
			putchar( 92, 31, ge+'0', 0 );
			draw_button( 15, 45, "确定", 0, CHINESE );
			draw_button( 81, 45, "取消", 0, CHINESE );
			break;
		case 5:		// 采样时长十位获得焦点
			print_ch_str( 20, 17, "启动时间", 0 );
			print_time( systems.temp_hour, systems.temp_min, 76, 17, HOUR_MIN );
			print_ch_str( 20, 31, "采样时长", 0 );
			putchar( 76, 31, bai+'0', 0 );
			putchar( 84, 31, shi+'0', 1 );
			putchar( 92, 31, ge+'0', 0 );
			draw_button( 15, 45, "确定", 0, CHINESE );
			draw_button( 81, 45, "取消", 0, CHINESE );
			break;
		case 6:		// 采样时长个位获得焦点
			print_ch_str( 20, 17, "启动时间", 0 );
			print_time( systems.temp_hour, systems.temp_min, 76, 17, HOUR_MIN );
			print_ch_str( 20, 31, "采样时长", 0 );
			putchar( 76, 31, bai+'0', 0 );
			putchar( 84, 31, shi+'0', 0 );
			putchar( 92, 31, ge+'0', 1 );
			draw_button( 15, 45, "确定", 0, CHINESE );
			draw_button( 81, 45, "取消", 0, CHINESE );
			break;
		case 7:		// 确定按钮获得焦点
			print_ch_str( 20, 17, "启动时间", 0 );
			print_time( systems.temp_hour, systems.temp_min, 76, 17, HOUR_MIN );
			print_ch_str( 20, 31, "采样时长", 0 );
			putchar( 76, 31, bai+'0', 0 );
			putchar( 84, 31, shi+'0', 0 );
			putchar( 92, 31, ge+'0', 0 );
			draw_button( 15, 45, "确定", 1, CHINESE );
			draw_button( 81, 45, "取消", 0, CHINESE );
			break;
		case 8:		// 取消按钮获得焦点
			print_ch_str( 20, 17, "启动时间", 0 );
			print_time( systems.temp_hour, systems.temp_min, 76, 17, HOUR_MIN );
			print_ch_str( 20, 31, "采样时长", 0 );
			putchar( 76, 31, bai+'0', 0 );
			putchar( 84, 31, shi+'0', 0 );
			putchar( 92, 31, ge+'0', 0 );
			draw_button( 15, 45, "确定", 0, CHINESE );
			draw_button( 81, 45, "取消", 1, CHINESE );
			break;
		default: break;
		}
}

void groupWorkTimeSetMenuOnUp(void)
{
	struct MENU *lp;
	unsigned int bai, shi, ge;		// 设置采样时长的百位十位个位

	lp = win->curfous;

	switch ( lp->status1 )
	{
	case 0:		// 启动时间文本获得焦点
		lp->status1 = 7;
		
			print_ch_str( 20, 17, "启动时间", 0 );
			draw_button( 15, 45, "确定", 1, CHINESE );

		break;
	case 1:		// 启动时间的时获得焦点
		if ( systems.temp_hour >= 23 )
		{
			systems.temp_hour = 0;
		}
		else
		{
			systems.temp_hour++;
		}
		print_time( systems.temp_hour, systems.temp_min, 76, 17, HOUR );
		break;
	case 2:		// 启动时间的分获得焦点
		if ( 59 == systems.temp_min )
		{
			systems.temp_min = 0;
		}
		else
		{
			systems.temp_min++;
		}
		print_time( systems.temp_hour, systems.temp_min, 76, 17, MIN );
		break;
	case 3:		// 采样时长文本获得焦点
		lp->status1 = 0;
		
			print_ch_str( 20, 17, "启动时间", 1 );
			print_ch_str( 20, 31, "采样时长", 0 );

		break;
	case 4:		// 采样时长百位获得焦点
		bai = (unsigned int)( systems.a_min / 100 );
		systems.a_min -= bai * 100;
		if ( bai == 9 )
		{
			bai = 0;
		}
		else
		{
			bai++;
		}
		systems.a_min += bai * 100;
        if(systems.a_min==0)
        {
           systems.a_min=1;
           putchar( 92, 31, 1+'0', 0 );
        }   
        
        
		putchar( 76, 31, bai+'0', 1 );    // 刷新百位上的值
		break;
	case 5:		// 采样时长十位获得焦点
		shi = (unsigned int)( systems.a_min % 100 / 10 );
		systems.a_min -= shi * 10;
		if ( shi == 9 )
		{
			shi = 0;
		}
		else
		{
			shi++;
		}
		systems.a_min += shi * 10;
         if(systems.a_min==0)
        {
           systems.a_min=1;
           putchar( 92, 31, 1+'0', 0 );
        }
        
		putchar( 84, 31, shi+'0', 1 );
		break;
	case 6:		// 采样时长个位获得焦点
		ge = (unsigned int)( systems.a_min % 10 );
		systems.a_min -= ge;
		if ( ge == 9 )
		{ if(systems.a_min/10==0)
           {
              ge=1;
           }
          else
			ge = 0;
		}
		else
		{
			ge++;
		}
		systems.a_min += ge;
        
        
		putchar( 92, 31, ge+'0', 1 );
		break;
	case 7:		// 确定按钮获得焦点
		lp->status1 = 3;
		
			print_ch_str( 20, 31, "采样时长", 1 );
			draw_button( 15, 45, "确定", 0, CHINESE );

		break;
	case 8:		// 取消按钮获得焦点
		break;
	default: break;
	}
}

void groupWorkTimeSetMenuOnDown(void)
{
	struct MENU *lp;
	unsigned int bai, shi, ge;		// 设置采样时长的百位十位个位

	lp = win->curfous;

	switch ( lp->status1 )
	{
	case 0:		// 启动时间文本获得焦点
		lp->status1 = 3;
		
			print_ch_str( 20, 17, "启动时间", 0 );
			print_ch_str( 20, 31, "采样时长", 1 );

		break;
	case 1:		// 启动时间的时获得焦点
		if ( 0 == systems.temp_hour )
		{
			systems.temp_hour = 23;
		}
		else
		{
			systems.temp_hour--;
		}
		print_time( systems.temp_hour, systems.temp_min, 76, 17, HOUR );
		break;
	case 2:		// 启动时间的分获得焦点
		if ( 0 == systems.temp_min )
		{
			systems.temp_min = 59;
		}
		else
		{
			systems.temp_min--;
		}
		print_time( systems.temp_hour, systems.temp_min, 76, 17, MIN );
		break;
	case 3:		// 采样时长文本获得焦点
		lp->status1 = 7;
		
			print_ch_str( 20, 31, "采样时长", 0 );
			draw_button( 15, 45, "确定", 1, CHINESE );
	
		break;
	case 4:		// 采样时长百位获得焦点
		bai = (unsigned int)( systems.a_min / 100 );
		systems.a_min -= bai * 100;
		if ( bai == 0 )
		{
			bai = 9;
		}
		else
		{
			bai--;
		}
		systems.a_min += bai * 100;
        
        if(systems.a_min==0)
        {
           systems.a_min=1;
           putchar( 92, 31, 1+'0', 0 );
        }
		putchar( 76, 31, bai+'0', 1 );    // 刷新百位上的值
		break;
	case 5:		// 采样时长十位获得焦点
		shi = (unsigned int)( systems.a_min % 100 / 10 );
		systems.a_min -= shi * 10;
		if ( shi == 0 )
		{
			shi = 9;
		}
		else
		{
			shi--;
		}
		systems.a_min += shi * 10;
        if(systems.a_min==0)
        {
           systems.a_min=1;
           putchar( 92, 31, 1+'0', 0 );
        }
        
		putchar( 84, 31, shi+'0', 1 );
		break;
	case 6:		// 采样时长个位获得焦点
		ge = (unsigned int)( systems.a_min % 10 );
		systems.a_min -= ge;
		if(ge>0)
        {
          ge--;
          if(systems.a_min/10==0&&ge==0)
           {
              ge=9;
           }
          else
			ge = 0;
        }
		else
        {
          ge=9;
        } 
        
		systems.a_min += ge;
		putchar( 92, 31, ge+'0', 1 );
		break;
        
        
	case 7:		// 确定按钮获得焦点
		lp->status1 = 0;

			print_ch_str( 20, 17, "启动时间", 1 );
			draw_button( 15, 45, "确定", 0, CHINESE );
		break;
	case 8:		// 取消按钮获得焦点
		break;
	default: break;
	}
}

void groupWorkTimeSetMenuOnLeft(void)
{
	struct MENU *lp;
	unsigned int bai, shi, ge;

	lp = win->curfous;

	switch ( lp->status1 )
	{
	case 1:
		lp->status1 = 0;
		print_time( systems.temp_hour, systems.temp_min, 76, 17, HOUR_MIN );

			print_ch_str( 20, 17, "启动时间", 1 );

		break;
	case 2:
		lp->status1 = 1;
		print_time( systems.temp_hour, systems.temp_min, 76, 17, HOUR );
		break;
	case 4:
		lp->status1 = 3;
		bai = (unsigned int)( systems.a_min / 100 );
		clr_rect( 75, 31, 8, 12 );
		putchar( 76, 31, bai+'0', 0 );

			print_ch_str( 20, 31, "采样时长", 1 );

		break;
	case 5:
		lp->status1 = 4;
		bai = (unsigned int)( systems.a_min / 100 );
		shi = (unsigned int)( systems.a_min % 100 / 10 );
		fill_rect( 75, 31, 8, 12 );
		putchar( 76, 31, bai+'0', 1 );
		clr_rect( 83, 31, 8, 12 );
		putchar( 84, 31, shi+'0', 0 );
		break;
	case 6:
		lp->status1 = 5;
		shi = (unsigned int)( systems.a_min % 100 / 10 );
		ge = (unsigned int)( systems.a_min % 10 );
		fill_rect( 83, 31, 8, 12 );
		putchar( 84, 31, shi+'0', 1 );
		clr_rect( 91, 31, 8, 12 );
		putchar( 92, 31, ge+'0', 0 );
		break;
	case 7:
		lp->status1 = 8;

			draw_button( 15, 45, "确定", 0, CHINESE );
			draw_button( 81, 45, "取消", 1, CHINESE );

		break;
	case 8:
		lp->status1 = 7;

			draw_button( 15, 45, "确定", 1, CHINESE );
			draw_button( 81, 45, "取消", 0, CHINESE );

		break;
	default: break;
	}
}

void groupWorkTimeSetMenuOnRight(void)
{
	struct MENU *lp;
	unsigned int bai, shi, ge;

	lp = win->curfous;

	switch ( lp->status1 )
	{
	case 0:
		lp->status1 = 1;
		print_time( systems.temp_hour, systems.temp_min, 76, 17, HOUR );

			print_ch_str( 20, 17, "启动时间", 0 );

		break;
	case 1:
		lp->status1 = 2;
		print_time( systems.temp_hour, systems.temp_min, 76, 17, MIN );
		break;
	case 3:
		lp->status1 = 4;
		bai = (unsigned int)( systems.a_min / 100 );
		fill_rect( 75, 31, 8, 12 );
		putchar( 76, 31, bai+'0', 1 );

			print_ch_str( 20, 31, "采样时长", 0 );

		break;
	case 4:
		lp->status1 = 5;
		bai = (unsigned int)( systems.a_min / 100 );
		shi = (unsigned int)( systems.a_min % 100 / 10 );
		clr_rect( 75, 31, 8, 12 );
		putchar( 76, 31, bai+'0', 0 );
		fill_rect( 83, 31, 8, 12 );
		putchar( 84, 31, shi+'0', 1 );
		break;
	case 5:
		lp->status1 = 6;
		shi = (unsigned int)( systems.a_min % 100 / 10 );
		ge = (unsigned int)( systems.a_min % 10 );
		clr_rect( 83, 31, 8, 12 );
		putchar( 84, 31, shi+'0', 0 );
		fill_rect( 91, 31, 8, 12 );
		putchar( 92, 31, ge+'0', 1 );
		break;
	case 7:
		lp->status1 = 8;

			draw_button( 15, 45, "确定", 0, CHINESE );
			draw_button( 81, 45, "取消", 1, CHINESE );

		break;
	case 8:
		lp->status1 = 7;
		
			draw_button( 15, 45, "确定", 1, CHINESE );
			draw_button( 81, 45, "取消", 0, CHINESE );
	
		break;
	default: break;
	}
}

void groupWorkTimeSetMenuOnOk(void)
{
	struct MENU *lp;
	int index;		// 索引组号
	unsigned int set_group_start_time;		// 当前组采样开始时间(min)
	unsigned int set_group_complete_time;	// 当前组采样完成时间(min)
	unsigned int index_group_start_time;	// 索引组采样启动时间(min)
	unsigned int index_group_complete_time;	// 索引组采样结束时间(min)
	unsigned char flag = 0;				// 冲突标志

	lp = win->curfous;

	if ( 7 == lp->status1 )
	{
		if ( 0 == systems.a_min )	// 流量为空
		{
			if ( lp->subMenu )
			{
				win->curfous = lp->subMenu;
				win->curfous->show();
			}
		}
		else
		{
			if ( 1 == systems.groups[systems.set_group].srb )	// 当前组已设置为启用组
			{
				// 计算当前组采样启动时间(min)
				set_group_start_time = systems.temp_hour * 60 + systems.temp_min;

				// 计算当前组采样完成时间(min)
				set_group_complete_time = systems.temp_hour * 60
										+ systems.temp_min + systems.a_min;

				for ( index=0; /*index != systems.set_group &&*/ index < GROUP_NUM; index++ )
				{
                    if ( systems.set_group == index ) continue;  //不能和自己进行比较
					// 计算索引组采样启动时间(min)
					index_group_start_time = systems.groups[index].start_time.hour * 60
									+ systems.groups[index].start_time.min;

					// 计算索引组采样完成时间(min)
					index_group_complete_time = systems.groups[index].start_time.hour * 60
									+ systems.groups[index].start_time.min
									+ systems.groups[index].smp_time;

					if (( set_group_start_time <= index_group_start_time
							&& set_group_complete_time >= index_group_start_time )
                        || ( set_group_start_time >= index_group_start_time
							&& set_group_start_time <= index_group_complete_time )
                        && 1 == systems.groups[index].srb )
					{
						flag = 1;		// 发生冲突
						systems.err_group_num = index;	// 保存发生冲突的组号
						break;
					}
				}
				if ( 1 == flag )		// 已发生冲突
				{
					if ( lp->subMenu )
					{
						win->curfous = lp->subMenu;
						win->curfous->show();
					}
				}
				else
				{
					systems.groups[systems.set_group].start_time.hour = systems.temp_hour;  // 确认启动采样时间
					systems.groups[systems.set_group].start_time.min = systems.temp_min;
					systems.groups[systems.set_group].smp_time = systems.a_min;   // 确认采样时长
					if ( lp->topMenu )
					{
						systems.temp_hour = 0;		// 恢复默认设置值
						systems.temp_min = 0;		// 恢复默认设置值
						systems.a_min = 1;			// 恢复默认设置值

						win->curfous = lp->topMenu;
						win->curfous->show();
					}
				}
			}
			else	// 当前组没有设置为启用组
			{
				systems.groups[systems.set_group].start_time.hour = systems.temp_hour;  // 确认启动采样时间
				systems.groups[systems.set_group].start_time.min = systems.temp_min;
				systems.groups[systems.set_group].smp_time = systems.a_min;   // 确认采样时长
				if ( lp->topMenu )
				{
					systems.temp_hour = 0;		// 恢复默认设置值
					systems.temp_min = 0;		// 恢复默认设置值
					systems.a_min = 1;			// 恢复默认设置值

					win->curfous = lp->topMenu;
					win->curfous->show();
				}
			}
		}
	}
	else if ( 8 == lp->status1 )
	{
		if ( lp->topMenu )
		{
			win->curfous = lp->topMenu;
			win->curfous->show();
		}
	}
}

void groupWorkTimeSetMenuOnEsc(void)
{
	struct MENU *lp;
	lp = win->curfous;
	if ( lp->topMenu )
	{
		systems.temp_hour = 0;		// 恢复默认设置值
		systems.temp_min = 0;		// 恢复默认设置值
		systems.a_min = 1;			// 恢复默认设置值

		win->curfous = lp->topMenu;
		win->curfous->show();
	}
}

void groupWorkTimeSetMenuExec(void)
{

}

/*
 * 组冲突
 */
void groupWarnMenuShow(void)
{

	unsigned int tmp_hour, tmp_min;
	unsigned int smp_time;

	clr_lcd();
	draw_rect( 0, 0, 127, 63 );
	draw_line( 20, 15, 107, 15 );
	
		tmp_hour = systems.groups[systems.err_group_num].start_time.hour;  // 冲突的组时间
		tmp_min = systems.groups[systems.err_group_num].start_time.min;
		smp_time = systems.groups[systems.err_group_num].smp_time;	// 冲突组的采样时长

		
			print_ch_str( 53, 3, "警告", 0 );
			print_str( 83, 3, "!", 0 );
			print_ch_str( 22, 17, "时间与组", 0 );
			print_int( 72, 17, systems.err_group_num + 1, 0 );
			print_ch_str( 85, 17, "冲突", 0 );
			print_ch_str( 2, 30, "组", 0 );
			print_int( 18, 30, systems.err_group_num + 1, 0 );
			print_ch_str( 30, 30, "时间", 0 );
			print_time( tmp_hour, tmp_min, 54, 30, HOUR_MIN );
			putchar( 94, 30, ',', 0 );
			print_int( 100, 30, smp_time, 0 );
			print_ch_str( 2, 48, "请重新设置", 0 );
			draw_button( 92, 44, "确定", 1, CHINESE );

	

}

void groupWarnMenuOnOk(void)
{
	struct MENU *lp;
	lp = win->curfous;
	if ( lp->topMenu )
	{
		win->curfous = lp->topMenu;
		win->curfous->show();
	}
}

/*
 * 定时模式定时组流量设置菜单
 */
void groupFlowSetMenuShow(void)
{
	//struct MENU *lp;
	

	//lp = win->curfous;
	if ( systems.groups[systems.set_group].flow )
	{
		systems.a_flow = systems.groups[systems.set_group].flow;
	}
	
}

void groupFlowSetMenuOnUp(void)
{
	struct MENU *lp;
	lp = win->curfous;
	flowSetUp( lp );
}

void groupFlowSetMenuOnDown(void)
{
	struct MENU *lp;
	lp = win->curfous;
	flowSetDown( lp );
}

void groupFlowSetMenuOnLeft(void)
{
	struct MENU *lp;
	lp = win->curfous;
	flowSetLeft( lp );
}

void groupFlowSetMenuOnRight(void)
{
	struct MENU *lp;
	lp = win->curfous;
	flowSetRight( lp );
}

void groupFlowSetMenuOnOk(void)
{
	struct MENU *lp;
	lp = win->curfous;
	if ( 3 == lp->status1 )		// 确定获得焦点
	{
		if ( systems.a_flow < 40 || systems.a_flow > MAX_FLOW )
		{
			if ( lp->subMenu )
			{
				win->curfous = lp->subMenu;		// 进入错误警告菜单
				win->curfous->show();
			}
		}
		else
		{
			systems.groups[systems.set_group].flow = systems.a_flow;	// 确认当前组的流量设置            
			systems.a_flow = ORIGIN_FLOW;
            changeFlow( systems.groups[systems.set_group].flow );
            
			if ( lp->topMenu )
			{
				win->curfous = lp->topMenu;
				win->curfous->show();
			}
		}
	}
	else if ( 4 == lp->status1 )		// 取消获得焦点
	{
		if ( lp->topMenu )
		{
			win->curfous = lp->topMenu;
			win->curfous->show();
		}
	}
}

void groupFlowSetMenuOnEsc(void)
{
	struct MENU *lp;
	lp = win->curfous;
	if ( lp->topMenu )
	{
		win->curfous = lp->topMenu;
		win->curfous->show();
	}
}

void groupFlowSetMenuExec(void)
{

}




/*
 * 循环模式参数设置菜单
 */
void cylParaSetMenuShow(void)
{
	struct MENU *lp;
	lp = win->curfous;
	clr_lcd();
    
    draw_rectmod();

	draw_symbol(7,4,14,27,xunhuan_mu,0);
	draw_symbol(7,33,14,27,canshu_mu,0);
	
	putchar(58,4,':',0);
	putchar(58,18,':',0);
	putchar(58,32,':',0);
	putchar(58,46,':',0);
	print_str(64+24,4,"ml/min",0);
	print_str(64+12, 18, "h  m", 0 );	
	print_str(64+12, 32, "h  m", 0 );	
	
	if ( systems.cyls.flow )
	{
		systems.a_flow = systems.cyls.flow;
	}
	else
	{
		systems.a_flow = ORIGIN_FLOW;
	}
	systems.temp_hour = systems.cyls.mid_time / 60;		
	systems.temp_min = systems.cyls.mid_time % 60;
	
	if(systems.cyls.smp_time)
	{
		systems.a_hour = systems.cyls.smp_time / 60;		
		systems.a_min = systems.cyls.smp_time % 60;
	}
	else
	{
		systems.a_hour = 0;
		systems.a_min = ORIGIN_SMP_TIME;
	}
	if((systems.cyls.cyl_num)&&(systems.cyls.cyl_num < 100))
	{
		systems.a_temp = systems.cyls.cyl_num;	
	}
	else
	{
		systems.a_temp = 1;
	}
	lp->status1 = 0;
	switch ( lp->status1 )
	{
		case 0:  //焦点在第一行处				
			print_ch_str( 34, 4, "流量", 1 );   // 打印第一行菜单标题
			print_ch_str( 34, 18, "间隔", 0 );  // 打印第二行菜单的标题
			print_ch_str( 34, 32, "时长", 0 );  // 打印第三行菜单的标题
			print_ch_str(34, 46, "次数", 0 );
			break;
			
		case 1:  //焦点在第二行处
			print_ch_str( 34, 4, "流量", 0 );   // 打印第一行菜单标题
          	print_ch_str( 34, 18, "间隔", 1 );  // 打印第二行菜单的标题
          	print_ch_str( 34, 32, "时长", 0 );  // 打印第三行菜单的标题
          	print_ch_str(34, 46, "循环", 0 );                
			break;
			
		case 2:  //焦点在第三行处
			print_ch_str( 34, 4, "流量", 0 );   // 打印第一行菜单标题
          	print_ch_str( 34, 18, "间隔", 0 );  // 打印第二行菜单的标题
          	print_ch_str( 34, 32, "时长", 1 );  // 打印第三行菜单的标题
          	print_ch_str(34, 46, "循环", 0 );    
			break;
			
        case 3:
			print_ch_str( 34, 4, "流量", 0 );   // 打印第一行菜单标题
          	print_ch_str( 34, 18, "间隔", 0 );  // 打印第二行菜单的标题
          	print_ch_str( 34, 32, "时长", 0 );  // 打印第三行菜单的标题
          	print_ch_str(34, 46, "循环", 1 );    
        	break;  
		default: 
			break;
	}
	setFlow(64,4,systems.a_flow,0);
	setTime(64,18,systems.temp_hour,systems.temp_min,0);	
	setTime(64,32,systems.a_hour,systems.a_min,0);	
	if(systems.a_temp > 9)
	{
		print_int(64,46,systems.a_temp,0);
	}
	else
	{
		putchar(64,46,'0',0);
		print_int(64+6,46,systems.a_temp,0);
	}
}

void cylParaSetMenuOnUp(void)
{
	struct MENU *lp;
	unsigned int temp = 0;
	unsigned int temp1 = 0;
	lp = win->curfous;


	switch ( lp->status1 )
	{
		case 0: 
			lp->status1=3;
			print_ch_str( 34, 4, "流量", 0);   // 打印第一行菜单标题
			print_ch_str( 34, 18, "间隔", 0 );  // 打印第二行菜单的标题
			print_ch_str( 34, 32, "时长", 0 );  // 打印第三行菜单的标题
			print_ch_str(34, 46, "次数", 1 );			
			break;
			
		case 1:
			lp->status1 = 0;
			print_ch_str( 34, 4, "流量", 1);   // 打印第一行菜单标题
			print_ch_str( 34, 18, "间隔", 0 );  // 打印第二行菜单的标题
			print_ch_str( 34, 32, "时长", 0 );  // 打印第三行菜单的标题
			print_ch_str(34, 46, "次数", 0 );	
			break;
			
		case 2:
			lp->status1 = 1;
			print_ch_str( 34, 4, "流量", 0);   // 打印第一行菜单标题
			print_ch_str( 34, 18, "间隔", 1 );  // 打印第二行菜单的标题
			print_ch_str( 34, 32, "时长", 0 );  // 打印第三行菜单的标题
			print_ch_str(34, 46, "次数", 0 );
			break;
			
        case 3:
            lp->status1 = 2;
			print_ch_str( 34, 4, "流量", 0);   // 打印第一行菜单标题
			print_ch_str( 34, 18, "间隔", 0 );  // 打印第二行菜单的标题
			print_ch_str( 34, 32, "时长", 1 );  // 打印第三行菜单的标题
			print_ch_str(34, 46, "次数", 0 );
            break;
			
		case 4:		
			temp = (unsigned int)(systems.a_flow / 1000);
			systems.a_flow -= temp * 1000;
			temp++;
			if(temp > MAX_FLOW/1000)
			{
				temp = 0;
			}			
			systems.a_flow  += temp * 1000;
			
			if( systems.a_flow > MAX_FLOW )
			{
				systems.a_flow = MAX_FLOW;				
			}
			
			if ( systems.a_flow < MIN_FLOW )
			{      
				systems.a_flow = MIN_FLOW;
			}
			setFlow(64,4,systems.a_flow,1);
			break;
			
		case 5:
			temp1 = (unsigned int)(systems.a_flow / 1000); 
			temp = (unsigned int)((systems.a_flow % 1000) / 100);
			systems.a_flow  -= temp * 100;
			
			if(temp1==MAX_FLOW/1000)					
			{
#if       		MID_FLOW <100				
				temp++;
				
#else        	
				temp+=(MID_FLOW/100);
				
#endif       				
          		if(temp>(MAX_FLOW%1000)/100)
          		{
           	 		temp=0;
              
          		}
			}
			else
			{
#if        		MID_FLOW <100  				
				temp++;
				
#else           
				temp+=(MID_FLOW/100);
					
#endif  
				if(10 == temp)
				{
					temp = 0;
				}	
			}
			
			systems.a_flow  += temp * 100;			
			
			if(systems.a_flow  < MIN_FLOW)
			{
				systems.a_flow  = MIN_FLOW;	
			}
        
			if(systems.a_flow  > MAX_FLOW)
			{            
				systems.a_flow  = MAX_FLOW;
			}	
			setFlow(64,4,systems.a_flow,2);
			break;	
	
		case 6:
#if  		MID_FLOW <100
		
			temp  = (unsigned int)((systems.a_flow % 100) / 10);
			systems.a_flow -= temp * 10;
			
			if(0 == temp)
			{
				temp = 5;
			}
			else
			{
				temp = 0;
			}
			
			systems.a_flow += temp * 10;
			
			if( systems.a_flow > MAX_FLOW )
			{
				systems.a_flow = MAX_FLOW;				
			}
			
			if ( systems.a_flow < MIN_FLOW )
			{      
				systems.a_flow = MIN_FLOW;
			}
			setFlow(64,4,systems.a_flow,3);        
#endif          
			break;
			
		case 7:
			temp = (unsigned char)(systems.temp_hour/10);		
			systems.temp_hour -= temp * 10;
			if(temp >= 9)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.temp_hour = systems.temp_hour + temp*10;		
            setTime(64,18,systems.temp_hour,systems.temp_min,1);
			break;
			
		case 8:	
			temp = (unsigned char)(systems.temp_hour%10);
			systems.temp_hour -= temp;
			if(temp >= 9)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.temp_hour = temp+systems.temp_hour;	
			setTime(64,18,systems.temp_hour,systems.temp_min,2);
			break;
			
		case 9:
			temp = (unsigned char)(systems.temp_min/10);	
			systems.temp_min -= temp * 10;
			if(temp >= 5)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.temp_min = temp*10+systems.temp_min;	
			setTime(64,18,systems.temp_hour,systems.temp_min,3);
			break;
		
		case 10:
			temp = (unsigned char)(systems.temp_min%10);	
			systems.temp_min -= temp;			
			if(temp >= 9)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.temp_min = systems.temp_min+temp;	
			setTime(64,18,systems.temp_hour,systems.temp_min,4);
			break;	
			
		case 11:
			temp = (unsigned char)(systems.a_hour/10);		
			systems.a_hour -= temp * 10;
			if(temp >= 9)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.a_hour = systems.a_hour + temp*10;	
            setTime(64,32,systems.a_hour,systems.a_min,1);
			break;
			
		case 12:	
			temp = (unsigned char)(systems.a_hour%10);
			systems.a_hour -= temp;
			if(temp >= 9)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.a_hour = temp+systems.a_hour;	
			setTime(64,32,systems.a_hour,systems.a_min,2);
			break;
			
		case 13:
			temp = (unsigned char)(systems.a_min/10);	
			systems.a_min -= temp * 10;
			if(temp >= 5)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.a_min = temp*10+ systems.a_min;	
			setTime(64,32,systems.a_hour,systems.a_min,3);
			break;
		
		case 14:
			temp = (unsigned char)(systems.a_min%10);	
			systems.a_min -= temp;			
			if(temp >= 9)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.a_min = systems.a_min+temp;	
			setTime(64,32,systems.a_hour,systems.a_min,4);
			break;		
		case 15:
			temp = (unsigned char)(systems.a_temp/10);
			temp1 = (unsigned char)(systems.a_temp%10);
			if(temp >= 9)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			putchar(64,46,temp+'0',1);
			putchar(70,46,temp1+'0',0);
			systems.a_temp = temp*10+temp1;
			break;	
		case 16:
			temp = (unsigned char)(systems.a_temp/10);
			temp1 = (unsigned char)(systems.a_temp%10);
			if(temp1 >= 9)
			{
				temp1 = 0;
			}
			else
			{
				temp1++;
			}
			putchar(64,46,temp+'0',0);
			putchar(70,46,temp1+'0',1);
			systems.a_temp = temp*10+temp1;
			break;	
		default: break;
	}
}

void cylParaSetMenuOnDown(void)
{
	struct MENU *lp;
	unsigned char temp = 0, temp1 = 0;
	
	lp = win->curfous;

	switch ( lp->status1 )
	{
		case 0: 
			lp->status1=1;
			print_ch_str( 34, 4, "流量", 0);   // 打印第一行菜单标题
			print_ch_str( 34, 18, "间隔", 1 );  // 打印第二行菜单的标题
			print_ch_str( 34, 32, "时长", 0 );  // 打印第三行菜单的标题
			print_ch_str(34, 46, "次数", 0 );			
			break;
			
		case 1:
			lp->status1 = 2;
			print_ch_str( 34, 4, "流量",0);   // 打印第一行菜单标题
			print_ch_str( 34, 18, "间隔", 0 );  // 打印第二行菜单的标题
			print_ch_str( 34, 32, "时长", 1 );  // 打印第三行菜单的标题
			print_ch_str(34, 46, "次数", 0 );	
			break;
			
		case 2:
			lp->status1 = 3;
			print_ch_str( 34, 4, "流量", 0);   // 打印第一行菜单标题
			print_ch_str( 34, 18, "间隔", 0);  // 打印第二行菜单的标题
			print_ch_str( 34, 32, "时长", 0 );  // 打印第三行菜单的标题
			print_ch_str(34, 46, "次数", 1 );
			break;
			
        case 3:
            lp->status1 = 0;
			print_ch_str( 34, 4, "流量", 1);   // 打印第一行菜单标题
			print_ch_str( 34, 18, "间隔", 0 );  // 打印第二行菜单的标题
			print_ch_str( 34, 32, "时长", 0 );  // 打印第三行菜单的标题
			print_ch_str(34, 46, "次数", 0 );
            break;
			
		case 4:
           temp = (unsigned int)(systems.a_flow / 1000);
		   systems.a_flow -= temp * 1000;
		   if ( temp > 0 )
		   {
			   temp--;
		   }
		   else
		   {
			   temp=MAX_FLOW/1000;
			   
		   }
		   systems.a_flow += temp * 1000;
		   
		   if ( systems.a_flow < MIN_FLOW )
		   {
			   systems.a_flow = MIN_FLOW;			   
		   }
		   
		   if ( systems.a_flow > MAX_FLOW )
		   {
			   systems.a_flow=MAX_FLOW;            
		   }
           setFlow(64,4,systems.a_flow,1); 
		   break;
		   
		case 5:
	        temp1 = (unsigned int)(systems.a_flow / 1000); 
			temp  = (unsigned int)(( systems.a_flow % 1000 ) / 100 );
			systems.a_flow -= temp * 100;
			if(temp1==MAX_FLOW/1000)
			{				
				if(temp>MID_FLOW/100)
				{ 					
#if        			MID_FLOW <100    					
					temp--;
#else   			
					
					temp-=(MID_FLOW/100);
#endif        
					
				}
				else
				{
					temp=(MAX_FLOW%1000)/100;
				}
			}
			else
			{
				
				if(temp>MID_FLOW/100)
				{  					
#if        			MID_FLOW <100  					
					temp--;
#else             					
					temp-=(MID_FLOW/100);
#endif          
					
				}
				else
				{
					temp=(MAX_FLOW%1000)/100;
				}				
			} 
			systems.a_flow += temp * 100;
			
			if ( systems.a_flow < MIN_FLOW )
			{
				systems.a_flow = MIN_FLOW;
			}
			
			if ( systems.a_flow > MAX_FLOW )
			{            
				systems.a_flow = MAX_FLOW;           
			} 
			setFlow(64,4, systems.a_flow,2);
			break;
		
		case 6:
#if  		MID_FLOW <100    	
		
			temp  = (unsigned int)(( systems.a_flow % 100 ) / 10 );
			systems.a_flow -= temp * 10;
			
			if ( 0 == temp )
			{
				temp = 6;
			}
			temp--;

			systems.a_flow += temp * 10;
			
			if ( systems.a_flow > MAX_FLOW )
			{
				systems.a_flow = MAX_FLOW; 				
			}
        
			if ( systems.a_flow < MIN_FLOW )
			{     
				systems.a_flow = MIN_FLOW;            
			}
			setFlow(64,4, systems.a_flow,3);
        
#endif  
       		break; 
			
		case 7:
			temp = (unsigned char)(systems.temp_hour/10);		
			systems.temp_hour -= temp * 10;
			if(temp == 0)
			{
				temp = 10;
			}
			temp--;
			systems.temp_hour = systems.temp_hour + temp*10;	
            setTime(64,18,systems.temp_hour,systems.temp_min,1);
			break;
			
		case 8:	
			temp = (unsigned char)(systems.temp_hour%10);
			systems.temp_hour -= temp;
			if(temp == 0)
			{
				temp = 10;
			}
			temp--;
			systems.temp_hour = systems.temp_hour+temp;	
			setTime(64,18,systems.temp_hour,systems.temp_min,2);
			break;
			
		case 9:
			temp = (unsigned char)(systems.temp_min/10);	
			systems.temp_min -= temp * 10;
			if(temp == 0)
			{
				temp = 6;
			}
			temp--;
			systems.temp_min = systems.temp_min +temp*10;	
			setTime(64,18,systems.temp_hour,systems.temp_min,3);
			break;
		
		case 10:
			temp = (unsigned char)(systems.temp_min%10);	
			systems.temp_min -= temp;			
			if(temp == 0)
			{
				temp = 10;
			}
			temp--;
			systems.temp_min = systems.temp_min+temp;	
			setTime(64,18,systems.temp_hour,systems.temp_min,4);
			break;	
			
		case 11:
			temp = (unsigned char)(systems.a_hour/10);		
			systems.a_hour -= temp * 10;
			if(temp == 0)
			{
				temp = 10;
			}
			temp--;
			systems.a_hour = systems.a_hour+ temp*10;	
            setTime(64,32,systems.a_hour,systems.a_min,1);
			break;
			
		case 12:	
			temp = (unsigned char)(systems.a_hour%10);
			systems.a_hour -= temp;
			if(temp == 0)
			{
				temp = 10;
			}
			temp--;
			systems.a_hour = systems.a_hour +temp;	
			setTime(64,32,systems.a_hour,systems.a_min,2);
			break;
			
		case 13:
			temp = (unsigned char)(systems.a_min/10);	
			systems.a_min -= temp * 10;
			if(temp == 0)
			{
				temp = 6;
			}
			temp--;
			systems.a_min = systems.a_min +temp*10;	
			setTime(64,32,systems.a_hour,systems.a_min,3);
			break;
		
		case 14:
			temp = (unsigned char)(systems.a_min%10);	
			systems.a_min -= temp;			
			if(temp == 0)
			{
				temp = 10;
			}
			temp--;			
			systems.a_min = systems.a_min+temp;	
			setTime(64,32,systems.a_hour,systems.a_min,4);
			break;		
			
		case 15:
			temp = (unsigned char)(systems.a_temp/10);
			temp1 = (unsigned char)(systems.a_temp%10);
			if(temp == 0)
			{
				temp = 9;
			}
			else
			{
				temp--;
			}
			putchar(64,46,temp+'0',1);
			systems.a_temp = temp*10+temp1;
			break;	
		case 16:
			temp = (unsigned char)(systems.a_temp/10);
			temp1 = (unsigned char)(systems.a_temp%10);
			if(temp1 == 0)
			{
				temp1 = 9;
			}
			else
			{
				temp1--;
			}
			putchar(70,46,temp1+'0',1);
			systems.a_temp = temp*10+temp1;
			break;
		default: break;
	}
}

void cylParaSetMenuOnLeft(void)
{
	struct MENU *lp;
	unsigned char temp = 0, temp1 = 0;
	
	lp = win->curfous;

	switch ( lp->status1 )
	{
		case 5:
			lp->status1 = 4;
			setFlow(64,4,systems.a_flow,1);
			break;
			
#if  		MID_FLOW <100
			
		case 6:
			lp->status1 = 5;
			setFlow(64,4,systems.a_flow,3);
			break;			
#endif			
			
		case 8:
			lp->status1 = 7;
			setTime(64,18,systems.temp_hour,systems.temp_min,1);
			break;	
			
		case 9:
			lp->status1 = 8;			
			setTime(64,18,systems.temp_hour,systems.temp_min,2);
			break;
			
		case 10:
			lp->status1 = 9;
			setTime(64,18,systems.temp_hour,systems.temp_min,3);
			break;
			
		case 12:
			lp->status1 = 11;
			setTime(64,32,systems.a_hour,systems.a_min,1);
			break;	
			
		case 13:
			lp->status1 = 12;			
			setTime(64,32,systems.a_hour,systems.a_min,2);
			break;
			
		case 14:
			lp->status1 = 13;
			setTime(64,32,systems.a_hour,systems.a_min,3);
			break;
			
		case 16:
			lp->status1 = 15;
			temp = (unsigned char)(systems.a_temp/10);
			temp1 = (unsigned char)(systems.a_temp%10);
			putchar(64,46,temp+'0',1);
			putchar(70,46,temp1+'0',0);
			break;
		default:
			break;
	}
	
}

void cylParaSetMenuOnRight(void)
{
	struct MENU *lp;
	unsigned char temp = 0, temp1 = 0;
	
	lp = win->curfous;

	switch ( lp->status1 )
	{
		case 4:
			lp->status1 = 5;
			setFlow(64,4,systems.a_flow,2);
			break;
			
#if  		MID_FLOW <100
			
		case 5:
			lp->status1 = 6;
			setFlow(64,4,systems.a_flow,4);
			break;			
#endif			
			
		case 7:
			lp->status1 = 8;
			setTime(64,18,systems.temp_hour,systems.temp_min,2);
			break;	
			
		case 8:
			lp->status1 = 9;			
			setTime(64,18,systems.temp_hour,systems.temp_min,3);
			break;
			
		case 9:
			lp->status1 = 10;
			setTime(64,18,systems.temp_hour,systems.temp_min,4);
			break;
			
		case 11:
			lp->status1 = 12;
			setTime(64,32,systems.a_hour,systems.a_min,2);
			break;	
			
		case 12:
			lp->status1 = 13;			
			setTime(64,32,systems.a_hour,systems.a_min,3);
			break;
			
		case 13:
			lp->status1 = 14;
			setTime(64,32,systems.a_hour,systems.a_min,4);
			break;
			
		case 15:
			lp->status1 = 16;
			temp = (unsigned char)(systems.a_temp/10);
			temp1 = (unsigned char)(systems.a_temp%10);
			putchar(64,46,temp+'0',0);
			putchar(70,46,temp1+'0',1);
			break;
		default:
			break;
	}
	
}

void cylParaSetMenuOnOk(void)
{
	struct MENU *lp;
	lp = win->curfous;
	unsigned int temp=0,temp1 = 0;
	switch ( lp->status1 )
	{
		case 0:					// 获得“定时”焦点
			lp->status1 = 4;
			print_ch_str( 34, 4, "流量", 0 );				
			setFlow(64,4,systems.a_flow,1);			
			break;
			
		case 1:					// 获得“流量”焦点
			lp->status1 = 7;
			print_ch_str( 34, 18, "间隔", 0 );				
			setTime(64,18,systems.temp_hour,systems.temp_min,1);			
			break;
			
		case 2:					// 获得“时间”焦点
			lp->status1 = 11;
			print_ch_str( 34, 32, "时长", 0 );					
			setTime(64,32,systems.a_hour,systems.a_min,1);
			break;			
			
		case 3:	  
			lp->status1 = 15;
			print_ch_str( 34, 46, "次数", 0 );		
			temp = systems.a_temp /10;
			temp1 = systems.a_temp %10;			
			putchar(64,46,temp+'0',1);
			putchar(70,46,temp1+'0',0);
			break;
			
		case 4:
		case 5:
		case 6:
			lp->status1 = 1;
			systems.cyls.flow = systems.a_flow;
			print_ch_str( 34, 18, "间隔", 1 );
			setFlow(64,4,systems.a_flow,0);			
			break;
			
		case 7:
		case 8:
		case 9:
		case 10:
			lp->status1 = 2;
			systems.cyls.mid_time = systems.temp_hour*60 + systems.temp_min;
			print_ch_str( 34, 32, "时长", 1 );
			setTime(64,18,systems.temp_hour,systems.temp_min,0);	
			break;
		
		case 11:
		case 12:
		case 13:
		case 14:
			lp->status1 = 3;
			print_ch_str( 34, 46, "次数", 1 );	
			systems.cyls.smp_time = systems.a_hour*60 + systems.a_min;
			setTime(64,32,systems.a_hour,systems.a_min,0);
			break;
			
		case 15:
		case 16:
			systems.cyls.cyl_num = systems.a_temp;
			eeprom_WriteCylsSetPara(systems.cyls);
			if ( lp->topMenu )
			{
				win->curfous = lp->topMenu;
				win->curfous->show();
				
			}
			break;
	default: break;
	}
}

void cylParaSetMenuOnEsc(void)
{
	struct MENU *lp;
	lp = win->curfous;
	systems.cyls.flow = systems.a_flow;
	systems.cyls.mid_time = systems.temp_hour*60 + systems.temp_min;
	systems.cyls.smp_time = systems.a_hour*60 + systems.a_min;
	systems.cyls.cyl_num = systems.a_temp;
	eeprom_WriteCylsSetPara(systems.cyls);
	if ( lp->topMenu )
	{
		win->curfous = lp->topMenu->topMenu;
		win->curfous->show();
		
	}
}

void volParaSetMenuShow(void)
{
	struct MENU *lp;
	lp = win->curfous;
	clr_lcd();
    
    draw_rectmod();
    //draw_symbol( 117, 3,7, 58, gundong, 0 );  //右三角  
  

	draw_symbol(7,4,14,27,dingrong_mu,0);
	draw_symbol(7,33,14,27,canshu_mu,0);
	
	putchar(58,4,':',0);
	putchar(58,18,':',0);
	putchar(58,32,':',0);
	putchar(58,46,':',0);
	putchar(64+36,4,'L',0);
	print_str(64+24,18,"ml/min",0);	
	print_str(64+12, 32, "h  m", 0 );	
	
	lp->status1 = 0;
	print_ch_str( 34, 4, "体积", 1 );   // 打印第一行菜单标题
	print_ch_str( 34, 18, "流量", 0 );  // 打印第二行菜单的标题
	print_ch_str( 34, 32, "间隔", 0 );  // 打印第三行菜单的标题
	print_ch_str(34, 46, "次数", 0 );
	

	if(systems.constant_v.smp_volume.d)					//体积
	{
		systems.a_volume = systems.constant_v.smp_volume.d;
	}
	else
	{
		systems.a_volume = ORIGN_VOL;
	}
	
	if ( systems.constant_v.flow )						//流量
	{
		systems.a_flow = systems.constant_v.flow;
	}
	else
	{
		systems.a_flow = ORIGIN_FLOW;
	}

	systems.temp_hour = systems.constant_v.mid_time / 60;		//间隔
	systems.temp_min = systems.constant_v.mid_time % 60;

	
	if((systems.constant_v.cyl_num)&&(systems.constant_v.cyl_num < 100))//次数
	{
		systems.a_temp = systems.constant_v.cyl_num;	
	}
	else
	{
		systems.a_temp = 1;
	}
	setFlow(64,18,systems.a_flow,0);
	setTime(64,32,systems.temp_hour,systems.temp_min,0);
	setVol(64,4,systems.a_volume,0);
	if(systems.a_temp > 9)
	{
		print_int(64,46,systems.a_temp,0);
	}
	else
	{
		putchar(64,46,'0',0);
		print_int(64+6,46,systems.a_temp,0);
	}
	
}
void volParaSetMenuOnUp(void)
{
	struct MENU *lp;
	unsigned long temp = 0;
	unsigned int temp1 = 0;
	lp = win->curfous;


	switch ( lp->status1 )
	{
		case 0: 
			lp->status1=3;
			print_ch_str( 34, 4, "体积", 0);   // 打印第一行菜单标题
			print_ch_str( 34, 18, "流量", 0 );  // 打印第二行菜单的标题
			print_ch_str( 34, 32, "间隔", 0 );  // 打印第三行菜单的标题
			print_ch_str(34, 46, "次数", 1 );			
			break;
			
		case 1:
			lp->status1 = 0;
			print_ch_str( 34, 4, "体积", 1);   // 打印第一行菜单标题
			print_ch_str( 34, 18, "流量", 0 );  // 打印第二行菜单的标题
			print_ch_str( 34, 32, "间隔", 0 );  // 打印第三行菜单的标题
			print_ch_str(34, 46, "次数", 0 );	
			break;
			
		case 2:
			lp->status1 = 1;
			print_ch_str( 34, 4, "体积", 0);   // 打印第一行菜单标题
			print_ch_str( 34, 18, "流量", 1 );  // 打印第二行菜单的标题
			print_ch_str( 34, 32, "间隔", 0 );  // 打印第三行菜单的标题
			print_ch_str(34, 46, "次数", 0 );
			break;
			
        case 3:
            lp->status1 = 2;
			print_ch_str( 34, 4, "体积", 0);   // 打印第一行菜单标题
			print_ch_str( 34, 18, "流量", 0 );  // 打印第二行菜单的标题
			print_ch_str( 34, 32, "间隔", 1 );  // 打印第三行菜单的标题
			print_ch_str(34, 46, "次数", 0 );
            break;
			
		case 4:
			temp = (unsigned char)(systems.a_volume/1000);		
			systems.a_volume = systems.a_volume - temp*1000;
			if(temp >= 9)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.a_volume = systems.a_volume + temp*1000;		
            setVol(64,4,systems.a_volume,1);
			break;
			
		case 5:
			temp1 = (unsigned char)(systems.a_volume/100);	
			temp = (unsigned char)(temp1%10);	
			systems.a_volume = systems.a_volume - temp*100;
			if(temp >= 9)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.a_volume = systems.a_volume + temp*100;		
            setVol(64,4,systems.a_volume,2);
			break;
			
		case 6:
			temp1 = (unsigned int)(systems.a_volume/10);	
			temp = (unsigned char)(temp1%10);	
			systems.a_volume = systems.a_volume - temp*10;
			if(temp >= 9)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.a_volume = systems.a_volume + temp*10;		
            setVol(64,4,systems.a_volume,3);
			break;
			
		case 7:
			temp = (unsigned int)systems.a_volume%10;	
			systems.a_volume = systems.a_volume - temp;
			if(temp >= 9)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.a_volume = systems.a_volume + temp;		
            setVol(64,4,systems.a_volume,4);
			break;
			
		case 8:
			temp = (unsigned long)(systems.a_volume*10);	
			temp = temp % 10;
			systems.a_volume = systems.a_volume - (float)temp / 10 ;
			if(temp >= 9)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.a_volume = systems.a_volume + (float)temp / 10;		
            setVol(64,4,systems.a_volume,5);
			break;
			
	case 9:		
			temp = (unsigned int)(systems.a_flow / 1000);
			systems.a_flow -= temp * 1000;
			temp++;
			if(temp > MAX_FLOW/1000)
			{
				temp = 0;
			}			
			systems.a_flow  += temp * 1000;
			
			if( systems.a_flow > MAX_FLOW )
			{
				systems.a_flow = MAX_FLOW;				
			}
			
			if ( systems.a_flow < MIN_FLOW )
			{      
				systems.a_flow = MIN_FLOW;
			}
			setFlow(64,18,systems.a_flow,1);
			break;
			
		case 10:
			temp1 = (unsigned int)(systems.a_flow / 1000); 
			temp = (unsigned int)((systems.a_flow % 1000) / 100);
			systems.a_flow  -= temp * 100;
			
			if(temp1==MAX_FLOW/1000)					
			{
#if       		MID_FLOW <100				
				temp++;
				
#else        	
				temp+=(MID_FLOW/100);
				
#endif       				
          		if(temp>(MAX_FLOW%1000)/100)
          		{
           	 		temp=0;
              
          		}
			}
			else
			{
#if        		MID_FLOW <100  				
				temp++;
				
#else           
				temp+=(MID_FLOW/100);
					
#endif  
				if(10 == temp)
				{
					temp = 0;
				}	
			}
			
			systems.a_flow  += temp * 100;			
			
			if(systems.a_flow  < MIN_FLOW)
			{
				systems.a_flow  = MIN_FLOW;	
			}
        
			if(systems.a_flow  > MAX_FLOW)
			{            
				systems.a_flow  = MAX_FLOW;
			}	
			setFlow(64,18,systems.a_flow,2);
			break;	
	
		case 11:
#if  		MID_FLOW <100
		
			temp  = (unsigned int)((systems.a_flow % 100) / 10);
			systems.a_flow -= temp * 10;
			
			if(0 == temp)
			{
				temp = 5;
			}
			else
			{
				temp = 0;
			}
			
			systems.a_flow += temp * 10;
			
			if( systems.a_flow > MAX_FLOW )
			{
				systems.a_flow = MAX_FLOW;				
			}
			
			if ( systems.a_flow < MIN_FLOW )
			{      
				systems.a_flow = MIN_FLOW;
			}
			setFlow(64,18,systems.a_flow,3);        
#endif          
			break;
			
	case 12:
			temp = (unsigned char)(systems.temp_hour/10);		
			systems.temp_hour -= temp * 10;
			if(temp >= 9)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.temp_hour = systems.temp_hour + temp*10;		
            setTime(64,32,systems.temp_hour,systems.temp_min,1);
			break;
			
		case 13:	
			temp = (unsigned char)(systems.temp_hour%10);
			systems.temp_hour -= temp;
			if(temp >= 9)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.temp_hour = temp+systems.temp_hour;	
			setTime(64,32,systems.temp_hour,systems.temp_min,2);
			break;
			
		case 14:
			temp = (unsigned char)(systems.temp_min/10);	
			systems.temp_min -= temp * 10;
			if(temp >= 5)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.temp_min = temp*10+systems.temp_min;	
			setTime(64,32,systems.temp_hour,systems.temp_min,3);
			break;
		
		case 15:
			temp = (unsigned char)(systems.temp_min%10);	
			systems.temp_min -= temp;			
			if(temp >= 9)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.temp_min = systems.temp_min+temp;	
			setTime(64,32,systems.temp_hour,systems.temp_min,4);
			break;	
			
		case 16:
			temp = (unsigned char)(systems.a_temp/10);
			temp1 = (unsigned char)(systems.a_temp%10);
			if(temp >= 9)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			putchar(64,46,temp+'0',1);
			systems.a_temp = temp*10+temp1;
			break;	
			
		case 17:
			temp = (unsigned char)(systems.a_temp/10);
			temp1 = (unsigned char)(systems.a_temp%10);
			if(temp1 >= 9)
			{
				temp1 = 0;
			}
			else
			{
				temp1++;
			}
			putchar(70,46,temp1+'0',1);
			systems.a_temp = temp*10+temp1;
			break;	
			
		default:
			break;
	}
}
void volParaSetMenuOnDown(void)
{
	struct MENU *lp;
	unsigned long temp = 0;
	unsigned int temp1 = 0;
	
	lp = win->curfous;

	switch ( lp->status1 )
	{
		case 0: 
			lp->status1=1;
			print_ch_str( 34, 4, "体积", 0);   // 打印第一行菜单标题
			print_ch_str( 34, 18, "流量", 1 );  // 打印第二行菜单的标题
			print_ch_str( 34, 32, "间隔", 0 );  // 打印第三行菜单的标题
			print_ch_str(34, 46, "次数", 0 );			
			break;
			
		case 1:
			lp->status1 = 2;
			print_ch_str( 34, 4, "体积",0);   // 打印第一行菜单标题
			print_ch_str( 34, 18, "流量", 0 );  // 打印第二行菜单的标题
			print_ch_str( 34, 32, "间隔", 1 );  // 打印第三行菜单的标题
			print_ch_str(34, 46, "次数", 0 );	
			break;
			
		case 2:
			lp->status1 = 3;
			print_ch_str( 34, 4, "体积", 0);   // 打印第一行菜单标题
			print_ch_str( 34, 18, "流量", 0);  // 打印第二行菜单的标题
			print_ch_str( 34, 32, "间隔", 0 );  // 打印第三行菜单的标题
			print_ch_str(34, 46, "次数", 1 );
			break;
			
        case 3:
            lp->status1 = 0;
			print_ch_str( 34, 4, "体积", 1);   // 打印第一行菜单标题
			print_ch_str( 34, 18, "流量", 0 );  // 打印第二行菜单的标题
			print_ch_str( 34, 32, "间隔", 0 );  // 打印第三行菜单的标题
			print_ch_str(34, 46, "次数", 0 );
            break;
			
		case 4:
			temp = (unsigned char)(systems.a_volume/1000);		
			systems.a_volume = systems.a_volume - temp*1000;
			if(temp <= 0)
			{
				temp = 9;
			}
			else
			{
				temp--;
			}
			systems.a_volume = systems.a_volume + temp*1000;		
            setVol(64,4,systems.a_volume,1);
			break;
			
		case 5:
			temp1 = (unsigned char)(systems.a_volume/100);	
			temp = (unsigned char)(temp1%10);	
			systems.a_volume = systems.a_volume - temp*100;
			if(temp <= 0)
			{
				temp = 9;
			}
			else
			{
				temp--;
			}
			systems.a_volume = systems.a_volume + temp*100;		
            setVol(64,4,systems.a_volume,2);
			break;
			
		case 6:
			temp1 = (unsigned int)(systems.a_volume/10);	
			temp = (unsigned char)(temp1%10);	
			systems.a_volume = systems.a_volume - temp*10;
			if(temp <= 0)
			{
				temp = 9;
			}
			else
			{
				temp--;
			}
			systems.a_volume = systems.a_volume + temp*10;		
            setVol(64,4,systems.a_volume,3);
			break;
			
		case 7:
			temp = (unsigned int)systems.a_volume%10;	
			systems.a_volume = systems.a_volume - temp;
			if(temp <= 0)
			{
				temp = 9;
			}
			else
			{
				temp--;
			}
			systems.a_volume = systems.a_volume + temp;		
            setVol(64,4,systems.a_volume,4);
			break;
			
		case 8:
			temp = (unsigned long)(systems.a_volume*10);	
			temp = temp % 10;
			systems.a_volume = systems.a_volume - (float)temp / 10 ;
			if(temp <= 0)
			{
				temp = 9;
			}
			else
			{
				temp--;
			}
			systems.a_volume = systems.a_volume + (float)temp / 10;		
            setVol(64,4,systems.a_volume,5);
			break;
			
		case 9:
           temp = (unsigned int)(systems.a_flow / 1000);
		   systems.a_flow -= temp * 1000;
		   if ( temp > 0 )
		   {
			   temp--;
		   }
		   else
		   {
			   temp=MAX_FLOW/1000;
			   
		   }
		   systems.a_flow += temp * 1000;
		   
		   if ( systems.a_flow < MIN_FLOW )
		   {
			   systems.a_flow = MIN_FLOW;			   
		   }
		   
		   if ( systems.a_flow > MAX_FLOW )
		   {
			   systems.a_flow=MAX_FLOW;            
		   }
           setFlow(64,18,systems.a_flow,1); 
		   break;
		   
		case 10:
	        temp1 = (unsigned int)(systems.a_flow / 1000); 
			temp  = (unsigned int)(( systems.a_flow % 1000 ) / 100 );
			systems.a_flow -= temp * 100;
			if(temp1==MAX_FLOW/1000)
			{				
				if(temp>MID_FLOW/100)
				{ 					
#if        			MID_FLOW <100    					
					temp--;
#else   			
					
					temp-=(MID_FLOW/100);
#endif        
					
				}
				else
				{
					temp=(MAX_FLOW%1000)/100;
				}
			}
			else
			{
				
				if(temp>MID_FLOW/100)
				{  					
#if        			MID_FLOW <100  					
					temp--;
#else             					
					temp-=(MID_FLOW/100);
#endif          
					
				}
				else
				{
					temp=(MAX_FLOW%1000)/100;
				}				
			} 
			systems.a_flow += temp * 100;
			
			if ( systems.a_flow < MIN_FLOW )
			{
				systems.a_flow = MIN_FLOW;
			}
			
			if ( systems.a_flow > MAX_FLOW )
			{            
				systems.a_flow = MAX_FLOW;           
			} 
			setFlow(64,18, systems.a_flow,2);
			break;
		
		case 11:
#if  		MID_FLOW <100    	
		
			temp  = (unsigned int)(( systems.a_flow % 100 ) / 10 );
			systems.a_flow -= temp * 10;
			
			if ( 0 == temp )
			{
				temp = 6;
			}
			temp--;

			systems.a_flow += temp * 10;
			
			if ( systems.a_flow > MAX_FLOW )
			{
				systems.a_flow = MAX_FLOW; 				
			}
        
			if ( systems.a_flow < MIN_FLOW )
			{     
				systems.a_flow = MIN_FLOW;            
			}
			setFlow(64,18, systems.a_flow,3);
        
#endif  
       		break; 
			
	case 12:
			temp = (unsigned char)(systems.temp_hour/10);		
			systems.temp_hour -= temp * 10;
			if(temp == 0)
			{
				temp = 10;
			}
			temp--;
			systems.temp_hour = systems.temp_hour + temp*10;	
            setTime(64,32,systems.temp_hour,systems.temp_min,1);
			break;
			
		case 13:	
			temp = (unsigned char)(systems.temp_hour%10);
			systems.temp_hour -= temp;
			if(temp == 0)
			{
				temp = 10;
			}
			temp--;
			systems.temp_hour = systems.temp_hour+temp;	
			setTime(64,32,systems.temp_hour,systems.temp_min,2);
			break;
			
		case 14:
			temp = (unsigned char)(systems.temp_min/10);	
			systems.temp_min -= temp * 10;
			if(temp == 0)
			{
				temp = 6;
			}
			temp--;
			systems.temp_min = systems.temp_min + temp*10;	
			setTime(64,32,systems.temp_hour,systems.temp_min,3);
			break;
		
		case 15:
			temp = (unsigned char)(systems.temp_min%10);	
			systems.temp_min -= temp;			
			if(temp == 0)
			{
				temp = 10;
			}
			temp--;
			systems.temp_min = systems.temp_min+temp;	
			setTime(64,32,systems.temp_hour,systems.temp_min,4);
			break;	
			
		case 16:
			temp = (unsigned char)(systems.a_temp/10);
			temp1 = (unsigned char)(systems.a_temp%10);
			if(temp == 0)
			{
				temp = 9;
			}
			else
			{
				temp--;
			}
			putchar(64,46,temp+'0',1);
			systems.a_temp = temp*10+temp1;
			break;	
			
		case 17:
			temp = (unsigned char)(systems.a_temp/10);
			temp1 = (unsigned char)(systems.a_temp%10);
			if(temp1 == 0)
			{
				temp1 = 9;
			}
			else
			{
				temp1--;
			}
			putchar(70,46,temp1+'0',1);
			systems.a_temp = temp*10+temp1;
			break;
		default:
			break;
	}
}
void volParaSetMenuOnLeft(void)
{
 	struct MENU *lp;
	unsigned char temp = 0, temp1 = 0;
	
	lp = win->curfous;

	switch ( lp->status1 )
	{
		case 5:
			lp->status1 = 4;
			setVol(64,4,systems.a_volume,1);
			break;
			
		case 6:
			lp->status1 = 5;
			setVol(64,4,systems.a_volume,2);
			break;
			
		case 7:
			lp->status1 = 6;
			setVol(64,4,systems.a_volume,3);
			break;
			
		case 8:
			lp->status1 = 7;
			setVol(64,4,systems.a_volume,4);
			break;
			
		case 10:
			lp->status1 = 9;
			setFlow(64,18,systems.a_flow,1);
			break;

#if  		MID_FLOW <100
			
		case 11:
			lp->status1 = 10;
			setFlow(64,4,systems.a_flow,2);
			break;			
#endif			
			
		case 13:
			lp->status1 = 12;
			setTime(64,32,systems.temp_hour,systems.temp_min,1);
			break;	
			
		case 14:
			lp->status1 = 13;			
			setTime(64,32,systems.temp_hour,systems.temp_min,2);
			break;
			
		case 15:
			lp->status1 = 14;
			setTime(64,32,systems.temp_hour,systems.temp_min,3);
			break;			
			
		case 17:
			lp->status1 = 16;
			temp = (unsigned char)(systems.a_temp/10);
			temp1 = (unsigned char)(systems.a_temp%10);
			putchar(64,46,temp+'0',1);
			putchar(70,46,temp1+'0',0);
			break;
		default:
			break;
	}
	
}
void volParaSetMenuOnRight(void)
{
	struct MENU *lp;
	unsigned char temp = 0, temp1 = 0;
	
	lp = win->curfous;

	switch ( lp->status1 )
	{
		case 4:
			lp->status1 = 5;
			setVol(64,4,systems.a_volume,2);
			break;
			
		case 5:
			lp->status1 = 6;
			setVol(64,4,systems.a_volume,3);
			break;
			
		case 6:
			lp->status1 = 7;
			setVol(64,4,systems.a_volume,4);
			break;
			
		case 7:
			lp->status1 = 8;
			setVol(64,4,systems.a_volume,5);
			break;
			
		case 9:
			lp->status1 = 10;
			setFlow(64,18,systems.a_flow,2);
			break;

#if  		MID_FLOW <100
			
		case 11:
			lp->status1 = 10;
			setFlow(64,18,systems.a_flow,3);
			break;			
#endif			
			
		case 12:
			lp->status1 = 13;
			setTime(64,32,systems.temp_hour,systems.temp_min,2);
			break;	
			
		case 13:
			lp->status1 = 14;			
			setTime(64,32,systems.temp_hour,systems.temp_min,3);
			break;
			
		case 14:
			lp->status1 = 15;
			setTime(64,32,systems.temp_hour,systems.temp_min,4);
			break;			
			
		case 16:
			lp->status1 = 17;
			temp = (unsigned char)(systems.a_temp/10);
			temp1 = (unsigned char)(systems.a_temp%10);
			putchar(64,46,temp+'0',0);
			putchar(70,46,temp1+'0',1);
			break;
		default:
			break;
	}
}
void volParaSetMenuOnOk(void)
{
struct MENU *lp;
	lp = win->curfous;
	unsigned int temp=0,temp1 = 0;
	switch ( lp->status1 )
	{
		case 0:					// 获得“定时”焦点
			lp->status1 = 4;
			print_ch_str( 34, 4, "体积", 0 );				
			setVol(64,4,systems.a_volume,1);		
			break;
			
		case 1:					// 获得“流量”焦点
			lp->status1 = 9;
			print_ch_str( 34, 18, "流量", 0 );	
			setFlow(64,18,systems.a_flow,1);			
			break;
			
		case 2:					// 获得“时间”焦点
			lp->status1 = 12;
			print_ch_str( 34, 32, "间隔", 0 );					
			setTime(64,32,systems.temp_hour,systems.temp_min,1);
			break;			
			
		case 3:	  
			lp->status1 = 16;
			print_ch_str( 34, 46, "次数", 0 );		
			temp = systems.a_temp /10;
			temp1 = systems.a_temp %10;			
			putchar(64,46,temp+'0',1);
			putchar(70,46,temp1+'0',0);
			break;
			
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
			lp->status1 = 1;
			print_ch_str( 34, 18, "流量", 1 );	
			systems.constant_v.smp_volume.d = systems.a_volume;
			setVol(64,4,systems.a_volume,0);	
			break;
			
		case 9:
		case 10:
		case 11:
			lp->status1 = 2;
			systems.constant_v.flow = systems.a_flow;
			print_ch_str( 34, 32, "间隔", 1 );	
			setFlow(64,18,systems.a_flow,0);		
			break;
			
		case 12:
		case 13:
		case 14:
		case 15:
			lp->status1 = 3;
			print_ch_str( 34, 46, "次数", 1 );	
			systems.constant_v.mid_time = systems.temp_hour*60 + systems.temp_min;
			setTime(64,32,systems.temp_hour,systems.temp_min,0);	
			break;
			
		case 16:
		case 17:
			systems.constant_v.cyl_num = systems.a_temp;
			eeprom_WriteCapSetPara(systems.constant_v);
			if ( lp->topMenu )
			{
				win->curfous = lp->topMenu;
				win->curfous->show();
				
			}
			break;
	}
}

void volParaSetMenuOnEsc(void)
{
	struct MENU *lp;
	lp = win->curfous;
	
	systems.constant_v.smp_volume.d = systems.a_volume;
	systems.constant_v.flow = systems.a_flow;
	systems.constant_v.mid_time = systems.temp_hour*60 + systems.temp_min;
	systems.constant_v.cyl_num = systems.a_temp;
	eeprom_WriteCapSetPara(systems.constant_v);
	if(lp->topMenu)
	{
		win->curfous = lp->topMenu->topMenu;
		win->curfous->show();
	}
}
/*
void cylFlowSetMenuOnUp(void)
{
	struct MENU *lp;
	lp = win->curfous;
	flowSetUp( lp );
}

void cylFlowSetMenuOnDown(void)
{
	struct MENU *lp;
	lp = win->curfous;
	flowSetDown( lp );
}

void cylFlowSetMenuOnLeft(void)
{
	struct MENU *lp;
	lp = win->curfous;
	flowSetLeft( lp );
}

void cylFlowSetMenuOnRight(void)
{
	struct MENU *lp;
	lp = win->curfous;
	flowSetRight( lp );
}

void cylFlowSetMenuOnOk(void)
{
	struct MENU *lp;
	lp = win->curfous;
	
     if ( 4 == lp->status1 )
	{
		if ( lp->topMenu )
		{
			win->curfous = lp->topMenu;
			win->curfous->show();
		}
	}
    else  
	{
		if ( systems.a_flow < MIN_FLOW || systems.a_flow > MAX_FLOW )
		{
			if ( lp->subMenu )
			{
				win->curfous = lp->subMenu;		// 进入错误警告菜单
				win->curfous->show();
			}
		}
		else
		{
			systems.cyls.flow = systems.a_flow;		// 确认设置流量
			systems.a_flow = ORIGIN_FLOW;			// 还原默认值
            changeFlow( systems.cyls.flow );
                        
			if ( lp->topMenu )
			{
				win->curfous = lp->topMenu;
				win->curfous->show();
			}
		}
	}
    
}

void cylFlowSetMenuOnEsc(void)
{
	struct MENU *lp;
	lp = win->curfous;

	if ( lp->topMenu )
	{
		win->curfous = lp->topMenu;
		win->curfous->show();
	}
}

void cylFlowSetMenuExec(void)
{

}


*/

/*
 * 流量错误菜单
 */
void errFlowMenuShow(void)
{

	clr_lcd();
	draw_rect( 0, 0, 127, 63 );
	draw_line( 20, 15, 107, 15 );

	
		print_ch_str( 53, 3, "警告", 0 );
		print_str( 83, 3, "!", 0 );
		if ( systems.a_flow < MIN_FLOW )
		{
			print_ch_str( 10, 17, "流量值不能小于", 0 );
			print_int( 97, 17, MIN_FLOW, 0 );
			putchar( 109, 17, '!', 0 );
		}
		else if ( systems.a_flow > MAX_FLOW )
		{
			print_ch_str( 10, 17, "流量值不能大于", 0 );
			print_int( 96, 17, MAX_FLOW, 0 );
			putchar( 120, 17, '!', 0 );
		}

		print_ch_str( 18, 30, "请重新设置流量", 0 );
		print_ch_str( 2, 48, "任意键返回", 0 );
		draw_button( 92, 44, "确定", 1, CHINESE );

}

void errFlowMenuOnOk(void)
{
	struct MENU *lp;
	lp = win->curfous;
	if ( lp->topMenu )
	{
		win->curfous = lp->topMenu;
		win->curfous->show();
	}
}





/*
 * 循环模式间隔时间设置菜单
 */
void cylDelaySetMenuShow(void)
{
	struct MENU *lp;
	unsigned int bai, shi, ge;
	systems.a_min = systems.cyls.mid_time;
	lp = win->curfous;

	clr_lcd();
   
     draw_symbol(0,0,28,64,pertime_mu,0);   

    
    draw_rectmod();
    draw_rect( 57-3, 22-10-2, 51, 18 );
    draw_line( 94-3, 23-10-2, 94-3, 29-2 );
    draw_symbol( 96-3, 23-10-2, 10, 16, up_down_mu, 0 );
    draw_symbol( 43-3, 23-10-2, 12, 16, left_mu, 0 );	// 画左三角
    draw_symbol( 110-3, 23-10-2, 12, 16, right_mu, 0 );	// 画左三角
    print_str( 68+1, 30, "Min", 0 );

    bai = (unsigned int)(systems.a_min / 100);
    shi = (unsigned int)(systems.a_min % 100 /10 );
    ge  = (unsigned int)(systems.a_min % 10);

  
    	
    	switch ( lp->status1 )
    	{
    	case 0:
			
			print_num( 60-3, 23-10-2, bai, 1 );
			print_num( 68, 23-10-2, shi, 0 );
			print_num( 76+3, 23-10-2, ge, 0 );
			draw_button( 44-2, 43, "确定", 0, CHINESE );
			draw_button( 87-2, 43, "取消", 0, CHINESE );
    		break;
    	case 1:
			print_num( 60-3, 23-10-2, bai, 0 );
			
			print_num( 68, 23-10-2, shi, 1 );
			print_num( 76+3, 23-10-2, ge, 0 );
			draw_button( 44-2, 43, "确定", 0, CHINESE );
			draw_button( 87-2, 43, "取消", 0, CHINESE );
    		break;
    	case 2:
			print_num( 60-3, 23-10-2, bai, 0 );
			print_num( 68, 23-10-2, shi, 0 );
			
			print_num( 76+3, 23-10-2, ge, 1 );
			draw_button( 44-2, 43, "确定", 0, CHINESE );
			draw_button( 87-2, 43, "取消", 0, CHINESE );
    		break;
    	case 3:
			print_num( 60-3, 23-10-2, bai, 0 );
			print_num( 68, 23-10-2, shi, 0 );
			print_num( 76+3, 23-10-2, ge, 0 );
			draw_button( 44-2, 43, "确定", 1, CHINESE );
			draw_button( 87-2, 43, "取消", 0, CHINESE );
    		break;
    	case 4:
			print_num( 60-3, 23-10-2, bai, 0 );
			print_num( 68, 23-10-2, shi, 0 );
			print_num( 76+3, 23-10-2, ge, 0 );
			draw_button( 44-2, 43, "确定", 0, CHINESE );
			draw_button( 87-2, 43, "取消", 1, CHINESE );
    		break;
    	default: break;
    	}

}

void cylDelaySetMenuOnUp(void)
{
	struct MENU *lp;
	unsigned int bai, shi, ge;
	lp = win->curfous;
	switch ( lp->status1 )
	{
	case 0:
		bai = (unsigned int)(systems.a_min / 100);
        systems.a_min -= bai * 100;     // 间隔时间减去百位上的值
        if( bai == 9 )
        {
          bai = 0;
        }
        else
        {
          bai++;
        }
        systems.a_min += bai * 100;    // 百位回位
        print_num( 60-3, 23-10-2, bai, 1 );	// 重新打印百位上的值，也即是刷新
		break;
	case 1:
		shi = (unsigned int)(systems.a_min % 100 /10 );
        systems.a_min -= shi * 10;     // 间隔时间减去十位上的值
        if( shi == 9 )
        {
          shi = 0;
        }
        else
        {
          shi++;
        }
        systems.a_min += shi * 10;    // 十位回位
        print_num( 68, 23-10-2, shi, 1 );	// 重新打印十位上的值，也即是刷新
		break;
	case 2:
	    ge = (unsigned int)(systems.a_min % 10);
        systems.a_min -= ge;     // 间隔时间减去个位上的值
        if( ge == 9 )
        {
          ge = 0;
        }
        else
        {
          ge++;
        }
        systems.a_min += ge;    // 个位回位
        print_num( 76+3, 23-10-2, ge, 1 );	// 重新打印个位上的值，也即是刷新
		break;
	default: break;
	}
}

void cylDelaySetMenuOnDown(void)
{
	struct MENU *lp;
	unsigned int bai, shi, ge;
	lp = win->curfous;
	switch ( lp->status1 )
	{
	case 0:
		bai = (unsigned int)(systems.a_min / 100);
        systems.a_min -= bai * 100;     // 间隔时间减去百位上的值
        if( bai == 0 )
        {
          bai = 9;
        }
        else
        {
          bai--;
        }
        systems.a_min += bai * 100;    // 百位回位
        print_num( 60-3, 23-10-2, bai, 1 );	// 重新打印百位上的值，也即是刷新
		break;
	case 1:
		shi = (unsigned int)(systems.a_min % 100 /10 );
        systems.a_min -= shi * 10;     // 间隔时间减去十位上的值
        if( shi == 0 )
        {
          shi = 9;
        }
        else
        {
          shi--;
        }
        systems.a_min += shi * 10;    // 十位回位
        print_num( 68, 23-10-2, shi, 1 );	// 重新打印十位上的值，也即是刷新
		break;
	case 2:
	    ge = (unsigned int)(systems.a_min % 10);
        systems.a_min -= ge;     // 间隔时间减去个位上的值
        if( ge == 0 )
        {
          ge = 9;
        }
        else
        {
          ge--;
        }
        systems.a_min += ge;    // 个位回位
        print_num( 76+3, 23-10-2, ge, 1 );	// 重新打印个位上的值，也即是刷新
		break;
	default: break;
	}
}

void cylDelaySetMenuOnLeft(void)
{
	struct MENU *lp;
	unsigned int bai, shi, ge;
	lp = win->curfous;
	switch ( lp->status1 )
	{
	 case 0:
		lp->status1 = 4;
		bai = (unsigned int)(systems.a_min / 100);
	
		print_num( 60-3, 23-10-2, bai, 0 );
		
			draw_button( 87-2, 43, "取消", 1, CHINESE );

		break;
	 case 1:
		lp->status1 = 0;
		bai = (unsigned int)(systems.a_min / 100);
		shi = (unsigned int)(systems.a_min % 100 /10 );
	
		print_num( 60-3, 23-10-2, bai, 1 );
		print_num( 68, 23-10-2, shi, 0 );
		break;
	 case 2:
		lp->status1 = 1;
		shi = (unsigned int)(systems.a_min % 100 /10 );
		ge = (unsigned int)(systems.a_min % 10);
	
		print_num( 68, 23-10-2, shi, 1 );
		print_num( 76+3, 23-10-2, ge, 0 );
		break;
	 case 3:
		lp->status1 = 2;
		ge = (unsigned int)(systems.a_min % 10);
		
		print_num( 76+3, 23-10-2, ge, 1 );
		
			draw_button( 44-2, 43, "确定", 0, CHINESE );

		break;
	 case 4:
		lp->status1 = 3;
		
			draw_button( 44-2, 43, "确定", 1, CHINESE );
			draw_button( 87-2, 43, "取消", 0, CHINESE );
	
		break;
	default: break;
	}
}











void cylDelaySetMenuOnRight(void)
{
	struct MENU *lp;
	unsigned int bai, shi, ge;
	lp = win->curfous;
	switch ( lp->status1 )
	{
	case 0:
		lp->status1 = 1;
		bai = (unsigned int)(systems.a_min / 100);
		shi = (unsigned int)(systems.a_min % 100 /10 );
		
		print_num( 60-3, 23-10-2, bai, 0 );
		print_num( 68, 23-10-2, shi, 1 );
		break;
	case 1:
		lp->status1 = 2;
		shi = (unsigned int)(systems.a_min % 100 /10 );
		ge = (unsigned int)(systems.a_min % 10);
		
		print_num( 68, 23-10-2, shi, 0 );
		print_num( 76+3, 23-10-2, ge, 1 );
		break;
	case 2:
		lp->status1 = 3;
		ge = (unsigned int)(systems.a_min % 10);
		
		print_num( 76+3, 23-10-2, ge, 0 );
		
			draw_button( 44-2, 43, "确定", 1, CHINESE );

		break;
	case 3:
		lp->status1 = 4;
		
			draw_button( 44-2, 43, "确定", 0, CHINESE );
			draw_button( 87-2, 43, "取消", 1, CHINESE );

		break;
	case 4:
		lp->status1 = 0;
		bai = (unsigned int)(systems.a_min / 100);
		//*?///fill_rect( 59, 23-10-2, 8, 12 );
		print_num( 60-3, 23-10-2, bai, 1 );
		
			draw_button( 87-2, 43, "取消", 0, CHINESE );
	
		break;
	default: break;
	}
}

void cylDelaySetMenuOnOk(void)
{
	struct MENU *lp;
	lp = win->curfous;
	
	 if ( 4 == lp->status1 )
	{
		if ( lp->topMenu )
		{
			win->curfous = lp->topMenu;
			win->curfous->show();
		}
	}else
    {
        systems.cyls.mid_time = systems.a_min;	// 确认设置间隔时间
		if ( lp->topMenu )
		{
			systems.a_min = 1;
			win->curfous = lp->topMenu;
			win->curfous->show();
		}
    
    
    }    
        
        
        
}

void cylDelaySetMenuOnEsc(void)
{
	struct MENU *lp;
	lp = win->curfous;
	if ( lp->topMenu )
	{
		systems.a_min = 1;
		win->curfous = lp->topMenu;
		win->curfous->show();
	}
}

void cylDelaySetMenuExec(void)
{

    
    
}





/*
 * 循环模式循环次数设置菜单
 */
void cylCyclesSetMenuShow(void)
{
	struct MENU *lp;
	unsigned int shi, ge;
	lp = win->curfous;
	systems.a_min = systems.cyls.cyl_num;

	clr_lcd();
	
    
    draw_rectmod();
    draw_line( 29, 16, 125, 16 );	
    
    
	
	shi = (unsigned int)(systems.a_min / 10);
	ge  = (unsigned int)(systems.a_min % 10);

	  draw_symbol(7,4,14,27,shuzhi_mu,0);
        draw_symbol(7,33,14,27,shezhi_mu,0);
		print_ch_str( 41, 3, "设置循环次数", 0 );
      	print_ch_str( 87, 27, "次", 0 );
        draw_line(72,23,72,23+16);
        draw_symbol( 74, 23, 10, 16, up_down_mu, 0 );
        draw_rect(51,22,35,18);
		switch ( lp->status1 )
		{
		case 0:
			
			print_num( 52, 23, shi, 1 );  // 打印十位值
			print_num( 62, 23, ge, 1 );  //打印个位值
			draw_button( 44-5, 44, "确定", 0, CHINESE );
			draw_button( 87-4, 44, "退出", 0, CHINESE );
			break;
		case 1:
            print_num(52,23,shi,0);
            print_num( 62, 23, ge, 0 );  //打印个位值
			draw_button( 44-5, 44, "确定", 1, CHINESE );
			draw_button( 87-4, 44, "退出", 0, CHINESE );
			break;
		case 2:
			print_num( 52, 23, shi, 0 );  // 打印十位值
			print_num( 62, 23, ge, 0 );  //打印个位值
			draw_button( 44-5, 44, "确定", 0, CHINESE );
			draw_button( 87-4, 44, "退出", 1, CHINESE );
			break;
		default: break;
		}

}

void cylCyclesSetMenuOnUp(void)
{
	struct MENU *lp;
	unsigned int shi, ge;
	lp = win->curfous;
	if ( 0 == lp->status1 )
	{
		if ( 10 == systems.a_min )
		{
			systems.a_min = 1;
		}
		else
		{
			systems.a_min++;
		}
		shi = (unsigned int)(systems.a_min / 10);
		ge  = (unsigned int)(systems.a_min % 10);
		print_num( 52, 23, shi, 1 );  // 打印十位值
		print_num( 62, 23, ge, 1 );  //打印个位值
	}
}

void cylCyclesSetMenuOnDown(void)
{
	struct MENU *lp;
	unsigned int shi, ge;
	lp = win->curfous;
	if ( 0 == lp->status1 )
	{
		if ( 1 == systems.a_min )
		{
			systems.a_min = 10;
		}
		else
		{
			systems.a_min--;
		}
		shi = (unsigned int)(systems.a_min / 10);
		ge  = (unsigned int)(systems.a_min % 10);
		print_num( 52, 23, shi, 1 );  // 打印十位值
		print_num( 62, 23, ge, 1 );  //打印个位值
	}
}

void cylCyclesSetMenuOnLeft(void)
{
	struct MENU *lp;
	unsigned int shi, ge;
	shi = (unsigned int)(systems.a_min / 10);
	ge  = (unsigned int)(systems.a_min % 10);
	lp = win->curfous;
	switch ( lp->status1 )
	{
	case 0:
		lp->status1 = 2;
		
        
		print_num( 52, 23, shi, 0 );  // 打印十位值
		print_num( 62, 23, ge, 0 );  //打印个位值
		
			draw_button( 87-4, 44, "退出", 1, CHINESE );
	
		break;
	case 1:
		lp->status1 = 0;
		
		print_num( 52, 23, shi, 1 );  // 打印十位值
		print_num( 62, 23, ge, 1 );   //打印个位值
		
			draw_button( 44-5, 44, "确定", 0, CHINESE );
	
		break;
	case 2:
		lp->status1 = 1;
		
			draw_button( 44-5, 44, "确定", 1, CHINESE );
			draw_button( 87-4, 44, "退出", 0, CHINESE );
	
		break;
	default: break;
	}
}

void cylCyclesSetMenuOnRight(void)
{
	struct MENU *lp;
	unsigned int shi, ge;
	shi = (unsigned int)(systems.a_min / 10);
	ge  = (unsigned int)(systems.a_min % 10);
	lp = win->curfous;
	switch ( lp->status1 )
	{
	case 0:
		lp->status1 = 1;
		//clr_rect( 53, 23, 16, 12 );
		print_num( 52, 23, shi, 0 );  // 打印十位值
		print_num( 62, 23, ge, 0 );  //打印个位值
		
			draw_button( 44-5, 44, "确定", 1, CHINESE );
	
		break;
	case 1:
		lp->status1 = 2;
		
			draw_button( 44-5, 44, "确定", 0, CHINESE );
			draw_button( 87-4, 44, "退出", 1, CHINESE );
	
		break;
	case 2:
		lp->status1 = 0;
		
		print_num( 52, 23, shi, 1 );  // 打印十位值
		print_num( 62, 23, ge, 1 );  //打印个位值
		
			draw_button( 87-4, 44, "退出", 0, CHINESE );
	
		break;
	default: break;
	}
}

void cylCyclesSetMenuOnOk(void)
{
	struct MENU *lp;
	lp = win->curfous;
	
	 if ( 2 == lp->status1 )
	{
		if ( lp->topMenu )
		{
            
			win->curfous = lp->topMenu;
			win->curfous->show();
		}
	}
     else
    {
        systems.cyls.cyl_num = systems.a_min;	// 确定设置循环次数
		if ( lp->topMenu )
		{
			systems.a_min = 1;
			win->curfous = lp->topMenu;
			win->curfous->show();
		}
    }    
        
        
}

void cylCyclesSetMenuOnEsc(void)
{
	struct MENU *lp;
	lp = win->curfous;
	systems.a_min = 1;	// 取消设置循环次数
	if ( lp->topMenu )
	{
		win->curfous = lp->topMenu;
		win->curfous->show();
	}
}

void cylCyclesSetMenuExec(void)
{

}
















//函数边界//////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////





/*
 * 显示时间
 */
void show_samp_time(unsigned int mod)
{
	static unsigned char old_hour = 0;
	static unsigned char old_min = 0;
	static unsigned char old_sec = 0;

	unsigned char temp = 0;
	
	if(mod)
	{
		old_hour = (unsigned char)(systems.used_time / 60);		
		old_sec = systems.used_sec;
		old_min = (unsigned char)(systems.used_time %60);
		
		//显示时
		if ( old_hour < 10 )
		{	// printTime.min宽度为1个字符宽度，在其前面填充0

			print_int( USED_X+30, USED_Y, 0, 0 );
			print_int( USED_X+36, USED_Y, old_hour, 0 );
		}
		else
		{
			print_int( USED_X+30, USED_Y, old_hour, 0 );
		}
		putchar(USED_X+42,USED_Y,'h',0);
		//显示分
		if ( old_min < 10 )
		{	// printTime.min宽度为1个字符宽度，在其前面填充0

			print_int( USED_X+48, USED_Y, 0, 0 );
			print_int( USED_X+54, USED_Y, old_min, 0 );
		}
		else
		{
			print_int( USED_X+48, USED_Y, old_min, 0 );
		}
		putchar(USED_X+60,USED_Y,'m',0);
		
		//显示秒
		if ( old_sec < 10 )
		{	// printTime.min宽度为1个字符宽度，在其前面填充0

			print_int( USED_X+66, USED_Y, 0, 0 );
			print_int( USED_X+72, USED_Y, old_sec, 0 );
		}
		else
		{
			print_int( USED_X+66, USED_Y, old_sec, 0 );
		}	
		putchar(USED_X+78,USED_Y,'s',0);
	}
	else
	{
		temp = (unsigned char)(systems.used_time / 60);
		if(old_hour != temp)
		{
			old_hour = temp;
			if ( temp < 10 )
			{	// printTime.min宽度为1个字符宽度，在其前面填充0
	
				print_int( USED_X+30, USED_Y, 0, 0 );
				print_int( USED_X+36, USED_Y, old_hour, 0 );
			}
			else
			{
				print_int( USED_X+30, USED_Y, old_hour, 0 );
			}
		}
		
		temp = (unsigned char)(systems.used_time %60);
		if(old_min != temp)
		{
			old_min = temp;
			if ( old_min < 10 )
			{	// printTime.min宽度为1个字符宽度，在其前面填充0
	
				print_int( USED_X+48, USED_Y, 0, 0 );
				print_int( USED_X+54, USED_Y, old_min, 0 );
			}
			else
			{
				print_int( USED_X+48, USED_Y, old_min, 0 );
			}
		}	
		
		temp = systems.used_sec;
		if(old_sec != temp)
		{
			old_sec = temp;
			if ( old_sec < 10 )
			{	// printTime.min宽度为1个字符宽度，在其前面填充0
	
				print_int( USED_X+66, USED_Y, 0, 0 );
				print_int( USED_X+72, USED_Y, old_sec, 0 );
			}
			else
			{
				print_int( USED_X+66, USED_Y, old_sec, 0 );
			}
		}	
	}
}

void show_time( unsigned int mod )
{
	static unsigned char old_hour = 0;
	static unsigned char old_min = 0;
	static unsigned char old_sec = 0;
	// 打印系统当前时间
    
	if ( systems.sys_time.sec != old_sec || mod == 1 )
	{	// 系统当前时间秒值发生改变，更新系统当前时间的秒值
		old_sec = systems.sys_time.sec;		// 更新系统当前时间的秒值
		if ( old_sec & 0x01 )
		{
			putchar( 14, 0, ':', 0 );			// 奇数秒打印冒号
		}
		else
		{
			putchar( 14, 0, ' ', 0 );			// 偶数秒打印空格
		}
	}

	// 打印显示系统当前时间的分
	if ( systems.sys_time.min != old_min || mod == 1 )
	{	// 系统当前时间秒值发生改变，更新系统当前时间的秒值
		old_min = systems.sys_time.min;		// 更新系统当前时间的分值
		clr_rect( 20, 0, 12, 12 );
		if ( old_min < 10 )
		{	// printTime.min宽度为1个字符宽度，在其前面填充0

			print_int( 20, 0, 0, 0 );
			print_int( 26, 0, old_min, 0 );
		}
		else
		{
			print_int( 20, 0, old_min, 0 );
		}
	}
	// 打印显示系统当前时间的时
	if ( systems.sys_time.hour != old_hour || mod == 1 )
	{	// 系统当前时间秒值发生改变，更新系统当前时间的秒值
		old_hour = systems.sys_time.hour;		// 更新系统当前时间的时值
		clr_rect( 2, 0, 12, 12 );
		if ( old_hour < 10 )
		{
			print_int( 2, 0, 0, 0 );
			print_int( 8, 0, old_hour, 0 );
		}
		else
		{
			print_int( 2, 0, old_hour, 0 );
		}
	}
}

/*
 * 状态显示函数
 */
void status_show( unsigned int status )             //状态参数显示函数
{
    clr_rect( STAT_X, STAT_Y, 12, 12 );
    
	switch( status )
	{
	case STOP:
		draw_symbol( STAT_X, STAT_Y, 12, 12, status_icn[0], 0 );
		break;
	case WAITING:
		draw_symbol( STAT_X, STAT_Y, 12, 12, status_icn[1], 0 );
		break;
	case SAMPLING:
		draw_symbol( STAT_X, STAT_Y, 12, 12, status_icn[2], 0 );
		break;
	case PAUSE:
		draw_symbol( STAT_X, STAT_Y, 12, 12, status_icn[3], 0 );
		break;
	default: break;
	}
}

/*
 * 流量显示函数
 */
void flow_show( unsigned int flow )
{
	//static unsigned int temp = 0;

	if( flow <= 9999 )
	{
		clr_rect( FLOW_X+30, FLOW_Y, 24, 12 );
		if( flow >= 1000 )                  // 如果大于1000，则显示四位
		{
			print_int( FLOW_X+30, FLOW_Y, flow, 0 );    // 显示位置从最左边开始
		}
		else if( flow < 1000 && flow >= 100 )         // 如果大于100，小于1000
		{
			print_int( FLOW_X+32, FLOW_Y, flow, 0 );   //则从左边数过来1位再显示
		}
		else if( flow < 100 && flow >= 10 )         // 如果大于100 小于10
		{
			print_int( FLOW_X+34, FLOW_Y, flow, 0 );   //则从左边数过来2位再显示
		}
		else
		{
			print_int( FLOW_X+40, FLOW_Y, flow, 0 );   //如果小于10，则从左边数起第三位开始显示
		}
	}
    
    if ( COMPLETE == systems.status )
    {
        clr_rect( FLOW_X+30, FLOW_Y, 24, 12 );
    }
}



/*
 * 显示已采体积
 */
void used_volume_show( double new_volume,unsigned int x, unsigned int y)
{
    double volume = new_volume / 1000;
	if(volume >= 10000.0)
	{
		print_float( x, y, volume, 1, 0 );
	}
	else if ( volume >= 1000.0 )
    {
        print_float( x+6, y, volume, 1, 0 );
    }
    else if ( volume >= 100.0 )
    {
        print_float( x+6, y, volume, 2, 0 );
    }
    else if ( volume >= 10 )
    {
        print_float( x+6, y, volume, 3, 0 );
    }
    else if ( volume > 0 )
    {
        print_float( x+6, y, volume, 4, 0 );
    }
    else if ( volume <= 0.001 )
    {
		clr_rect(x,y,36,12);
        putchar( x+36, y, '0', 0 );
    }
}

/*
 *  进度条和进度显示,percent为进度
 */
void percent_show( double percent ) // 进度条和进度显示,percent为进度
{
	unsigned int per_int;
	unsigned int gewei;
	unsigned int shiwei;
	if ( percent >= 0 )
	{
		clr_rect( 98, 25, 18, 12 );
		clr_rect( PERC_X+1, PERC_Y+2, 60, 8 );
		per_int = (unsigned int)( percent*100 );
		if ( per_int == 100 )
		{
			print_str( PERC_X+62, PERC_Y, "100", 0 );
			fill_rect( PERC_X+1, PERC_Y+2, 60, 8 );
		}
		else if ( per_int >= 10 && per_int < 100 )
		{
			gewei = per_int % 10;
			putchar( PERC_X+72, PERC_Y, gewei+'0', 0 );
			shiwei = ( per_int - gewei ) / 10;
			putchar( PERC_X+66, PERC_Y, shiwei+'0', 0 );

			fill_rect( PERC_X+1, PERC_Y+2, (unsigned char)( percent*60 ), 8 );
		}
		else
		{
			putchar( PERC_X+72, PERC_Y, per_int+'0', 0 );
			fill_rect( PERC_X+1, PERC_Y+2, (unsigned char)( percent*60 ), 8 );
		}
	}
}


void setFlow(int x, int y, unsigned int flow, char state)
{
	unsigned char qian,bai; 	
	unsigned char shi,ge;
	
    qian = (unsigned char)(flow / 1000);
    bai  = (unsigned char)(flow % 1000 / 100 );
    shi  = (unsigned char)(flow % 100 / 10 );
    ge   = (unsigned char)(flow % 10);
	
	switch(state)
	{
		case 0:
			putchar(x,y,qian+'0',0);
			putchar(x+6,y,bai+'0',0);
			putchar(x+12,y,shi+'0',0);
			putchar(x+18,y,ge+'0',0);
			break;
			
		case 1:
			putchar(x,y,qian+'0',1);
			putchar(x+6,y,bai+'0',0);
			putchar(x+12,y,shi+'0',0);
			putchar(x+18,y,ge+'0',0);
			break;
			
		case 2:
			putchar(x,y,qian+'0',0);
			putchar(x+6,y,bai+'0',1);
			putchar(x+12,y,shi+'0',0);
			putchar(x+18,y,ge+'0',0);
			break;
			
		case 3:
			putchar(x,y,qian+'0',0);
			putchar(x+6,y,bai+'0',0);
			putchar(x+12,y,shi+'0',1);
			putchar(x+18,y,ge+'0',0);
			break;
			
		case 4:
			putchar(x,y,qian+'0',0);
			putchar(x+6,y,bai+'0',0);
			putchar(x+12,y,shi+'0',0);
			putchar(x+18,y,ge+'0',1);
			break;
		default:
			break;
	}
}

void setTime(int x, int y, unsigned char hour,unsigned char min, char state)
{
	unsigned char shi_hour,ge_hour; 	
	unsigned char shi_min,ge_min;
	
	
	shi_hour = (unsigned char)(hour/10);
	ge_hour = (unsigned char)(hour%10);
	shi_min = (unsigned char)(min/10);
	ge_min = (unsigned char)(min%10);
	
	switch(state)
	{
		case 0:
			putchar(x,y,shi_hour+'0',0);
			putchar(x+6,y,ge_hour+'0',0);		
			putchar(x+18,y,shi_min+'0',0);
			putchar(x+24,y,ge_min+'0',0);	
			break;
			
		case 1:
			putchar(x,y,shi_hour+'0',1);
			putchar(x+6,y,ge_hour+'0',0);		
			putchar(x+18,y,shi_min+'0',0);
			putchar(x+24,y,ge_min+'0',0);	
			break;
			
		case 2:
			putchar(x,y,shi_hour+'0',0);
			putchar(x+6,y,ge_hour+'0',1);		
			putchar(x+18,y,shi_min+'0',0);
			putchar(x+24,y,ge_min+'0',0);	
			break;
			
		case 3:
			putchar(x,y,shi_hour+'0',0);
			putchar(x+6,y,ge_hour+'0',0);		
			putchar(x+18,y,shi_min+'0',1);
			putchar(x+24,y,ge_min+'0',0);	
			break;

		case 4:
			putchar(x,y,shi_hour+'0',0);
			putchar(x+6,y,ge_hour+'0',0);		
			putchar(x+18,y,shi_min+'0',0);
			putchar(x+24,y,ge_min+'0',1);	
			break;
			
		case 5:
			putchar(x,y,shi_hour+'0',1);
			putchar(x+6,y,ge_hour+'0',1);		
			putchar(x+18,y,shi_min+'0',0);
			putchar(x+24,y,ge_min+'0',0);	
			break;

		case 6:
			putchar(x,y,shi_hour+'0',0);
			putchar(x+6,y,ge_hour+'0',0);		
			putchar(x+18,y,shi_min+'0',1);
			putchar(x+24,y,ge_min+'0',1);	
			break;
		default:
			break;
	}
}
void setVol(int x, int y, double vol, char state)
{
	unsigned char qian,bai; 	
	unsigned char shi,ge;
	unsigned char temp;
	unsigned long num;
	
	num = (unsigned long)(vol*10);
	
    qian = (unsigned char)(num / 10000);
    bai  = (unsigned char)(num % 10000 / 1000 );
    shi  = (unsigned char)(num % 1000 / 100 );
    ge   = (unsigned char)(num % 100 / 10);	
	temp = (unsigned char)(num % 10);
	putchar(x+24,y,'.',0);
	
	
	switch(state)
	{
		case 0:
			putchar(x,y,qian+'0',0);
			putchar(x+6,y,bai+'0',0);		
			putchar(x+12,y,shi+'0',0);
			putchar(x+18,y,ge+'0',0);	
			putchar(x+30,y,temp+'0',0);	
			break;
			
		case 1:
			putchar(x,y,qian+'0',1);
			putchar(x+6,y,bai+'0',0);		
			putchar(x+12,y,shi+'0',0);
			putchar(x+18,y,ge+'0',0);	
			putchar(x+30,y,temp+'0',0);	
			break;
			
		case 2:
			putchar(x,y,qian+'0',0);
			putchar(x+6,y,bai+'0',1);		
			putchar(x+12,y,shi+'0',0);
			putchar(x+18,y,ge+'0',0);	
			putchar(x+30,y,temp+'0',0);	
			break;
			
		case 3:
			putchar(x,y,qian+'0',0);
			putchar(x+6,y,bai+'0',0);		
			putchar(x+12,y,shi+'0',1);
			putchar(x+18,y,ge+'0',0);	
			putchar(x+30,y,temp+'0',0);	
			break;
			
		case 4:
			putchar(x,y,qian+'0',0);
			putchar(x+6,y,bai+'0',0);		
			putchar(x+12,y,shi+'0',0);
			putchar(x+18,y,ge+'0',1);	
			putchar(x+30,y,temp+'0',0);	
			break;
			
		case 5:
			putchar(x,y,qian+'0',0);
			putchar(x+6,y,bai+'0',0);		
			putchar(x+12,y,shi+'0',0);
			putchar(x+18,y,ge+'0',0);	
			putchar(x+30,y,temp+'0',1);	
			break;
		default:
			break;
	}
}
void flowSetShow(struct MENU *lp)
{	
	clr_lcd();
    draw_rectmod();   
	
	draw_symbol(7,4,14,27,shoudong_mu,0);
	draw_symbol(7,33,14,27,canshu_mu,0);		 
		 
	putchar(56,4,':',0);
	print_str( 86, 4, "mL/min", 0 );	
	print_str(56, 18, ":", 0 );	
	putchar(80,32,':',0);		
	draw_symbol(100,33,8,8,temp_dram,0);	
	
		 

	print_ch_str(32,4,"流量",1);	
	setFlow(62,4,systems.a_flow,0);
	
	print_ch_str(32,18,"时长",0);
	print_str(62+12, 18, "h  m", 0 );
	setTime(62, 18, systems.a_hour,systems.a_min,0);	
	
	print_ch_str(32,32,"温度输入",0);	
	print_ch_str(32,46,"输入确认",0);
	draw_rect(96,46 ,12,12);
	
	if(systems.set_temp_flag)
	{
		draw_symbol(97,47,10,10,gou_mu,0);		
	}
	else
	{
		systems.a_temp = (int)systems.envir_temp;
	}
	
	if(systems.a_temp > 9)
	{
		print_int(86,32,(int)systems.a_temp,0);
	}
	else
	{
		putchar(86,32,'0',0);
		print_int(92,32,(int)systems.a_temp,0);
	}

	
	//draw_button(  40, 44, "确定", 0, CHINESE );  //打印确定按钮
	//draw_button(  84, 44, "取消", 0, CHINESE );  //打印取消按钮
		
}

void flowSetUp(struct MENU *lp)		//设置流量温度时长
{
 	unsigned int temp, temp1 = 0;//shi = 0, bai = 0, qian = 0;
	
    switch(lp->status1)
	{
		case 5:
			temp = (unsigned int)(systems.a_flow / 1000);
			systems.a_flow -= temp * 1000;
			temp++;
			if(temp > MAX_FLOW/1000)
			{
				temp = 0;
			}			
			systems.a_flow  += temp * 1000;
			if( systems.a_flow > MAX_FLOW )
			{
				systems.a_flow = MAX_FLOW;				
			}
			
			if ( systems.a_flow < MIN_FLOW )
			{      
				systems.a_flow = MIN_FLOW;
			}
			setFlow(62,4,systems.a_flow,1);
			break;
			
		case 6:
			temp1 = (unsigned int)(systems.a_flow / 1000); 
			temp = (unsigned int)((systems.a_flow % 1000) / 100);
			systems.a_flow  -= temp * 100;
			
			if(temp1==MAX_FLOW/1000)					
			{
#if       		MID_FLOW <100				
				temp++;
				
#else        	
				temp+=(MID_FLOW/100);
				
#endif       				
          		if(temp>(MAX_FLOW%1000)/100)
          		{
           	 		temp=0;
              
          		}
			}
			else
			{
#if        		MID_FLOW <100  				
				temp++;
				
#else           
				temp+=(MID_FLOW/100);
					
#endif  
				if(10 == temp)
				{
					temp = 0;
				}	
			}
			
			systems.a_flow  += temp * 100;			
			
			if(systems.a_flow  < MIN_FLOW)
			{
				systems.a_flow  = MIN_FLOW;	
			}
        
			if(systems.a_flow  > MAX_FLOW)
			{            
				systems.a_flow  = MAX_FLOW;
			}	
			setFlow(62,4,systems.a_flow,2);
			break;	
	
		case 7:
#if  		MID_FLOW <100
		
			temp  = (unsigned int)((systems.a_flow % 100) / 10);
			systems.a_flow -= temp * 10;
			
			if(0 == temp)
			{
				temp = 5;
			}
			else
			{
				temp = 0;
			}
			
			systems.a_flow += temp * 10;
			
			if( systems.a_flow > MAX_FLOW )
			{
				systems.a_flow = MAX_FLOW;				
			}
			
			if ( systems.a_flow < MIN_FLOW )
			{      
				systems.a_flow = MIN_FLOW;
			}
			setFlow(62,4,systems.a_flow,3);        
#endif  
        
			break;
			
		case 8:
			temp = (unsigned char)(systems.a_hour/10);		
			systems.a_hour -= temp * 10;
			if(temp >= 9)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.a_hour = systems.a_hour + temp*10;	
            setTime(62,18,systems.a_hour,systems.a_min,1);
			break;
			
		case 9:	
			temp = (unsigned char)(systems.a_hour%10);
			systems.a_hour -= temp;
			if(temp >= 9)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.a_hour = temp+systems.a_hour;	
			setTime(62,18,systems.a_hour,systems.a_min,2);
			break;
			
		case 10:
			temp = (unsigned char)(systems.a_min/10);	
			systems.a_min -= temp * 10;
			if(temp >= 5)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.a_min = temp*10+systems.a_min;	
			setTime(62,18,systems.a_hour,systems.a_min,3);
			break;
		
		case 11:
			temp = (unsigned char)(systems.a_min%10);	
			systems.a_min -= temp;			
			if(temp >= 9)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.a_min = systems.a_min+temp;	
			setTime(62,18,systems.a_hour,systems.a_min,4);
			break;
			
		case 12:
			temp = (unsigned char)(systems.a_temp/10);			
			temp1 = (unsigned char)(systems.a_temp%10);
			if(temp >= 3)
			{
				temp = 0;
			}
			else
			{
				temp++;
			}
			systems.a_temp = temp*10+temp1;	
			putchar( 86, 32, temp+'0',  1 );
			break;
			
		case 13:
			temp = (unsigned char)(systems.a_temp/10);			
			temp1 = (unsigned char)(systems.a_temp%10);
			if(temp1 >= 9)
			{
				temp1 = 0;
			}
			else
			{
				temp1++;
			}
			systems.a_temp = temp*10+temp1;	
			putchar( 92, 32, temp1+'0',  1 );
			break;
		default:
			break;
	}
}

void flowSetDown( struct MENU *lp )
{
	unsigned int temp = 0, temp1 = 0;

	switch ( lp->status1 )
	{
	 	case 5:   
           temp = (unsigned int)(systems.a_flow / 1000);
		   systems.a_flow -= temp * 1000;
		   if ( temp > 0 )
		   {
			   temp--;
		   }
		   else
		   {
			   temp=MAX_FLOW/1000;
			   
		   }
		   systems.a_flow += temp * 1000;
		   
		   if ( systems.a_flow < MIN_FLOW )
		   {
			   systems.a_flow = MIN_FLOW;			   
		   }
		   
		   if ( systems.a_flow > MAX_FLOW )
		   {
			   systems.a_flow=MAX_FLOW;            
		   }
           setFlow(62,4,systems.a_flow,1); 
			break;
		   
		case 6:
        temp1 = (unsigned int)(systems.a_flow / 1000); 
		temp  = (unsigned int)(( systems.a_flow % 1000 ) / 100 );
		systems.a_flow -= temp * 100;
        if(temp1==MAX_FLOW/1000)
        {
          
          if(temp>MID_FLOW/100)
          {      
#if        MID_FLOW <100            
           temp--;
#else   
           
               
           temp-=(MID_FLOW/100);
#endif           
          }
          else
           temp=(MAX_FLOW%1000)/100;
        }
        else
        {
           
          if(temp>MID_FLOW/100)
          {      
#if        MID_FLOW <100            
           temp--;
#else             
               
           temp-=(MID_FLOW/100);
#endif           
          }
          else
           temp=(MAX_FLOW%1000)/100;
		  
        } 
		systems.a_flow += temp * 100;
        
        if ( systems.a_flow < MIN_FLOW )
        {
            systems.a_flow = MIN_FLOW;
        }
        
        if ( systems.a_flow > MAX_FLOW )
        {            
            systems.a_flow = MAX_FLOW;           
        } 
        setFlow(62,4, systems.a_flow,2);
		break;
		
	case 7:
#if  MID_FLOW <100     
            
		temp  = (unsigned int)(( systems.a_flow % 100 ) / 10 );
		systems.a_flow -= temp * 10;
     
        if ( 0 == temp )
		{
			temp = 5;
		}
		else
		{
			temp = 0;
		}

        systems.a_flow += temp * 10;

        if ( systems.a_flow > MAX_FLOW )
        {
			systems.a_flow = MAX_FLOW;          
        }
        
        if ( systems.a_flow < MIN_FLOW )
        {     
            systems.a_flow = MIN_FLOW;            
        }
        setFlow(62,4, systems.a_flow,3);
        
#endif  
       break; 
	   
	case 8:
		temp = (unsigned char)(systems.a_hour/10);		
		systems.a_hour -= temp*10;
		if(temp == 0)
		{
			temp = 10;
		}
		temp--;
		
		systems.a_hour = temp*10+systems.a_hour;	
		setTime(62,18,systems.a_hour,systems.a_min,1);
		break;
			
	case 9:
		temp = (unsigned char)(systems.a_hour%10);
		systems.a_hour -= temp;
		if(temp == 0)
		{
			temp = 9;
		}
		else
		{
			temp--;
		}
		systems.a_hour = systems.a_hour+temp;	
		setTime(62,18,systems.a_hour,systems.a_min,2);
		break;
		
	case 10:
		temp = (unsigned char)(systems.a_min/10);
		systems.a_min -= 10*temp;
		if(temp == 0)
		{
			temp = 6;
		}
		temp--;
		systems.a_min = temp*10+systems.a_min;	
		setTime(62,18,systems.a_hour,systems.a_min,3);
		break;

	case 11:
		temp = (unsigned char)(systems.a_min%10);
		systems.a_min -= temp;
		if(temp == 0)
		{
			temp = 10;
		}
		temp--;
		
		systems.a_min =systems.a_min +temp;	
		setTime(62,18,systems.a_hour,systems.a_min,4);
		break;
		
	case 12:	
		temp = (unsigned char)(systems.a_temp/10);	
		systems.a_temp -= 10*temp;
		if(temp == 0)
		{
			temp = 4;
		}		
		temp--;		
		systems.a_temp += temp*10;	
		putchar( 86, 32, temp+'0',  1 );
		break;
			
	case 13:		
		temp = (unsigned char)(systems.a_temp%10);
		systems.a_temp -= temp;
		if(temp == 0)
		{
			temp = 10;
		}
		temp--;
		systems.a_temp += temp;	
		putchar( 92, 32, temp+'0',  1 );
		break;
	default: 
	    break;
	}
}

void flowSetLeft( struct MENU *lp )
{
	unsigned char var1, var2;
	
	switch ( lp->status1 )
	{
		case 6:
			lp->status1 = 5;
			setFlow(62,4,systems.a_flow,1);
			break;
			
#if 		MID_FLOW <100			
		case 7:
			lp->status1 = 6;
			setFlow(62,4,systems.a_flow,2);
			break;	
#endif		
		case 9:
			lp->status1 = 8;
			setTime(62,18,systems.a_hour,systems.a_min,1);
			break;
			
		case 10:
			lp->status1 = 9;
			setTime(62,18,systems.a_hour,systems.a_min,2);
			break;
			
		case 11:
			lp->status1 = 10;
			setTime(62,18,systems.a_hour,systems.a_min,3);
			break;
			
		case 13:
			lp->status1 = 12;
			var1 = (unsigned char)(systems.a_temp / 10 );
			var2 = (unsigned char)(systems.a_temp % 10 );
			putchar(86, 32, var1+'0', 1 );	
			putchar(92, 32, var2+'0', 0 );	
			break;
		default:
			break;
	}
}

void flowSetRight( struct MENU *lp )
{
	unsigned char var1, var2;

	switch ( lp->status1 )
	{
		case 5:
			lp->status1 = 6;
			setFlow(62,4,systems.a_flow,2);
			break;			
	
		case 6:
#if       	MID_FLOW <100		
		
			lp->status1 = 7;
			setFlow(62,4,systems.a_flow,3);
#endif
			break;
			
		case 8:
			lp->status1 = 9;
			setTime(62,18,systems.a_hour,systems.a_min,2);
			break;
			
		case 9:
			lp->status1 = 10;
			setTime(62,18,systems.a_hour,systems.a_min,3);
			break;
			
		case 10:
			lp->status1 = 11;
			setTime(62,18,systems.a_hour,systems.a_min,4);
			break;
			
		case 12:
			lp->status1 = 13;
			var1 = (unsigned char)(systems.a_temp / 10 );
			var2 = (unsigned char)(systems.a_temp % 10 );
			putchar(86, 32, var1+'0', 0 );	
			putchar(92, 32, var2+'0', 1 );	
			break; 
		default: 
			break;
	}
}
void flowSetOk(struct MENU *lp)
{
	unsigned char var1, var2;

		switch ( lp->status1 )
		{
			case 0:
				lp->status1 = 5;
				clr_rect(32,4,24,12);
				print_ch_str(32,4,"流量",0);	
				var1 = (unsigned char)(systems.a_flow / 1000);				
				putchar(62, 4, var1+'0', 1 );
				break;
				
			case 1:
				lp->status1 = 8;
				clr_rect(32,18,24,12);
				print_ch_str(32,18,"时长",0);
				var1 = (unsigned char)(systems.a_hour/10);
				putchar(62,18,var1+'0',1);
				break;
	
			case 2:
				lp->status1 = 12;
				clr_rect(32,32,24,12);
				print_ch_str(32,32,"温度输入",0);
				var1 = (unsigned char)( systems.a_temp / 10 );
				putchar(86,32,var1+'0',1);
				break;
				
			case 3:
				//print_ch_str(32,44,"温度设定",0);
				systems.set_temp_flag ^= 1;
				if(systems.set_temp_flag)
				{
					draw_symbol(97,47,10,10,gou_mu,0);
					systems.manu.temperature = systems.a_temp;
				}
				else
				{
					clr_rect(97,47,10,10);
					systems.manu.temperature = (int)env_temp;
				}
				break;
				
			case 4:
				systems.a_flow = systems.manu.flow;	// 取消设置流量
				systems.a_min = systems.manu.smp_time % 60;	// 取消设置流量smp_time
				systems.a_hour = systems.manu.smp_time / 60;	// 取消设置流量smp_time
				systems.set_temp_flag = 0;
				
				if ( lp->topMenu )
				{
					win->curfous = lp->topMenu;		
					win->curfous->show();
					
				}
				break;
				
			case 5:
			case 6:
			case 7:
				if ( systems.a_flow < MIN_FLOW || systems.a_flow > MAX_FLOW )
				{	// 判断流量是否在40——4000之间(合法流量)
					if ( lp->subMenu )
					{
						win->curfous = lp->subMenu;		// 进入错误警告菜单
						win->curfous->show();
					}					
				}
				else
				{
					systems.manu.flow = systems.a_flow;	// 确认设置流量
					//systems.a_flow = ORIGIN_FLOW;	// systems.a_flow 复位默认值
					changeFlow( systems.manu.flow );
					
					/*if ( lp->topMenu )
					{
						win->curfous = lp->topMenu;
						win->curfous->show();					
					}*/
					lp->status1 = 1;
					print_ch_str(32,18,"时长",1);	
					setFlow(62,4,systems.a_flow,0);
					/*var1 = (unsigned char)(systems.a_flow / 1000);
					var2 = (unsigned char)(systems.a_flow % 1000 / 100 );
					var3 = (unsigned char)(systems.a_flow % 100 / 10);
					putchar(62,4,var1+'0',0);
					putchar(62+6,4,var2+'0',0);
					putchar(62+12,4,var3+'0',0);
					*/
				}
				break;
				
			case 8:
			case 9:
			case 10:
			case 11:
				lp->status1 = 2;
				print_ch_str(32,32,"温度输入",1);
				systems.manu.smp_time = (unsigned long)systems.a_hour*3600 + (unsigned long)systems.a_min*60 + systems.a_sec;	
				setTime(62,18,systems.a_hour,systems.a_min,0);
				break;
				
			case 12:
			case 13:			
				// 判断设置温度的范围之间(合法流量)
				lp->status1 = 3; 
				print_ch_str(32,46,"输入确认",1);
				var1 = (unsigned char)(systems.a_temp / 10);
				var2 = (unsigned char)(systems.a_temp % 10);
				if(systems.set_temp_flag)
				{
					systems.manu.temperature = systems.a_temp;
				}
				else
				{
					systems.manu.temperature = (int)env_temp;
				}
				putchar(86,32,var1+'0',0);
				putchar(92,32,var2+'0',0);
				break;
				
		/*	case 14:	
				systems.set_temp_flag ^= 1;				
				if(systems.set_temp_flag)
				{
					draw_symbol(97,45,10,10,gou_mu,0);
				}
				else
				{
					fill_rect(97,45,10,10);
				}
				break;*/
			default: 
				break;
		}
}


void group_status_set( unsigned int x_pos, unsigned int y_pos )
{
	clr_rect( x_pos+60, y_pos, 12, 12 );
	draw_rect( x_pos+60, y_pos, 12, 12 );
	if( 1 == systems.groups[systems.set_group].srb )
	{
		draw_symbol( x_pos+61, y_pos+1, 10, 10, gou_mu, 0 );
	}
}




void draw_dialog( int mod )            //打印一个用于设置参数的对话框，在屏幕的左部
{ 
	clr_rect( 35, 0, 93, 64 );    //描绘一个椭圆框的处理过程
	draw_line( 38, 0, 124, 0 );
	draw_point( 37, 1 );
	draw_point( 36, 2 );

	if( mod )
	{
		draw_line( 35, 3, 35, 63 );
		draw_line( 35, 63, 124, 63 );
	}
	else
	{
		 draw_line( 35, 3, 35, 60 );
		 draw_point( 36, 61 );
		 draw_point( 37, 62 );
		 draw_line( 38, 63, 124, 63 );
	}

	draw_point( 125, 62 );
	draw_point( 126, 61 );

	draw_line( 127, 3, 127, 60 );
	draw_point( 125, 1 );
	draw_point( 126, 2 );

	draw_line( 40, 15, 122, 15 );       //打印一条靠标题栏的直线
}


void groupMoveFont(void)
{
    static unsigned int cyls = 0;
    static char *move[10] = { "第", "组","组正","组正在","组正在采","组正在采样",
                            "正在采样","在采样","采样","样" };
	
    static char *move_wait[10] = { "第", "组","组正","组正在","组正在等","组正在等待",
                            "正在等待","在等待","等待","待" };
    
    switch ( systems.status )
    {
		case SAMPLING:
			clr_rect( 35, 50, 98, 12 );
            switch ( cyls )
            {
            case 0:
                print_ch_str( 110, 50, move[0], 0 );
                break;
            case 1:
                print_ch_str( 98, 50, move[0], 0 );
                if ( systems.cur_group < 9 )
                {
                    print_int( 113, 50, systems.cur_group + 1, 0 );
                }
                else
                {
                    print_int( 110, 50, systems.cur_group + 1, 0 );
                }
                break;
            case 2:
                print_ch_str( 86, 50, move[0], 0 );
                if ( systems.cur_group < 9 )
                {
                    print_int( 101, 50, systems.cur_group + 1, 0 );
                }
                else
                {
                    print_int( 98, 50, systems.cur_group + 1, 0 );
                }
                print_ch_str( 110, 50, move[1], 0 );
                break;
            case 3:
                print_ch_str( 74, 50, move[0], 0 );
                if ( systems.cur_group < 9 )
                {
                    print_int( 89, 50, systems.cur_group + 1, 0 );
                }
                else
                {
                    print_int( 86, 50, systems.cur_group + 1, 0 );
                }
                print_ch_str( 98, 50, move[2], 0 );
                break;
            case 4:
                print_ch_str( 62, 50, move[0], 0 );
                if ( systems.cur_group < 9 )
                {
                    print_int( 77, 50, systems.cur_group + 1, 0 );
                }
                else
                {
                    print_int( 74, 50, systems.cur_group + 1, 0 );
                }
                print_ch_str( 86, 50, move[3], 0 );
                break;
            case 5:
				print_ch_str( 50, 50, move[0], 0 );
                if ( systems.cur_group < 9 )
                {
                    print_int( 65, 50, systems.cur_group + 1, 0 );
                }
                else
                {
                    print_int( 62, 50, systems.cur_group + 1, 0 );
                }
                print_ch_str( 74, 50, move[4], 0 );
                break;
				
            case 6:
				print_ch_str( 38, 50, move[0], 0 );
                if ( systems.cur_group < 9 )
                {
                    print_int( 53, 50, systems.cur_group + 1, 0 );
                }
                else
                {
                    print_int( 50, 50, systems.cur_group + 1, 0 );
                }
                print_ch_str( 62, 50, move[5], 0 );
                break;
				
            case 7:
			    if ( systems.cur_group < 9 )
                {
                    print_int( 41, 50, systems.cur_group + 1, 0 );
                }
                else
                {
                    print_int( 38, 50, systems.cur_group + 1, 0 );
                }
                print_ch_str( 50, 50, move[5], 0 );
				break;
				
            case 8:            
			case 9:	
			case 10:				
                print_ch_str( 38, 50, move[cyls-2], 0 );
				press_temp_show(98-12*(cyls-8),50,systems.envir_temp,systems.bar_pre,1);				
                break;

			case 11:
            case 12:		
			case 13:		
				press_temp_show(98-12*(cyls-8),50,systems.envir_temp,systems.bar_pre,1);				
                break;
			case 14:	
				print_float(38, 50,systems.envir_temp,1,0);	
				print_str(74,50,"P:     kpa",0);
				print_float(86, 50,systems.bar_pre,1,0);					
				draw_symbol(62,52,8,8,temp_dram,0);
				break;
			case 15:
				print_str(50,50,"P:     kpa",0);
				print_float(62, 50,systems.bar_pre,1,0);					
				draw_symbol(38,52,8,8,temp_dram,0);
				break;
			case 16:
				print_str(38,50,"P:     kpa",0);
				print_float(50, 50,systems.bar_pre,1,0);					
				break;
			case 17:
				print_str(38,50,"kpa",0);	
				break;
            default: break;
			
            }
        break;
		
		
    case WAITING:
			clr_rect( 35, 50, 92, 12 );
            switch ( cyls )
            {
            case 0:
                print_ch_str(110, 50, move_wait[0], 0 );
                break;
				
            case 1:
                print_ch_str( 98, 50, move_wait[0], 0 );
                if ( systems.cur_group < 9 )
                {
                    print_int( 113, 50, systems.cur_group + 1, 0 );
                }
                else
                {
                    print_int(110, 50, systems.cur_group + 1, 0 );
                }
                break;
				
            case 2:
                print_ch_str( 86, 50, move_wait[0], 0 );
                if ( systems.cur_group < 9 )
                {
                    print_int( 101, 50, systems.cur_group + 1, 0 );
                }
                else
                {
                    print_int( 98, 50, systems.cur_group + 1, 0 );
                }
                print_ch_str( 110, 50, move_wait[1], 0 );
                break;
				
            case 3:
                print_ch_str( 74, 50, move_wait[0], 0 );
                if ( systems.cur_group < 9 )
                {
                    print_int( 89, 50, systems.cur_group + 1, 0 );
                }
                else
                {
                    print_int( 86, 50, systems.cur_group + 1, 0 );
                }
                print_ch_str( 98, 50, move_wait[2], 0 );
                break;
				
            case 4:
                print_ch_str( 62, 50, move_wait[0], 0 );
                if ( systems.cur_group < 9 )
                {
                    print_int( 77, 50, systems.cur_group + 1, 0 );
                }
                else
                {
                    print_int( 74, 50, systems.cur_group + 1, 0 );
                }
                print_ch_str( 86, 50, move_wait[3], 0 );
                break;
				
            case 5:
                if ( systems.cur_group < 9 )
                {
                    print_int( 53, 50, systems.cur_group + 1, 0 );
                }
                else
                {
                    print_int( 50, 50, systems.cur_group + 1, 0 );
                }
                print_ch_str( 62, 50, move_wait[4], 0 );
                break;
				
            case 6:
				print_ch_str( 38, 50, move[0], 0 );
                if ( systems.cur_group < 9 )
                {
                    print_int( 53, 50, systems.cur_group + 1, 0 );
                }
                else
                {
                    print_int( 50, 50, systems.cur_group + 1, 0 );
                }
				print_ch_str( 62, 50, move_wait[5], 0 );
				break;
				
            case 7:
			    if ( systems.cur_group < 9 )
                {
                    print_int( 41, 50, systems.cur_group + 1, 0 );
                }
                else
                {
                    print_int( 38, 50, systems.cur_group + 1, 0 );
                }
                print_ch_str( 50, 50, move_wait[5], 0 );
				break;
            case 8:            
			case 9:	
			case 10:				
                print_ch_str( 38, 50, move_wait[cyls-2], 0 );
				press_temp_show(98-12*(cyls-8),50,systems.envir_temp,systems.bar_pre,1);				
                break;

			case 11:
            case 12:		
			case 13:		
				press_temp_show(98-12*(cyls-8),50,systems.envir_temp,systems.bar_pre,1);				
                break;
			case 14:	
				print_float(38, 50,systems.envir_temp,1,0);	
				print_str(74,50,"P:     kpa",0);
				print_float(86, 50,systems.bar_pre,1,0);					
				draw_symbol(62,52,8,8,temp_dram,0);
				break;
			case 15:
				print_str(50,50,"P:     kpa",0);
				print_float(62, 50,systems.bar_pre,1,0);					
				draw_symbol(38,52,8,8,temp_dram,0);
				break;
			case 16:
				print_str(38,50,"P:     kpa",0);
				print_float(50, 50,systems.bar_pre,1,0);					
				break;
			case 17:
				print_str(38,50,"kpa",0);	
				break;
                break;
            default: break;
            }
        break;
    }

    if ( 17 == cyls )
    {
        cyls = 0;
    }
    else
    {
        cyls++;
    }
}

void cylsMoveFont(void)
{
    static unsigned int cyls;
	
    static char *move[14] = { "第", "次", "次循", "次循环", "次循环正", "次循环正在", "次循环正在采",
                            "次循环正在采样","循环正在采样", "环正在采样", "正在采样","在采样", "采样", "样" };
    static char *move_wait[14] = { "第", "次","次循", "次循环", "次循环正", "次循环正在", "次循环正在等",
                            "次循环正在等待", "循环正在等待", "环正在等待","正在等待","在等待", "等待", "待" };
    
    switch ( systems.status )
    {
    	case SAMPLING:
			clr_rect( 35, 50, 98, 12 );
            switch ( cyls )
            {
            case 0:
                print_ch_str( 110, 50, move[0], 0 );
                break;
            case 1:
                print_ch_str( 98, 50, move[0], 0 );
                if ( systems.cur_cyls < 9 )
                {
                    print_int( 113, 50, systems.cur_cyls + 1, 0 );
                }
                else
                {
                    print_int( 110, 50, systems.cur_cyls + 1, 0 );
                }
                break;
            case 2:
                print_ch_str( 86, 50, move[0], 0 );
                if ( systems.cur_cyls < 9 )
                {
                    print_int( 101, 50, systems.cur_cyls + 1, 0 );
                }
                else
                {
                    print_int( 98, 50, systems.cur_cyls + 1, 0 );
                }
                print_ch_str( 110, 50, move[1], 0 );
                break;
            case 3:
                print_ch_str( 74, 50, move[0], 0 );
                if ( systems.cur_cyls < 9 )
                {
                    print_int( 89, 50, systems.cur_cyls + 1, 0 );
                }
                else
                {
                    print_int( 86, 50, systems.cur_cyls + 1, 0 );
                }
                print_ch_str( 98, 50, move[2], 0 );
                break;
            case 4:
                print_ch_str( 62, 50, move[0], 0 );
                if ( systems.cur_cyls < 9 )
                {
                    print_int( 77, 50, systems.cur_cyls + 1, 0 );
                }
                else
                {
                    print_int( 74, 50, systems.cur_cyls + 1, 0 );
                }
                print_ch_str( 86, 50, move[3], 0 );
                break;
				
            case 5:
				print_ch_str( 50, 50, move[0], 0 );
                if ( systems.cur_cyls < 9 )
                {
                    print_int( 65, 50, systems.cur_cyls + 1, 0 );
                }
                else
                {
                    print_int( 62, 50, systems.cur_cyls + 1, 0 );
                }
                print_ch_str( 74, 50, move[4], 0 );
                break;
				
            case 6:				
				print_ch_str( 38, 50, move[0], 0 );
                if ( systems.cur_cyls < 9 )
                {
                    print_int( 53, 50, systems.cur_cyls + 1, 0 );
                }
                else
                {
                    print_int( 50, 50, systems.cur_cyls + 1, 0 );
                }
                print_ch_str( 62, 50, move[5], 0 );
                break;
		
           case 7:
			    if ( systems.cur_cyls < 9 )
                {
                    print_int( 41, 50, systems.cur_cyls + 1, 0 );
                }
                else
                {
                    print_int( 38, 50, systems.cur_cyls + 1, 0 );
                }
                print_ch_str( 50, 50, move[6], 0 );
				break;
				
            case 8:            
			case 9:	
			case 10:	
			case 11:
				print_ch_str( 38, 50, move[cyls-1], 0 );
				break;
				
			case 12:	
			case 13:	
			case 14:	
                print_ch_str( 38, 50, move[cyls-1], 0 );
				press_temp_show(98-12*(cyls-12),50,systems.envir_temp,systems.bar_pre,1);				
                break;
				
			case 15:
			case 16:
            case 17:	
				press_temp_show(98-12*(cyls-12),50,systems.envir_temp,systems.bar_pre,1);				
                break;
				
			case 18:	
				print_float(38, 50,systems.envir_temp,1,0);	
				print_str(74,50,"P:     kpa",0);
				print_float(86, 50,systems.bar_pre,1,0);					
				draw_symbol(62,52,8,8,temp_dram,0);
				break;
			case 19:
				print_str(50,50,"P:     kpa",0);
				print_float(62, 50,systems.bar_pre,1,0);					
				draw_symbol(38,52,8,8,temp_dram,0);
				break;
			case 20:
				print_str(38,50,"P:     kpa",0);
				print_float(50, 50,systems.bar_pre,1,0);					
				break;
			case 21:
				print_str(38,50,"kpa",0);	
				break;
            default: break;
            }
			break;
			
    	case WAITING:
			clr_rect( 35, 50, 98, 12 );
            switch ( cyls )
            {
            case 0:
                print_ch_str( 110, 50, move_wait[0], 0 );
                break;
            case 1:
                print_ch_str( 98, 50, move_wait[0], 0 );
                if ( systems.cur_cyls < 9 )
                {
                    print_int( 113, 50, systems.cur_cyls + 1, 0 );
                }
                else
                {
                    print_int( 110, 50, systems.cur_cyls + 1, 0 );
                }
                break;
            case 2:
                print_ch_str( 86, 50, move_wait[0], 0 );
                if ( systems.cur_cyls < 9 )
                {
                    print_int( 101, 50, systems.cur_cyls + 1, 0 );
                }
                else
                {
                    print_int( 98, 50, systems.cur_cyls + 1, 0 );
                }
                print_ch_str( 110, 50, move_wait[1], 0 );
                break;
            case 3:
                print_ch_str( 74, 50, move_wait[0], 0 );
                if ( systems.cur_cyls < 9 )
                {
                    print_int( 89, 50, systems.cur_cyls + 1, 0 );
                }
                else
                {
                    print_int( 86, 50, systems.cur_cyls + 1, 0 );
                }
                print_ch_str( 98, 50, move_wait[2], 0 );
                break;
            case 4:
                print_ch_str( 62, 50, move_wait[0], 0 );
                if ( systems.cur_cyls < 9 )
                {
                    print_int( 77, 50, systems.cur_cyls + 1, 0 );
                }
                else
                {
                    print_int( 74, 50, systems.cur_cyls + 1, 0 );
                }
                print_ch_str( 86, 50, move_wait[3], 0 );
                break;
				
            case 5:
				print_ch_str( 50, 50, move_wait[0], 0 );
                if ( systems.cur_cyls < 9 )
                {
                    print_int( 65, 50, systems.cur_cyls + 1, 0 );
                }
                else
                {
                    print_int( 62, 50, systems.cur_cyls + 1, 0 );
                }
                print_ch_str( 74, 50, move_wait[4], 0 );
                break;
				
            case 6:				
				print_ch_str( 38, 50, move_wait[0], 0 );
                if ( systems.cur_cyls < 9 )
                {
                    print_int( 53, 50, systems.cur_cyls + 1, 0 );
                }
                else
                {
                    print_int( 50, 50, systems.cur_cyls + 1, 0 );
                }
                print_ch_str( 62, 50, move_wait[5], 0 );
                break;
		
           case 7:
			    if ( systems.cur_cyls < 9 )
                {
                    print_int( 41, 50, systems.cur_cyls + 1, 0 );
                }
                else
                {
                    print_int( 38, 50, systems.cur_cyls + 1, 0 );
                }
                print_ch_str( 50, 50, move_wait[6], 0 );
				break;
				
            case 8:            
			case 9:	
			case 10:	
			case 11:
				print_ch_str( 38, 50, move_wait[cyls-1], 0 );
				break;
				
			case 12:	
			case 13:	
			case 14:	
                print_ch_str( 38, 50, move_wait[cyls-1], 0 );
				press_temp_show(98-12*(cyls-12),50,systems.envir_temp,systems.bar_pre,1);				
                break;
				
			case 15:
			case 16:
            case 17:	
				press_temp_show(98-12*(cyls-12),50,systems.envir_temp,systems.bar_pre,1);				
                break;
				
			case 18:	
				print_float(38, 50,systems.envir_temp,1,0);	
				print_str(74,50,"P:     kpa",0);
				print_float(86, 50,systems.bar_pre,1,0);					
				draw_symbol(62,52,8,8,temp_dram,0);
				break;
			case 19:
				print_str(50,50,"P:     kpa",0);
				print_float(62, 50,systems.bar_pre,1,0);					
				draw_symbol(38,52,8,8,temp_dram,0);
				break;
			case 20:
				print_str(38,50,"P:     kpa",0);
				print_float(50, 50,systems.bar_pre,1,0);					
				break;
			case 21:
				print_str(38,50,"kpa",0);	
				break;
            default: break;
            }			
			break;
			
		case STOP:
	        if(systems.mod != 0)				
			{ 
				cyls = 0;
			} 
			break; 			
	}

    if ( 21 == cyls )
    {
        cyls = 0;
    }
    else
    {
        cyls++;
    }
}

void draw_rectmod(void)
{
    draw_rect(27,0,101,64);
    draw_rect(0,0,27,64);
    draw_line(1,1,25,1);
    draw_line(1,62,25,62);
    draw_line(1,2,1,61);
   
   draw_line(2,2,2,4);
    draw_line(3,2,3,3);
   draw_point(4,2);
    
    draw_line(25,2,25,4);
    draw_line(24,2,24,3);
    draw_point(23,2);
    
   draw_line(2,59,2,61);
    draw_line(3,60,3,61);
    draw_point(4,61);
    
    draw_line(25,59,25,61);
    draw_line(24,60,24,61);
    draw_point(23,61);
    
   //wan 
   // draw_line(27,2,27,61);
   // draw_line(26,2,26,61);
    
}

void calcCailTask(void)
{
	float pre = 0.0;
	float temp = 0.0;
	double cur_flow = 0.0;
	
	while(1)
	{
		if(systems.status == SAMPLING)
		{
			if(storeData.envT.f != 0)
			{
				temp = storeData.envT.f+273.15;     //温度
			}
			else
			{
				temp = 20 + 273.15;
			}
			pre = storeData.envP.f;          //压强
			cur_flow = 273.15*pre / 101.3;
			cur_flow = cur_flow/temp;
			cail_Flow = (float)(cur_flow*systems.dynamic_flow);
		}
		else
		{
			cail_Flow = 0;		
		}
		delayTick(600);
	}
}

