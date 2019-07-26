#include "ds18b20.h"
#include "delay.h"

float temperature1;
float temperature2; 
int main()
{
	
	u8 t = 0;
	


	delay_init();
	DS18B20_Init();
	DS18B20B_Init();	
	while(1)
	{	    	    
 		if(t%10==0)			//每100ms读取一次
		{									  
			temperature1 = DS18B20_GetTemp_SkipRom();
			temperature2 = DS18B20B_GetTemp_SkipRom();			
		}				   
	 	delay_ms(10);
		t++;
	}
}
