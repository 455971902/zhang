
#include "MSP430x54x.h"


#include "task.h"
#include "tick.h"
#include "os.h"


#include "display.h"
#include "pump.h"
#include "adc12.h"
#include "key.h"
#include "bsp.h"
#include "ds18b20.h"
#include "bmp085.h"
#include "iic.h"

OS_TCB task1Tcb;
OS_TCB task2Tcb;
OS_TCB task3Tcb;
OS_TCB task4Tcb;
OS_TCB task5Tcb;
OS_TCB task6Tcb;
OS_TCB task7Tcb;
OS_TCB task8Tcb;

void guiTask(void);

int main( void )
{
  initBsp();
  init_IIC();	
  osInit();

  createTask(&task1Tcb,pumpLoop,2,32);
  createTask(&task2Tcb,adcTask,1,32);
  createTask(&task3Tcb,scanKey,4,32);
  createTask(&task4Tcb,calcuTask,5,64);    
  createTask(&task6Tcb,calcCailTask,6,64);
  
  createTask(&task7Tcb,pressTask,7,64);
  createTask(&task8Tcb,TempTask,8,32);        //检测压强温度任务 
  createTask(&task5Tcb,guiTask,3,512);
  
  osStart();
}

void guiTask(void)
{
    struct GUI *gui;

    waitAdc12();              //等待ADC12过 你程完成 
    
    WDTCTL = WDT_ARST_1000;    //开看门狗
    
    powerOn();                //目前暂为空函数.开机函数

    gui = newGui();           //新建一个界面
    
    clrKeyBuf();              //清键盘缓冲区
    while(1)
    {   
        gui->guiExec();            //执行GUI线程
        delayTick(200);              //执行完一次，休眠
    }
}