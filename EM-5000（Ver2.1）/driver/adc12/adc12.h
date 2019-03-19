

#ifndef _ADC12_H_

#define _ADC12_H_

#define BUF_NUM  800       //����һǧ������


#define SAMP_BUF_SIZE   50
#define PRE_BUF_SIZE    700




struct SMP_DATA
{
    unsigned int battery1;
   /* unsigned int DQ;
    unsigned int battery2;
    unsigned int temp;
    unsigned int chargeCurrent;*/
	//unsigned int press_AD;			//�ò���ADֵ
	unsigned int pump_I;				//�����������
	unsigned int pump_V;				//�����ѹ����
	
};



#define startAdc()  (ADC12CTL0 |= ADC12SC + ADC12ENC)    //����������

#define checkCmpled()  (ADC12CTL1 & ADC12BUSY)   //���Բ����Ƿ���ɺ�

void set_SAMP_T(unsigned int tt);
unsigned int get_SAMP_T(void);
int getBatLevel(void);  //��õ�ص�������


void adcTask(void);    //adc��������

int waitAdc12(void);

void initBuf(void);    //��ʼ�����ݻ�������������ÿ����������ʱ����

void initAdc(void);    // ��ʼ��AD��������


unsigned int getPress(void); //���ѹ������ֵ,δ����С������

unsigned long getIAdc(void); //��õ�������ֵ�����ֵ���ܺͣ�δ��С������



unsigned int getBattery1(void);  //���battery1����ֵ����
unsigned int getDQ(void);        //���DQ����ֵ����
unsigned int getBattery2(void);  //���battery2����ֵ����
unsigned int getTemp(void);      //���tem����ֵ����
unsigned int getChargeCur(void); //��ó���������ֵ����




#endif