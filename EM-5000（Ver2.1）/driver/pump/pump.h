

#ifndef _PUMP_H_

#define _PUMP_H_

#define F_P_TABLE  0x1800   //flow��ѹ����FLASH��ַ,Flash segment D �����ڴ�Ŵ˱�ֵ
#define F_I_TABLE  0x1900   //flow-I��Flash��ʼ��ַ��Flash segment B�����ڴ�Ŵ�ֵ

/*С��EM-1500��
#define START_FLOW    10     //������Flow-Presure�����ʼ����ֵ
*/

#define START_FLOW    100     //������Flow-Presure�����ʼ����ֵ
#define STOP_FLOW     5000       //������Flow-Presure�����ֹ����ֵ

#define FLOW_SELECT		600

void set_PWM_MIN(unsigned int tt);
void set_PWM_START(unsigned int tt);


void set_default(void);
void initPumpPwm(void);         //��ʼ��PWM������������

void setMaxFlow(unsigned int maxFlow);    //������������������������
void setMinFlow(unsigned int minFlow);    //������������С������������
void setMidflow(unsigned int midFlow);    //���ü������


void startPump(int flow);       //������������

void stopPump(void);            //ֹͣ��������

void changeFlow(int flow);      //�ı���������




void pumpLoop(void);       //PID�㷨������


void calcuTask(void);   //����˲̬��������

unsigned int getCurFlow(void);



//���溯������������У׼ʱʹ��

void changePresure(unsigned int Presure);      //�ı���������

unsigned int getPFromFlow(unsigned int flow); //���������ѹ��ֵ����Ҫ�Ƿ���
                                              //Flow-Presure����


//void writeIToFlow(unsigned int I,unsigned int flow); //��flow��Ӧ�ĵ���ֵд�뵽
                                                     //flow-I����

void writePToFlow(unsigned int p,unsigned int flow); //��flow��Ӧ��presureֵд�뵽
                                                     //Flow-Presure��
int get_smppresure(void);


int getFault(void);   //��ò�������״̬


#endif