
#include "lcd.h"
#include "pump.h"
#include "MSP430x54x.h"
#include "key.h"
#include "adc12.h"
#include "stdlib.h"
#include "flash.h"
#include "display.h"
#include "math.h"
#include "port.h"
#include "tick.h"

#define   startPwm1()           TB0CCTL1 |= OUTMOD_7     //启动PWM1宏
#define   stopPwm1()            TB0CCTL1 &= ~OUTMOD_7    //停止PWM1宏
#define   setPwm1Duty(x)        TB0CCR1  = ((x))         //设置PWM1Duty宏
#define   setSampLed()              P9OUT &= ~BIT7       //点亮指示正采样LED宏
#define   clrSampLed()              P9OUT |= BIT7        //熄灭指示正在采样LED宏

#define new_2012_09_21

static unsigned int SHOW_ZHONGZHIDIR_LONG=1;

/*
#if MAX_FLOW==1500							//数据修改

#ifdef new_2012_09_21
static const unsigned int default_p[34]={10  ,20  ,30  ,40  ,50  ,50  , 50 , 65 ,98  ,164 ,201 ,283 ,317 ,442 ,461 ,636 ,648 ,855 ,836 ,1091 ,1062,1373,1302,1672,1558,1995,1860,2350,2168,2737,2523,3170,2886,3600};
/////////////////////////////////       {     10  ,   20  ,  30  ,  40  ,  50  ,  100 ,   150 ,  200 ,250 , 300 , 350 ,   400 ,  450 ,  500 ,550 ,  600 ,  650 , 700 , 750 ,   800 ,   850 ,  900 , 950 , 1000, 1050,1100, 1150, 1200, 1250, 1300,1350, 1400, 1450,1500}

#else
static const unsigned int default_p[34]={10  ,20  ,30  ,40  ,50  ,50  , 50 , 68 ,98  ,146 ,201 ,249 ,317 ,390 ,461 ,544 ,648 ,736 ,836 ,940 ,1062,1174,1302,1420,1558,1704,1860,1998,2168,2340,2523,2704,2886,3075};
/////////////////////////////////       {10  ,20  ,30  ,40  ,50  ,100  ,150,200 ,250 ,300 ,350 ,400 ,450 ,500 ,550 ,600 ,650 ,700 ,750 ,800 ,850 ,900 ,950 ,1000,1050,1100,1150,1200,1250,1300,1350,1400,1450,1500}
#endif

#else 
static const unsigned int default_p[34]={10  ,20  ,30  ,40  ,61  ,200 ,380 ,677 ,1017,1372,1807,2287,2852,3507,4001,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff };

#endif

*/

#if MAX_FLOW==5000	 	
static const unsigned int default_p[41]={101,115,150,180,211,238,281,318,357,399,440,490,538,580,840,690,745,810,875,930,1010,1090,1160,1230,1301,1390,1460,1540,1632,1708,1787,1881,1969,2070,2140,2230,2331,2420,2520,2608,2686};

#endif

//*************G该值为预设值 一般先初始化************************//
static unsigned int MAXSAMPFLOW = 5000;     //最大流量值				//修改
static unsigned int MINSAMPFLOW = 1000;       //最小流量值
static unsigned int MIDFLOW = 100;

static unsigned int PWM_T=200;
static unsigned int PWM_MIN=1000;									//修改
static unsigned int PWM_START=2000;
static unsigned int dutyVal=10;
//
static unsigned char startFlag = 0;  //气泵启动标志

static unsigned int smpPresure = 200;

static unsigned int smpFlow = 2000;

static unsigned int curFlow = 0;    //当前流量值


static int fault = 0; 

static int faultCnt = 0;

void set_PWM_MIN(unsigned int tt)
{
  PWM_MIN=tt;
  writeFlash(tt,0x1860);
    
}
void set_PWM_START(unsigned int tt)
{

 PWM_START=tt;
 writeFlash(tt,0x1862);
}




