


#include "adc12.h"
#include "port.h"
#include "tick.h"
#include "semaphore.h"

#include "MSP430x54x.h"

#include "lcd.h"


static struct SEMAPHORE adc12Sem;

__no_init static unsigned int preBuf[PRE_BUF_SIZE];   //ѹ�����ݻ���
__no_init static struct SMP_DATA smpData[SAMP_BUF_SIZE];
static unsigned int SAMP_T=6;

static unsigned char adcSmpFlag = 1;    //��ʾ����AD����

static int otherComp = 0;     //��ѹ�����ݻ�����������ݻ�������һ�γ�����־����
static int preComp = 0;       //ѹ�����ݻ�������һ�γ����ꡢ��
static unsigned long battery1Sum = 0; 	//
static unsigned long presureSum = 0;	//�ò���ֵ
static unsigned long pumpCurSum = 0; 	//
static unsigned long pumpVoltSum = 0;	//

//static unsigned long DQSum = 0;
//static unsigned long battery2Sum = 0;
//static unsigned long tempSum = 0;
//static unsigned long chargeCurSum = 0;


static unsigned int battery1 = 0;			//��ص���
static unsigned int press_AD = 0;			//�ò���ADֵ
static unsigned int pump_I = 0;				//�����������
static unsigned int pump_V = 0;				//�����ѹ����

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


void initAdc(void)   // ��ʼ��AD��������
{
    ADC12CTL0 |= ADC12MSC +  //���в�������ģʽ 
                 ADC12ON + ADC12REFON;    //AD���Ŀ���
    
    ADC12CTL1 |= ADC12SHP +      //ʹ�ô�������ʱ��ģʽ����
                 ADC12SSEL_2 +   //ʹ��ACLK��ΪAD����ʱ��
                 ADC12CONSEQ_1;  //ʹ�õ��ζ���ģʽ����
    ADC12CTL2 |= ADC12PDIV + ADC12RES_2;
    
    
    ADC12MCTL0 |= ADC12INCH_1;  //ͨ��1,�豸����״̬�ź�
    
    ADC12MCTL1 |= ADC12INCH_3;  //ͨ��3,ѹ�������������ź�
    
    ADC12MCTL2 |= ADC12INCH_13;  //ͨ��5���¶ȴ����������ź�
    
    ADC12MCTL3 |= ADC12INCH_14 + //ͨ��4,�����������ź�
                  ADC12EOS;      //����Ϊ���һ��������ͨ��
    
    P6OUT &= ~(BIT1 + BIT3);
    P6DIR &= ~(BIT1 + BIT3);
    P6SEL |= BIT1 + BIT3;  //ʹP6.1-P6.4ΪADģʽ�����
    
    P7OUT &= ~(BIT5 + BIT6);
    P7DIR &= ~(BIT5 + BIT6);
    P7SEL |= BIT5 + BIT6;
    
}


/*
unsigned int getPAdc(void)     //���ѹ������ֵ,δ����С������
{
    unsigned int re = 0;
    
    enterCritical();     //�����ٽ���
    
    re = presureSum / PRE_BUF_SIZE;     //���ѹ������ƽ��ֵ
    
    exitCritical();     //�˳��ٽ���
    
    return re;
    
}
*/

unsigned long getIAdc(void)   //��õ�������ֵ�����ֵ���ܺͣ�δ��С������
{
    unsigned long re = 0;
    
    enterCritical();    //�����ٽ���
    
    re = 0;   //��õ�������ֵ
    
    exitCritical();   //�˳��ٽ���
    
    return re;
}





unsigned int getBattery1(void)  //���battery1����ֵ����
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

unsigned int getPress(void)  //���battery1����ֵ����
{
    unsigned int re;
	
	enterCritical();     //�����ٽ���
    if(preComp)
    {
        re = (unsigned int)(presureSum / PRE_BUF_SIZE);
    }else
    {
        return press_AD;
    }
	exitCritical();     //�˳��ٽ���
    return re;
}


unsigned int getPumpCur(void)  //���battery1����ֵ����
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
unsigned int getPumpVolt(void)  //���battery1����ֵ����
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

unsigned int getDQ(void)         //���DQ����ֵ����
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

unsigned int getBattery2(void)   //���battery2����ֵ����
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


unsigned int getTemp(void)       //���tem����ֵ����
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


unsigned int getChargeCur(void)  //��ó���������ֵ����
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


void adcTask(void)    //adc������ 
{
    unsigned int adResult[4] = {0,0,0,0};
    long timeOut = 0;
    int i = 0;
    int j = 0;
    
    
    initBuf();    //��ʼ���������ݻ�����
    initAdc();    //��ʼ��ADCģ��
    
    initSema(&adc12Sem,0);
    
    while(1)
    {
        
        if(adcSmpFlag)  //AD�����Ƿ�����
        {
            enterCritical();  //�����ٽ���
            startAdc();    //����AD����
            
            timeOut = 0;
            
            while(!checkCmpled() && timeOut < 500000) //�����ʱ����ͬ���˳���ѭ��
            {                                         //�Է�ֹ�����ڴ�ռ��CPU
                timeOut ++;
            }
            exitCritical();                //�˳��ٽ���
            
            if(timeOut < 10000)   //�������ʱ���������򱣴��������
            {
                adResult[0] = ADC12MEM0; //battery1
                adResult[1] = ADC12MEM1; //presure
                adResult[2] = ADC12MEM2; //DQ
                adResult[3] = ADC12MEM3; //battery2
                
                if(otherComp == 0)
                {
                    enterCritical();   //�����ٽ���
                    smpData[i].battery1 = adResult[0];
                    battery1Sum += adResult[0];
                    battery1 = adResult[0];                    
                    
                    smpData[i].pump_I = adResult[2];
                    pumpCurSum += adResult[2];
                    pump_I = adResult[2];
                    
                    smpData[i].pump_V     = adResult[3];
                    pumpVoltSum += adResult[3];
                    pump_V = adResult[3];
                    
                    exitCritical();  //�˳��ٽ���
                    
                    i++;
                    if(i >= SAMP_BUF_SIZE)
                    {
                        i = 0;
                        otherComp = 1; //��һ����������������
                        upSema(&adc12Sem);
                    }
                }else
                {
                    enterCritical();   //�����ٽ���
                    battery1Sum -= smpData[i].battery1;
                    smpData[i].battery1 = adResult[0];
                    battery1Sum += adResult[0];                  
                    
                    pumpCurSum -= smpData[i].pump_I;
                    smpData[i].pump_I = adResult[2];
                    pumpCurSum += adResult[2];
                    
                    pumpVoltSum -= smpData[i].pump_V;
                    smpData[i].pump_V     = adResult[3];
                    pumpVoltSum += adResult[3];

                    exitCritical();    //�˳��ٽ���
                    
                    i++;
                    if(i >= SAMP_BUF_SIZE)
                    {
                        i = 0;
                    }
                }
               
                if(preComp == 0) //ѹ������δ������
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
                    
                }else  //ѹ���������ѱ�����
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
            }else         //��������������뵯����ʾ
            {
                          //�������ں������
            }
        }  
            
       delayTick(SAMP_T);     //1ms����ִ��һ�� 
        
       WDTCTL = WDTPW + WDTCNTCL;  //ι��
       P6OUT ^= BIT0;   
        
    }
     
    
               //�⿴�Ź�
    
}

int waitAdc12(void)
{
  downSema(&adc12Sem);
  
  return 1;
}