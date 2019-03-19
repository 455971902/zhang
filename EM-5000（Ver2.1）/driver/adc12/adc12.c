


#include "adc12.h"
#include "port.h"
#include "tick.h"
#include "semaphore.h"

#include "MSP430x54x.h"

#include "lcd.h"


static struct SEMAPHORE adc12Sem;

__no_init static unsigned int preBuf[PRE_BUF_SIZE];   //压力数据缓冲
__no_init static struct SMP_DATA smpData[SAMP_BUF_SIZE];
static unsigned int SAMP_T=6;

static unsigned char adcSmpFlag = 1;    //表示启用AD采样

static int otherComp = 0;     //除压力数据缓冲区外胡数据缓冲区第一次充满标志变量
static int preComp = 0;       //压力数据缓冲区第一次充满标、、
static unsigned long battery1Sum = 0; 	//
static unsigned long presureSum = 0;	//泵采样值
static unsigned long pumpCurSum = 0; 	//
static unsigned long pumpVoltSum = 0;	//

//static unsigned long DQSum = 0;
//static unsigned long battery2Sum = 0;
//static unsigned long tempSum = 0;
//static unsigned long chargeCurSum = 0;


static unsigned int battery1 = 0;			//电池电量
static unsigned int press_AD = 0;			//泵采样AD值
static unsigned int pump_I = 0;				//电机电流采样
static unsigned int pump_V = 0;				//电机电压采样

/*static unsigned int DQ = 0;
static unsigned int battery2 = 0;
static unsigned int temp = 0;
static unsigned int chargeCurrent = 0;
*/

void set_SAMP_T(unsigned int tt)
{
 
    SAMP_T=tt;

}

unsigned int get_SAMP_T(void)
{

  return SAMP_T;
}

void initBuf(void)
{
    int i;
    
    for(i = 0;i < SAMP_BUF_SIZE; i++)
    {        
        smpData[i].battery1 = 0;
        smpData[i].pump_I = 0;
        smpData[i].pump_V = 0;
    }
    
    for(i = 0; i < PRE_BUF_SIZE; i++)
    {
        preBuf[i] = 0;
    }
}


void initAdc(void)   // 初始化AD采样函数
{
    ADC12CTL0 |= ADC12MSC +  //队列采样出发模式 
                 ADC12ON + ADC12REFON;    //AD核心开启
    
    ADC12CTL1 |= ADC12SHP +      //使用触发采样时钟模式采样
                 ADC12SSEL_2 +   //使用ACLK作为AD核心时钟
                 ADC12CONSEQ_1;  //使用单次队列模式采样
    ADC12CTL2 |= ADC12PDIV + ADC12RES_2;
    
    
    ADC12MCTL0 |= ADC12INCH_1;  //通道1,设备开机状态信号
    
    ADC12MCTL1 |= ADC12INCH_3;  //通道3,压力传感器输入信号
    
    ADC12MCTL2 |= ADC12INCH_13;  //通道5，温度传感器输入信号
    
    ADC12MCTL3 |= ADC12INCH_14 + //通道4,充电电流输入信号
                  ADC12EOS;      //设置为最后一个采样胡通道
    
    P6OUT &= ~(BIT1 + BIT3);
    P6DIR &= ~(BIT1 + BIT3);
    P6SEL |= BIT1 + BIT3;  //使P6.1-P6.4为AD模式输入口
    
    P7OUT &= ~(BIT5 + BIT6);
    P7DIR &= ~(BIT5 + BIT6);
    P7SEL |= BIT5 + BIT6;
    
}


/*
unsigned int getPAdc(void)     //获得压力采样值,未经过小数运算
{
    unsigned int re = 0;
    
    enterCritical();     //进入临界区
    
    re = presureSum / PRE_BUF_SIZE;     //获得压力采样平均值
    
    exitCritical();     //退出临界区
    
    return re;
    
}
*/

unsigned long getIAdc(void)   //获得电流采样值，多个值的总和，未经小数运算
{
    unsigned long re = 0;
    
    enterCritical();    //进入临界区
    
    re = 0;   //获得电流采样值
    
    exitCritical();   //退出临界区
    
    return re;
}





unsigned int getBattery1(void)  //获得battery1采样值函数
{
    unsigned int re;
    if(otherComp)
    {
        re = (unsigned int)(battery1Sum / SAMP_BUF_SIZE);
    }else
    {
        return battery1;
    }
    return re;
}

unsigned int getPress(void)  //获得battery1采样值函数
{
    unsigned int re;
	
	enterCritical();     //进入临界区
    if(preComp)
    {
        re = (unsigned int)(presureSum / PRE_BUF_SIZE);
    }else
    {
        return press_AD;
    }
	exitCritical();     //退出临界区
    return re;
}