int get_smppresure()
{
return smpPresure;
}
void initTb0(void)                //初始化定时器TB0
{
    TB0CTL |= TBSSEL__SMCLK +     //使用SMCLK作为时钟源
              TBCLR + MC__UP;     //清空定时器基数器
    
    TB0CCR0 = 18000;               //PWM波周期
    
    TB0CCTL1 |= OUTMOD_7;         //TB1设置为输出模式
	
    TB0CCTL2 |= OUTMOD_0;         //TB2设置为输出模式
	
    PWM_MIN = readFlash((unsigned int*) 0x1860);
    PWM_START =readFlash((unsigned int*) 0x1862);

    if(PWM_MIN>18000)
    {
     PWM_MIN=1000;
     writeFlash(1000,0x1860);
     
    }
    if(PWM_START>18000)
    {
     PWM_START=2000; 
     writeFlash(2000,0x1862);
     
    }
    //read_flashf PWM_START PWM_MIN
    TB0CCR1 = PWM_START;                 //pwm占空比
    TB0CCR2 = 10;
}




void initPumpIo(void)   // 初始化PWM IO输出端口
{    
    P4DIR |= BIT1;      //使P4.1为输出端口
    P4SEL |= BIT1;      //使P4.1为PWM波输出
    
    P4DIR |= BIT2;      //使P4.2为输出口
    P4SEL |= BIT2;      //使P4.2为PWM输出口
    
    P9DIR |= BIT7;      //使P9.7为输出口
    P9OUT |= BIT7;     //使P9.7输出为高电平
	
	P6DIR |= (BIT6 + BIT7);
	P6OUT &= ~(BIT6 + BIT7);     //使P9.7输出为高电平
}


void initPumpPwm(void)       //初始化PWM波发生器函数
{
    initTb0();               //初始化TB0
    initPumpIo();            //初始化电机控制IO
}

void selectPump(unsigned int flow)
{
	/*if(flow >= FLOW_SELECT)
	{									//k2泵运行
		P6OUT &= ~BIT7;     //使P9.7输出为高电平
		P6OUT |= BIT6;     //使P9.7输出为高电平
	}
	else 							//k1泵运行
	{
		P6OUT |= BIT7;     //使P9.7输出为高电平
		P6OUT &= ~BIT6;     //使P9.7输出为高电平
	}	*/
	P6OUT &= ~BIT7;     //使P9.7输出为高电平
	P6OUT |= BIT6;     //使P9.7输出为高电平
}

void setMaxFlow(unsigned int maxFlow)
{
    enterCritical();   //关中断
    
    MAXSAMPFLOW = maxFlow;
    
    exitCritical();    //开中断
}

void setMinFlow(unsigned int minFlow)
{
    enterCritical();   //关中断
    
    MINSAMPFLOW = minFlow;
    
    exitCritical();    //开中断
}

void setMidflow(unsigned int midFlow)
{
    enterCritical();   //关中断
    
    MIDFLOW = midFlow;
    
    exitCritical();    //开中断
}



void startPump(int flow)       //启动采样函数
{
    int i;
    unsigned int readPresure;
    //unsigned int readI;        //读出对应流量的电流值
    
    if(flow > STOP_FLOW)        //判断需要采样的流量值是否大于最大值，如果是则设置为最大值
    {
        flow = STOP_FLOW;
    }
    
    if(flow < START_FLOW)       //判断需要采样的流量是否小于最小值，如果是设置为最小值
    {
        flow = START_FLOW;
    }
    
 /*   if(flow < 50)
    {
        i = ((int)(flow - 10)/10);
        
        i = i << 1;
        
        readPresure = readFlash((unsigned int*)(F_P_TABLE+i));
        
    }
	else
    {
        i = ((int)(flow - 50)/50) + 4 ; //获得Flow-Presure表的索引值
        
        i = i << 1;
        
        readPresure = readFlash((unsigned int*)(F_P_TABLE+i));    //
        
    }*/
	i = ((int)(flow - 1000)/100);
	
	i = i << 1;
	
	readPresure = readFlash((unsigned int*)(F_P_TABLE+i));
    
    enterCritical();    //关中断
    
    smpPresure = readPresure;  //设置采样压力值
    
    faultCnt = 0;
    fault = 0;
    smpFlow = flow;
	
    selectPump(smpFlow);
	
    dutyVal = PWM_START;
    
    setPwm1Duty(dutyVal);       //设置Pwm1Duty
    
    startPwm1();             //启动Pwm1
    
    startFlag = 1;            //使控制标志为启动状态
    
    setSampLed();            //点亮指示正采样LED灯
    
    exitCritical();    //开中断
}


