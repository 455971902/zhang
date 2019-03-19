
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

#define   startPwm1()           TB0CCTL1 |= OUTMOD_7     //����PWM1��
#define   stopPwm1()            TB0CCTL1 &= ~OUTMOD_7    //ֹͣPWM1��
#define   setPwm1Duty(x)        TB0CCR1  = ((x))         //����PWM1Duty��
#define   setSampLed()              P9OUT &= ~BIT7       //����ָʾ������LED��
#define   clrSampLed()              P9OUT |= BIT7        //Ϩ��ָʾ���ڲ���LED��

#define new_2012_09_21

static unsigned int SHOW_ZHONGZHIDIR_LONG=1;

/*
#if MAX_FLOW==1500							//�����޸�

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

//*************G��ֵΪԤ��ֵ һ���ȳ�ʼ��************************//
static unsigned int MAXSAMPFLOW = 5000;     //�������ֵ				//�޸�
static unsigned int MINSAMPFLOW = 1000;       //��С����ֵ
static unsigned int MIDFLOW = 100;

static unsigned int PWM_T=200;
static unsigned int PWM_MIN=1000;									//�޸�
static unsigned int PWM_START=2000;
static unsigned int dutyVal=10;
//
static unsigned char startFlag = 0;  //����������־

static unsigned int smpPresure = 200;

static unsigned int smpFlow = 2000;

static unsigned int curFlow = 0;    //��ǰ����ֵ


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
void initTb0(void)                //��ʼ����ʱ��TB0
{
    TB0CTL |= TBSSEL__SMCLK +     //ʹ��SMCLK��Ϊʱ��Դ
              TBCLR + MC__UP;     //��ն�ʱ��������
    
    TB0CCR0 = 18000;               //PWM������
    
    TB0CCTL1 |= OUTMOD_7;         //TB1����Ϊ���ģʽ
	
    TB0CCTL2 |= OUTMOD_0;         //TB2����Ϊ���ģʽ
	
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
    TB0CCR1 = PWM_START;                 //pwmռ�ձ�
    TB0CCR2 = 10;
}




void initPumpIo(void)   // ��ʼ��PWM IO����˿�
{    
    P4DIR |= BIT1;      //ʹP4.1Ϊ����˿�
    P4SEL |= BIT1;      //ʹP4.1ΪPWM�����
    
    P4DIR |= BIT2;      //ʹP4.2Ϊ�����
    P4SEL |= BIT2;      //ʹP4.2ΪPWM�����
    
    P9DIR |= BIT7;      //ʹP9.7Ϊ�����
    P9OUT |= BIT7;     //ʹP9.7���Ϊ�ߵ�ƽ
	
	P6DIR |= (BIT6 + BIT7);
	P6OUT &= ~(BIT6 + BIT7);     //ʹP9.7���Ϊ�ߵ�ƽ
}


void initPumpPwm(void)       //��ʼ��PWM������������
{
    initTb0();               //��ʼ��TB0
    initPumpIo();            //��ʼ���������IO
}

void selectPump(unsigned int flow)
{
	/*if(flow >= FLOW_SELECT)
	{									//k2������
		P6OUT &= ~BIT7;     //ʹP9.7���Ϊ�ߵ�ƽ
		P6OUT |= BIT6;     //ʹP9.7���Ϊ�ߵ�ƽ
	}
	else 							//k1������
	{
		P6OUT |= BIT7;     //ʹP9.7���Ϊ�ߵ�ƽ
		P6OUT &= ~BIT6;     //ʹP9.7���Ϊ�ߵ�ƽ
	}	*/
	P6OUT &= ~BIT7;     //ʹP9.7���Ϊ�ߵ�ƽ
	P6OUT |= BIT6;     //ʹP9.7���Ϊ�ߵ�ƽ
}

void setMaxFlow(unsigned int maxFlow)
{
    enterCritical();   //���ж�
    
    MAXSAMPFLOW = maxFlow;
    
    exitCritical();    //���ж�
}

void setMinFlow(unsigned int minFlow)
{
    enterCritical();   //���ж�
    
    MINSAMPFLOW = minFlow;
    
    exitCritical();    //���ж�
}

void setMidflow(unsigned int midFlow)
{
    enterCritical();   //���ж�
    
    MIDFLOW = midFlow;
    
    exitCritical();    //���ж�
}



