

#ifndef _ADC12_H_

#define _ADC12_H_

#define BUF_NUM  800       //采样一千个数据


#define SAMP_BUF_SIZE   50
#define PRE_BUF_SIZE    700




struct SMP_DATA
{
    unsigned int battery1;
   /* unsigned int DQ;
    unsigned int battery2;
    unsigned int temp;
    unsigned int chargeCurrent;*/
	//unsigned int press_AD;			//泵采样AD值
	unsigned int pump_I;				//电机电流采样
	unsigned int pump_V;				//电机电压采样
	
};



#define startAdc()  (ADC12CTL0 |= ADC12SC + ADC12ENC)    //启动采样宏

#define checkCmpled()  (ADC12CTL1 & ADC12BUSY)   //测试采样是否完成宏

void set_SAMP_T(unsigned int tt);
unsigned int get_SAMP_T(void);
int getBatLevel(void);  //获得电池电量级别


void adcTask(void);    //adc采样任务

int waitAdc12(void);

void initBuf(void);    //初始化数据缓冲区函数，在每次启动采样时调用

void initAdc(void);    // 初始化AD采样函数


unsigned int getPress(void); //获得压力采样值,未经过小数运算

unsigned long getIAdc(void); //获得电流采样值，多个值的总和，未经小数运算



unsigned int getBattery1(void);  //获得battery1采样值函数
unsigned int getDQ(void);        //获得DQ采样值函数
unsigned int getBattery2(void);  //获得battery2采样值函数
unsigned int getTemp(void);      //获得tem采样值函数
unsigned int getChargeCur(void); //获得充电电流采样值函数




#endif