void stopPump(void)            //停止采样函数
{
    enterCritical();    //关中断
    
    setPwm1Duty(PWM_START);          //使duty为10
    stopPwm1();               //停止Pwm1

    startFlag = 0;           //使标志为停止状态
    
    
    
    
    clrSampLed();            //熄灭指示正采样LED灯
    
    exitCritical();    //开中断
}


void changeFlow(int flow)      //改变流量函数
{
    int i;
    unsigned int readPresure;
    //unsigned int readI;
    
    if(flow > STOP_FLOW)//判断需要采样的流量值是否大于最大值，如果是则设置为最大值
    {
        flow = STOP_FLOW;
    }
    
    if(flow < START_FLOW)//判断需要采样的流量是否小于最小值，如果是设置为最小值
    {
        flow = START_FLOW;
    }
    
   /* if(flow < 50)
    {
        i = ((int)(flow - 10)/10);
        
        i = i << 1;
        
        
        readPresure = readFlash((unsigned int*)(F_P_TABLE+i));
        //readI       = readFlash((unsigned int*)(F_I_TABLE+i));
    }else
    {
        i = ((int)(flow - 50)/50) + 4 ; //获得Flow-Presure表的索引值
        
        i = i << 1;
        
        readPresure = readFlash((unsigned int*)(F_P_TABLE+i));  
        //readI       = readFlash((unsigned int*)(F_I_TABLE+i));
    }*/
	
	i = ((int)(flow - 1000)/100);
	
	i = i << 1;
	
	
	readPresure = readFlash((unsigned int*)(F_P_TABLE+i));
    
    enterCritical();         //关中断
    
    smpPresure = readPresure;
    if(smpPresure<2000)
     set_SAMP_T(6);
    else
     set_SAMP_T(4);
    smpFlow = flow;
    selectPump(smpFlow);
    exitCritical();          //开中断
}


void changePresure(unsigned int Presure)//改变当前PID控制压力值
{                                       //此函数在校准流量时使用
    enterCritical();         //关中断
    
    smpPresure = Presure;
    
    exitCritical();          //开中断
   
}



unsigned int getPFromFlow(unsigned int flow)  //从流量获得压力值，主要是访问
{                                             //Flow-Presure表获得
    unsigned int re = 0;
    int i;

    
    if(flow > STOP_FLOW)//判断需要采样的流量值是否大于最大值，如果是则设置为最大值
    {
        flow = STOP_FLOW;
    }
    
    if(flow < START_FLOW)//判断需要采样的流量是否小于最小值，如果是设置为最小值
    {
        flow = START_FLOW;
    }
    

   /* if(flow < 50)
    {
        i = ((int)(flow - 10)/10);
        
        i = i << 1;
        
        re = readFlash((unsigned int*)(F_P_TABLE+i));
    }else
    {*/
        i = ((int)(flow - 1000)/100); //获得Flow-Presure表的索引值
        
        i = i << 1;
        
        re = readFlash((unsigned int*)(F_P_TABLE+i));    //将对应的压力值写入到Flow-Presure表   
   // }
    
    return re;
}



void writePToFlow(unsigned int p,unsigned int flow)  //将flow对应的presure值写入到
{                                                    //Flow-Presure表
    int i;
    
  /*  if(flow > STOP_FLOW)//判断需要采样的流量值是否大于最大值，如果是则设置为最大值
    {
        flow = STOP_FLOW;
    }
    
    if(flow < START_FLOW)//判断需要采样的流量是否小于最小值，如果是设置为最小值
    {
        flow = START_FLOW;
    }
    
    
    if(flow < 50)
    {
        i = ((int)(flow - 10)/10) << 1;
        enterCritical();      //关中断
        writeFlash(p,F_P_TABLE+i);
        exitCritical();       //开中断
        
    }else
    {
        i = (((int)(flow - 50)/50) + 4) << 1 ; //获得Flow-Presure表的索引值
        enterCritical();      //关中断
        writeFlash(p,F_P_TABLE+i);    //将对应的压力值写入到Flow-Presure表   
        exitCritical();       //开中断
    }*/
	
	//修改为EM-5000  流量为1000~5000
	
	if(flow > STOP_FLOW)//判断需要采样的流量值是否大于最大值，如果是则设置为最大值
    {
        flow = STOP_FLOW;
    }
    
    if(flow < START_FLOW)//判断需要采样的流量是否小于最小值，如果是设置为最小值
    {
        flow = START_FLOW;
    }
	

	i = ((int)(flow - 1000)/100) << 1;
	enterCritical();      //关中断
	writeFlash(p,F_P_TABLE+i);
	exitCritical();       //开中断
	
}