void startPump(int flow)       //������������
{
    int i;
    unsigned int readPresure;
    //unsigned int readI;        //������Ӧ�����ĵ���ֵ
    
    if(flow > STOP_FLOW)        //�ж���Ҫ����������ֵ�Ƿ�������ֵ�������������Ϊ���ֵ
    {
        flow = STOP_FLOW;
    }
    
    if(flow < START_FLOW)       //�ж���Ҫ�����������Ƿ�С����Сֵ�����������Ϊ��Сֵ
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
        i = ((int)(flow - 50)/50) + 4 ; //���Flow-Presure�������ֵ
        
        i = i << 1;
        
        readPresure = readFlash((unsigned int*)(F_P_TABLE+i));    //
        
    }*/
	i = ((int)(flow - 1000)/100);
	
	i = i << 1;
	
	readPresure = readFlash((unsigned int*)(F_P_TABLE+i));
    
    enterCritical();    //���ж�
    
    smpPresure = readPresure;  //���ò���ѹ��ֵ
    
    faultCnt = 0;
    fault = 0;
    smpFlow = flow;
	
    selectPump(smpFlow);
	
    dutyVal = PWM_START;
    
    setPwm1Duty(dutyVal);       //����Pwm1Duty
    
    startPwm1();             //����Pwm1
    
    startFlag = 1;            //ʹ���Ʊ�־Ϊ����״̬
    
    setSampLed();            //����ָʾ������LED��
    
    exitCritical();    //���ж�
}


void stopPump(void)            //ֹͣ��������
{
    enterCritical();    //���ж�
    
    setPwm1Duty(PWM_START);          //ʹdutyΪ10
    stopPwm1();               //ֹͣPwm1

    startFlag = 0;           //ʹ��־Ϊֹͣ״̬
    
    
    
    
    clrSampLed();            //Ϩ��ָʾ������LED��
    
    exitCritical();    //���ж�
}


void changeFlow(int flow)      //�ı���������
{
    int i;
    unsigned int readPresure;
    //unsigned int readI;
    
    if(flow > STOP_FLOW)//�ж���Ҫ����������ֵ�Ƿ�������ֵ�������������Ϊ���ֵ
    {
        flow = STOP_FLOW;
    }
    
    if(flow < START_FLOW)//�ж���Ҫ�����������Ƿ�С����Сֵ�����������Ϊ��Сֵ
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
        i = ((int)(flow - 50)/50) + 4 ; //���Flow-Presure�������ֵ
        
        i = i << 1;
        
        readPresure = readFlash((unsigned int*)(F_P_TABLE+i));  
        //readI       = readFlash((unsigned int*)(F_I_TABLE+i));
    }*/
	
	i = ((int)(flow - 1000)/100);
	
	i = i << 1;
	
	
	readPresure = readFlash((unsigned int*)(F_P_TABLE+i));
    
    enterCritical();         //���ж�
    
    smpPresure = readPresure;
    if(smpPresure<2000)
     set_SAMP_T(6);
    else
     set_SAMP_T(4);
    smpFlow = flow;
    selectPump(smpFlow);
    exitCritical();          //���ж�
}


void changePresure(unsigned int Presure)//�ı䵱ǰPID����ѹ��ֵ
{                                       //�˺�����У׼����ʱʹ��
    enterCritical();         //���ж�
    
    smpPresure = Presure;
    
    exitCritical();          //���ж�
   
}



unsigned int getPFromFlow(unsigned int flow)  //���������ѹ��ֵ����Ҫ�Ƿ���
{                                             //Flow-Presure����
    unsigned int re = 0;
    int i;

    
    if(flow > STOP_FLOW)//�ж���Ҫ����������ֵ�Ƿ�������ֵ�������������Ϊ���ֵ
    {
        flow = STOP_FLOW;
    }
    
    if(flow < START_FLOW)//�ж���Ҫ�����������Ƿ�С����Сֵ�����������Ϊ��Сֵ
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
        i = ((int)(flow - 1000)/100); //���Flow-Presure�������ֵ
        
        i = i << 1;
        
        re = readFlash((unsigned int*)(F_P_TABLE+i));    //����Ӧ��ѹ��ֵд�뵽Flow-Presure��   
   // }
    
    return re;
}



void writePToFlow(unsigned int p,unsigned int flow)  //��flow��Ӧ��presureֵд�뵽
{                                                    //Flow-Presure��
    int i;
    
  /*  if(flow > STOP_FLOW)//�ж���Ҫ����������ֵ�Ƿ�������ֵ�������������Ϊ���ֵ
    {
        flow = STOP_FLOW;
    }
    
    if(flow < START_FLOW)//�ж���Ҫ�����������Ƿ�С����Сֵ�����������Ϊ��Сֵ
    {
        flow = START_FLOW;
    }
    
    
    if(flow < 50)
    {
        i = ((int)(flow - 10)/10) << 1;
        enterCritical();      //���ж�
        writeFlash(p,F_P_TABLE+i);
        exitCritical();       //���ж�
        
    }else
    {
        i = (((int)(flow - 50)/50) + 4) << 1 ; //���Flow-Presure�������ֵ
        enterCritical();      //���ж�
        writeFlash(p,F_P_TABLE+i);    //����Ӧ��ѹ��ֵд�뵽Flow-Presure��   
        exitCritical();       //���ж�
    }*/
	
	//�޸�ΪEM-5000  ����Ϊ1000~5000
	
	if(flow > STOP_FLOW)//�ж���Ҫ����������ֵ�Ƿ�������ֵ�������������Ϊ���ֵ
    {
        flow = STOP_FLOW;
    }
    
    if(flow < START_FLOW)//�ж���Ҫ�����������Ƿ�С����Сֵ�����������Ϊ��Сֵ
    {
        flow = START_FLOW;
    }
	

	i = ((int)(flow - 1000)/100) << 1;
	enterCritical();      //���ж�
	writeFlash(p,F_P_TABLE+i);
	exitCritical();       //���ж�
	
}


