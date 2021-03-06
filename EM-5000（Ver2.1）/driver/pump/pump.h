

#ifndef _PUMP_H_

#define _PUMP_H_

#define F_P_TABLE  0x1800   //flow对压力表FLASH地址,Flash segment D 都用于存放此表值
#define F_I_TABLE  0x1900   //flow-I表Flash起始地址，Flash segment B都用于存放此值

/*小于EM-1500用
#define START_FLOW    10     //采样器Flow-Presure表的起始流量值
*/

#define START_FLOW    100     //采样器Flow-Presure表的起始流量值
#define STOP_FLOW     5000       //采样器Flow-Presure表的终止流量值

#define FLOW_SELECT		600

void set_PWM_MIN(unsigned int tt);
void set_PWM_START(unsigned int tt);


void set_default(void);
void initPumpPwm(void);         //初始化PWM波发生器函数

void setMaxFlow(unsigned int maxFlow);    //设置仪器的最大采样流量函数
void setMinFlow(unsigned int minFlow);    //设置仪器的最小采样流量函数
void setMidflow(unsigned int midFlow);    //设置间隔流量


void startPump(int flow);       //启动采样函数

void stopPump(void);            //停止采样函数

void changeFlow(int flow);      //改变流量函数




void pumpLoop(void);       //PID算法任务函数


void calcuTask(void);   //计算瞬态流量任务

unsigned int getCurFlow(void);



//下面函数用于在流量校准时使用

void changePresure(unsigned int Presure);      //改变流量函数

unsigned int getPFromFlow(unsigned int flow); //从流量获得压力值，主要是访问
                                              //Flow-Presure表获得


//void writeIToFlow(unsigned int I,unsigned int flow); //将flow对应的电流值写入到
                                                     //flow-I表里

void writePToFlow(unsigned int p,unsigned int flow); //将flow对应的presure值写入到
                                                     //Flow-Presure表
int get_smppresure(void);


int getFault(void);   //获得采样错误状态


#endif