void set_default(void)
{
  static unsigned int i=0;
  clr_lcd();
  draw_rect(25,19,83,20);
  print_ch_str(48,40,"恢复中",0);
  
  enterCritical();
  for(i=0;i<41;i++)
  {   
  writeFlash(default_p[i],F_P_TABLE+i*2);   
  
  if(i<40)fill_rect(25,19,i*2,20);
  else
    print_ch_str(48,40,"完  成",0);
    delay(10000);
    
      
  }
  exitCritical();       //开中断
}

void calcuTask(void)  //计算瞬态流量任务
{
    unsigned int actPre = 0;     //实际采样流量值
    
    unsigned int p1 = 0;
    unsigned int p2 = 0;
    
    unsigned int flow = 0;
    unsigned int stopPre = 10;    //在停止状态下的压力值，用于使流量归0
    
    while(1)
    {
        if(startFlag)
        {
            actPre = getPress();  //获得当前实际压力值
            
            
            for(flow = MINSAMPFLOW;flow <= MAXSAMPFLOW; flow += MIDFLOW)
            {
                if(flow == MINSAMPFLOW)         //如果采样处于最小处边界
                {
                    p1 = getPFromFlow(flow);
                    p2 = getPFromFlow(flow + MIDFLOW);
                    
                    if(actPre < p1)   //压力值小于最小流量处的压力值
                    {
                        enterCritical();   //关中断
                        curFlow = flow - (unsigned int)(MIDFLOW /(1.0*(p2 - p1))*(p1 - actPre));
                        if(actPre <= stopPre)
                        {
                            curFlow = 0;
                        }
                        exitCritical();    //开中断
                        break;
                    }
                }else if(flow == MAXSAMPFLOW)   //如果处于最大处边界
                {
                    p1 = getPFromFlow(flow - MIDFLOW);
                    p2 = getPFromFlow(flow);
                    
                    if(actPre >= p1)
                    {
                        enterCritical();   //开中断
                        curFlow = (unsigned int)(MIDFLOW /(1.0*(p2 - p1))*(actPre - p1)) + flow - MIDFLOW;
                        if(actPre <= stopPre)
                        {
                            curFlow = 0;
                        }
                        exitCritical();    //关中断
                        break;
                    }
                }else
                {
                    p1 = getPFromFlow(flow - MIDFLOW);
                    p2 = getPFromFlow(flow);
                    
                    if(actPre >= p1 && actPre < p2)
                    {
                        enterCritical();   //开中断
                        curFlow = (unsigned int)(MIDFLOW /(1.0*(p2 - p1))*(actPre - p1)) + flow - MIDFLOW;
                        if(actPre <= stopPre)
                        {
                            
                            curFlow = 0;
                        }
                        exitCritical();    //关中断
                        break;
                    }
                }
            }
            
            if(curFlow <= 0)    //出错处理
            {
                if(faultCnt >= 200)
                {
                    enterCritical();
                    fault  = 1;
                    exitCritical(); 
                }else
                {
                    faultCnt ++;
                }
                
            }else
            {
                faultCnt = 0;
            }
            
            if(curFlow < 100 && dutyVal >= 8998*2)
            {
                enterCritical();
                fault = 1;
                exitCritical(); 
            }
            
            
            if(curFlow > smpFlow)
            {
                if(curFlow - smpFlow > 0.05*smpFlow)
                {
                    clrSampLed();    //使LED灯灭
                }else
                {
                    setSampLed();
                }
            }
            
            if(curFlow <= smpFlow)
            {
                if(smpFlow - curFlow > 0.05*smpFlow)
                {
                    clrSampLed();
                }else
                {
                    setSampLed();
                }
            }
            
        }else
        {
            enterCritical();                      //关中断
            
            curFlow = 0;                //没有启动采样，则采样流量为0
            
            exitCritical();                       //开中断
            
            
          
            if(getPress()>20)
            {
             stopPre = 20; 
            }
            else
            {
             stopPre = getPress() + 7;    //获得在停止状态下的压力值
            }
        }
        delayTick(100);
    }
}