void set_default(void)
{
  static unsigned int i=0;
  clr_lcd();
  draw_rect(25,19,83,20);
  print_ch_str(48,40,"�ָ���",0);
  
  enterCritical();
  for(i=0;i<41;i++)
  {   
  writeFlash(default_p[i],F_P_TABLE+i*2);   
  
  if(i<40)fill_rect(25,19,i*2,20);
  else
    print_ch_str(48,40,"��  ��",0);
    delay(10000);
    
      
  }
  exitCritical();       //���ж�
}

void calcuTask(void)  //����˲̬��������
{
    unsigned int actPre = 0;     //ʵ�ʲ�������ֵ
    
    unsigned int p1 = 0;
    unsigned int p2 = 0;
    
    unsigned int flow = 0;
    unsigned int stopPre = 10;    //��ֹͣ״̬�µ�ѹ��ֵ������ʹ������0
    
    while(1)
    {
        if(startFlag)
        {
            actPre = getPress();  //��õ�ǰʵ��ѹ��ֵ
            
            
            for(flow = MINSAMPFLOW;flow <= MAXSAMPFLOW; flow += MIDFLOW)
            {
                if(flow == MINSAMPFLOW)         //�������������С���߽�
                {
                    p1 = getPFromFlow(flow);
                    p2 = getPFromFlow(flow + MIDFLOW);
                    
                    if(actPre < p1)   //ѹ��ֵС����С��������ѹ��ֵ
                    {
                        enterCritical();   //���ж�
                        curFlow = flow - (unsigned int)(MIDFLOW /(1.0*(p2 - p1))*(p1 - actPre));
                        if(actPre <= stopPre)
                        {
                            curFlow = 0;
                        }
                        exitCritical();    //���ж�
                        break;
                    }
                }else if(flow == MAXSAMPFLOW)   //���������󴦱߽�
                {
                    p1 = getPFromFlow(flow - MIDFLOW);
                    p2 = getPFromFlow(flow);
                    
                    if(actPre >= p1)
                    {
                        enterCritical();   //���ж�
                        curFlow = (unsigned int)(MIDFLOW /(1.0*(p2 - p1))*(actPre - p1)) + flow - MIDFLOW;
                        if(actPre <= stopPre)
                        {
                            curFlow = 0;
                        }
                        exitCritical();    //���ж�
                        break;
                    }
                }else
                {
                    p1 = getPFromFlow(flow - MIDFLOW);
                    p2 = getPFromFlow(flow);
                    
                    if(actPre >= p1 && actPre < p2)
                    {
                        enterCritical();   //���ж�
                        curFlow = (unsigned int)(MIDFLOW /(1.0*(p2 - p1))*(actPre - p1)) + flow - MIDFLOW;
                        if(actPre <= stopPre)
                        {
                            
                            curFlow = 0;
                        }
                        exitCritical();    //���ж�
                        break;
                    }
                }
            }
            
            if(curFlow <= 0)    //������
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
                    clrSampLed();    //ʹLED����
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
            enterCritical();                      //���ж�
            
            curFlow = 0;                //û���������������������Ϊ0
            
            exitCritical();                       //���ж�
            
            
          
            if(getPress()>20)
            {
             stopPre = 20; 
            }
            else
            {
             stopPre = getPress() + 7;    //�����ֹͣ״̬�µ�ѹ��ֵ
            }
        }
        delayTick(100);
    }
}




int getFault(void)    //��ò�������״̬
{
    int curFault = 0;
    
    enterCritical();
    curFault = fault;   //��ô���״̬
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
unsigned int cali_leve(unsigned int actnum,unsigned int smpnum)//0��2%��1��2-6%��2��6-11%��3��11-31%
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
    unsigned int actPresure = 0;    //ʵ��ѹ������ֵ
    int err = 0;
   long int ok = 0;
    
    initPumpPwm();     //��ʼ��PWM
    
    while(1)
    {
        
        if(startFlag)
        {
            actPresure = getPress();    //���ѹ��ֵ
            
            err = smpPresure - actPresure;   //��ò�����ֵ

            
            if(actPresure >= 4095 && dutyVal > 8050*2)
            {
               if(smpPresure<2700)
                ok = (long int)(err*10.0);
                else 
                ok=(long int)(err*3.0);
                
            }else if(actPresure >= 4095 && dutyVal > 7000*2) 
            {
                ok = (long int)(err*5.0);     //��ʹ��Pֵ
            }else if(actPresure >= 4095 && dutyVal > 3500*2)
            {
                ok = (int)(err*0.4);
            }else
            {
                  switch(cali_leve(actPresure,smpPresure))
            {//0��2%��1��2-6%��2��6-11%��3��11-31%
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
            
            if(dutyVal > 8998*2)   //������ֵ
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