unsigned int getPumpCur(void)  //获得battery1采样值函数
{
    unsigned int re;
    if(otherComp)
    {
        re = (unsigned int)(pumpCurSum / SAMP_BUF_SIZE);
    }else
    {
        return pump_I;
    }
    return re;
}
unsigned int getPumpVolt(void)  //获得battery1采样值函数
{
    unsigned int re;
    if(otherComp)
    {
        re = (unsigned int)(pumpVoltSum / SAMP_BUF_SIZE);
    }else
    {
        return pump_V;
    }
    return re;
}
/*

unsigned int getDQ(void)         //获得DQ采样值函数
{
    unsigned int re;
    if(otherComp)
    {
        re = (unsigned int)(DQSum / SAMP_BUF_SIZE);
    }else
    {
        return DQ;
    }
    
    return re;
}

unsigned int getBattery2(void)   //获得battery2采样值函数
{
    unsigned int re;
    if(otherComp)
    {
        re = (unsigned int)(battery2Sum / SAMP_BUF_SIZE);
    }else
    {
        return battery2;
    }
    re = 100;
    return re;
}


unsigned int getTemp(void)       //获得tem采样值函数
{
    unsigned int re;
    if(otherComp)
    {
        re = (unsigned int)(tempSum / SAMP_BUF_SIZE);
    }else
    {
        return temp;
    }
    
    return re;
    
}


unsigned int getChargeCur(void)  //获得充电电流采样值函数
{
    unsigned int re;
    if(otherComp)
    {
        re = (unsigned int)(chargeCurSum / SAMP_BUF_SIZE);
    }else
    {
        return chargeCurrent;
    }
    
   
    return re;
}

*/


void adcTask(void)    //adc采样任 
{
    unsigned int adResult[4] = {0,0,0,0};
    long timeOut = 0;
    int i = 0;
    int j = 0;
    
    
    initBuf();    //初始化采样数据缓冲区
    initAdc();    //初始化ADC模块
    
    initSema(&adc12Sem,0);
    
    while(1)
    {
        
        if(adcSmpFlag)  //AD采样是否启用
        {
            enterCritical();  //进入临界区
            startAdc();    //启动AD采样
            
            timeOut = 0;
            
            while(!checkCmpled() && timeOut < 500000) //如果超时，则同样退出本循环
            {                                         //以防止程序在此占用CPU
                timeOut ++;
            }
            exitCritical();                //退出临界区
            
            if(timeOut < 10000)   //如果采样时间正常，则保存采样数据
            {
                adResult[0] = ADC12MEM0; //battery1
                adResult[1] = ADC12MEM1; //presure
                adResult[2] = ADC12MEM2; //DQ
                adResult[3] = ADC12MEM3; //battery2
                
                if(otherComp == 0)
                {
                    enterCritical();   //进入临界区
                    smpData[i].battery1 = adResult[0];
                    battery1Sum += adResult[0];
                    battery1 = adResult[0];                    
                    
                    smpData[i].pump_I = adResult[2];
                    pumpCurSum += adResult[2];
                    pump_I = adResult[2];
                    
                    smpData[i].pump_V     = adResult[3];
                    pumpVoltSum += adResult[3];
                    pump_V = adResult[3];
                    
                    exitCritical();  //退出临界区
                    
                    i++;
                    if(i >= SAMP_BUF_SIZE)
                    {
                        i = 0;
                        otherComp = 1; //第一次填充满缓冲区完成
                        upSema(&adc12Sem);
                    }
                }else
                {
                    enterCritical();   //进入临界区
                    battery1Sum -= smpData[i].battery1;
                    smpData[i].battery1 = adResult[0];
                    battery1Sum += adResult[0];                  
                    
                    pumpCurSum -= smpData[i].pump_I;
                    smpData[i].pump_I = adResult[2];
                    pumpCurSum += adResult[2];
                    
                    pumpVoltSum -= smpData[i].pump_V;
                    smpData[i].pump_V     = adResult[3];
                    pumpVoltSum += adResult[3];

                    exitCritical();    //退出临界区
                    
                    i++;
                    if(i >= SAMP_BUF_SIZE)
                    {
                        i = 0;
                    }
                }
               
                if(preComp == 0) //压力缓冲未被充满
                {
                    enterCritical();
                    presureSum += adResult[1];
                    preBuf[j] = adResult[1];
                    exitCritical();
                    
                    j++;
                    if(j >= PRE_BUF_SIZE)
                    {
                        j = 0;
                        preComp = 1;
                    }
                    
                }else  //压力传感器已被充满
                {
                    enterCritical();
                    presureSum -= preBuf[j];
                    preBuf[j] = adResult[1];
                    presureSum += adResult[1];
                    exitCritical();
                    
                    j++;
                    if(j >= PRE_BUF_SIZE)
                    {
                        j = 0;
                    }
                }
            }else         //如果此项出错，则必须弹窗警示
            {
                          //代码需在后续添加
            }
        }  
            
       delayTick(SAMP_T);     //1ms毫秒执行一次 
        
       WDTCTL = WDTPW + WDTCNTCL;  //喂狗
       P6OUT ^= BIT0;   
        
    }
     
    
               //外看门狗
    
}

int waitAdc12(void)
{
  downSema(&adc12Sem);
  
  return 1;
}