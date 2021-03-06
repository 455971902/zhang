
#include "charger.h"
#include "MSP430X54X.h"

#include "adc12.h"

static int powStateChange = 0;       //
static int isChargeFlag = 0;         //是否在充电标志位
//static unsigned int tb0ccr2Val = 0;  //TB0CCR1的副本
static int batLevel = 4;             //电池电量级数


static int chargeStatus = 0;         //充电状态,0表示电压低于6.5V
                                     //1表示电压高于6.5V小于8.4V
                                     //2表示电压高于8.4V
                                     //3表示电压高于8.6V



int getBatLev(void)     //获得电量级数
{
    return batLevel;
}


int isCharge(void)           //检测是否在充电
{
    int re = 0;
    
    if(isChargeFlag == 1 && chargeStatus < 2)
    {
        re = 1;
    }
    
    return re;
}

int isPowStateChange(void)   //电源状态是否改变
{
    int re = 0;
    
    re = powStateChange;
    
    return re;
}

int getPowState(void)    //获取当前胡电源所处状态
{
    if(!KG() && getBattery1() > 100)
    {
        return ON_NOT_OUTPOWER;
    }
    
    if(KG() && getBattery1() <101)
    {
        return OFF_AND_OUTPOWER;
    }
    
    if(KG() && getBattery1() > 100)
    {
        return ON_AND_OUTPOWER;
    }
    return 0;
}


void charge(void)          //充电函数
{
    unsigned int battery1;
    
    //temp = getTemp();
    
    
    //battery2 = getBattery2();  //获得battery2的值
    battery1 = getBattery1();;
    
    if(battery1 >= 3323)
    {
        batLevel = 4;
    }else if(battery1 > 3258)
    {
        batLevel = 3;
    }else if(battery1 > 2989)
    {
        batLevel = 2;
    }else if(battery1 > 2850)
    {
        batLevel = 1;
    }else if(battery1 > 2700)//2515)
    {
        batLevel = 0;
    }
    else
    {
       batLevel=5;
    }
    
    
    //clr_rect(30,0,24,12);
   // print_int(30,0,battery1,0);

}