int getFault(void)    //获得采样错误状态
{
    int curFault = 0;
    
    enterCritical();
    curFault = fault;   //获得错误状态
    exitCritical(); 
    return curFault;
}
int zhongzhi_dir(int fl)
{
  static unsigned int NUM=1;
  static unsigned int fl_min=0;
  static unsigned int fl_max=0;
  static unsigned int fl_out=0;
  
  if(NUM==1)
  {
      
  
   fl_min=fl_max=fl;
   
   if(SHOW_ZHONGZHIDIR_LONG==1)
   {
    fl_out=fl;
   }
  }  
  else
  {
    
     if(fl>fl_max)
     fl_max=fl;
     if(fl<fl_min)
     fl_min=fl; 
    
     if(NUM>=SHOW_ZHONGZHIDIR_LONG)
     {
        fl_out=(fl_max+fl_min)/2;
        NUM=0;
        
     }
     
  }
    NUM++;   
  return fl_out;  

}





unsigned int getCurFlow(void)
{
    unsigned int re = 0;
    
    enterCritical();
    re = zhongzhi_dir(curFlow);
    exitCritical(); 
    return re;
}
unsigned int cali_leve(unsigned int actnum,unsigned int smpnum)//0：2%误差，1：2-6%，2：6-11%，3：11-31%
{
    unsigned int ee=0; 
    // 4:31-61%,5:61-%
  if(actnum<smpnum)  
  {
    ee=(smpnum-actnum)*100/smpnum;
    
  }
  else if(actnum>smpnum)
  {
    ee=(actnum-smpnum)*100/smpnum;
    
    if(smpnum<100)
    {
        return 1;
    }
    
    if(ee>11)
    {
        
        return 6;
    }     
  }
  
  if(ee<3)
  {
   return 0;
      
  }else if(ee<6)
  {
     return 1;  
  }
  else if(ee<11)
  {
    if(smpnum<100)
    return 1;    
    return 2;
  }
  else if(ee<31)
  {
    return 3;
  }
  else if(ee<61)
  {
      
    return 4;
  }
  else
  {
   return 5;
  }
}


void pumpLoop(void)
{
    unsigned int actPresure = 0;    //实际压力采样值
    int err = 0;
   long int ok = 0;
    
    initPumpPwm();     //初始化PWM
    
    while(1)
    {
        
        if(startFlag)
        {
            actPresure = getPress();    //获得压力值
            
            err = smpPresure - actPresure;   //获得采样差值

            
            if(actPresure >= 4095 && dutyVal > 8050*2)
            {
               if(smpPresure<2700)
                ok = (long int)(err*10.0);
                else 
                ok=(long int)(err*3.0);
                
            }else if(actPresure >= 4095 && dutyVal > 7000*2) 
            {
                ok = (long int)(err*5.0);     //仅使用P值
            }else if(actPresure >= 4095 && dutyVal > 3500*2)
            {
                ok = (int)(err*0.4);
            }else
            {
                  switch(cali_leve(actPresure,smpPresure))
            {//0：2%误差，1：2-6%，2：6-11%，3：11-31%
             // 4:31-61%,5:61-%
             case 0: PWM_T=400;
             break;
             case 1: PWM_T=300;
             break;
             case 2: PWM_T=200;
             break;
             case 3: PWM_T=170;
             break;
             case 4: PWM_T=150;           
             break;
             case 5:PWM_T=100;            
             break; 
            case 6:PWM_T=200;
            break;
            default:
                break;
            }
                  ok = (int)(err*0.11);

            }            
            
            if(ok==0)
            { 
             ok=(int)(err*0.4); 
             if(ok==0)
             {
              ok=(int)(err*0.6);
             }   
     
            }

               if(ok > 200)
                {
                   ok = 200;
                }else if(ok < -300)
                {
                   ok = -300;
                }          
            
            dutyVal += ok;
            
            if(dutyVal > 8998*2)   //进行限值
            {
                dutyVal = 8998*2;
            }
            
            if(dutyVal < PWM_MIN)
            {
                dutyVal = PWM_MIN;
            }
            setPwm1Duty(dutyVal);
        }
        delayTick(PWM_T);
    }
}