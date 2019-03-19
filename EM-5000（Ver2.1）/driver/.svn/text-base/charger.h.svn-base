
#ifndef _CHARGER_H_
#define _CHARGER_H_

#define ON_NOT_OUTPOWER     1     //只开机，但无外接电源
#define OFF_AND_OUTPOWER    2     //关机，但亦外接电源
#define ON_AND_OUTPOWER     3     //开机并且有外接电源


#define LED_COUNT    30     //LED灯闪烁计数最大值 

#define startCharge()  TB0CCTL2 |= OUTMOD_7
#define stopCharge()  TB0CCTL2 &= ~OUTMOD_7
#define setTb0ccr2(x)  TB0CCR2=((x))
#define KG()  P6IN & BIT6

#define ledOn()     do{P6DIR |= BIT4; P6OUT |= BIT4;}while(0)    //LED灯亮
#define ledOff()    do{P6DIR |= BIT4; P6OUT &= ~BIT4;}while(0)   //LED灯灭



int isCharge(void);          //检测是否在充电

int isPowStateChange(void);  //电源状态是否改变

int getPowState(void);    //获取当前胡电源所处状态


void charge(void);         //充电函数

int getBatLev(void);       //获得电量级数

#endif