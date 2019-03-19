

#include "flash.h"
#include "MSP430x54x.h"    //包含单片机的头文件


static unsigned int flashBuf[64];    //flash写时缓冲





unsigned int readFlash(unsigned int *addr)   //从地址addr里读入一个数据
{
    unsigned int *addrTemp;
    
    unsigned int re = 0;

    addrTemp = (unsigned int*)addr;  //获得地址
    
    re = *addrTemp;  //直接读取数据
    
    return re;       //返回写入值
}

int writeFlash(unsigned int data,unsigned long addr)   //往Flash里写数据函数
{
    int i,re = 0;
    
    unsigned int *addrTmp;
    
    if(addr >= 0x1800 && addr <= 0x187E)  //要写的数据在D段
    {
        addrTmp = (unsigned int*)0x1800;
        
        for(i = 0;i < 64;i++)  //先将数据读出来
        {
            flashBuf[i] = *(addrTmp+i); //数据被一个个读出来到数据缓冲区
        }
        
        i = (addr - 0x1800) >> 1;   //获得地址的偏移量
        
        addrTmp = (unsigned int*)0x1800;
        
        flashBuf[i] = data;   //数据写入到缓冲区中
        
        while(FCTL3 & BUSY) //判断Flash是否忙，不忙则继续下一步
        {
        }
        
        FCTL3 = FWKEY;          //清LOCK
        FCTL1 = FWKEY + ERASE; //对该段进行擦除操作初始化 
            
        *addrTmp = 0;   //擦除操作
        
        while(FCTL3 & BUSY) //判断Flash是否忙，不忙则继续下一步
        {
        }
        
        FCTL3 = FWKEY + LOCK;
        
        addrTmp = (unsigned int*)0x1800;
        
        for(i = 0;i < 64;i++)
        {
            while(FCTL3 & BUSY)   //检查Flash是否忙碌
            {
            }
            
            FCTL3 = FWKEY;          //清LOCK
            FCTL1 = FWKEY + WRT; //对该段进行写操作初始化
                
            *(addrTmp + i) = flashBuf[i];  //写入Flash
            
            while(FCTL3 & BUSY)   //检查Flash是否忙碌
            {
            }
            
            FCTL1 = FWKEY;   //清除读标志
            FCTL3 = FWKEY + LOCK;  //LOCK
        }
        
        re = 1;
        
    }else if(addr >= 0x1880 && addr <= 0x18FE)  //要写的数据在C段
    {
        addrTmp = (unsigned int*)0x1880;
        
        for(i = 0;i < 64;i++)  //先将数据读出来
        {
            flashBuf[i] = *(addrTmp+i); //数据被一个个读出来到数据缓冲区
        }
        
        i = (addr - 0x1880) >> 1;   //获得地址的偏移量
        
        addrTmp = (unsigned int*)0x1880;
        
        flashBuf[i] = data;   //数据写入到缓冲区中
        
        while(FCTL3 & BUSY) //判断Flash是否忙，不忙则继续下一步
        {
        }
        
        FCTL3 = FWKEY;          //清LOCK
        FCTL1 = FWKEY + ERASE; //对该段进行擦除操作初始化 
            
        *addrTmp = 0;   //擦除操作
        
        while(FCTL3 & BUSY) //判断Flash是否忙，不忙则继续下一步
        {
        }
        
        FCTL3 = FWKEY + LOCK;
        
        
        for(i = 0;i < 64;i++)
        {
            while(FCTL3 & BUSY)   //检查Flash是否忙碌
            {
            }
            
            FCTL3 = FWKEY;          //清LOCK
            FCTL1 = FWKEY + WRT; //对该段进行写操作初始化
                
            *(addrTmp + i) = flashBuf[i];  //写入Flash
            
            while(FCTL3 & BUSY)   //检查Flash是否忙碌
            {
            }
            
            FCTL1 = FWKEY;   //清除读标志
            FCTL3 = FWKEY + LOCK;  //LOCK
        }
        
        re = 1;
        
    }else if(addr >= 0x1900 && addr <= 0x197e)  //要写的数据在B段
    {
        addrTmp = (unsigned int*)0x1900;
        
        for(i = 0;i < 64;i++)  //先将数据读出来
        {
            flashBuf[i] = *(addrTmp+i); //数据被一个个读出来到数据缓冲区
        }
        
        i = (addr - 0x1900) >> 1;   //获得地址的偏移量
        
        addrTmp = (unsigned int*)0x1900;
        
        flashBuf[i] = data;   //数据写入到缓冲区中
        
        while(FCTL3 & BUSY) //判断Flash是否忙，不忙则继续下一步
        {
        }
        
        FCTL3 = FWKEY;          //清LOCK
        FCTL1 = FWKEY + ERASE; //对该段进行擦除操作初始化 
            
        *addrTmp = 0;   //擦除操作
        
        while(FCTL3 & BUSY) //判断Flash是否忙，不忙则继续下一步
        {
        }
        
        FCTL3 = FWKEY + LOCK;
        
        
        for(i = 0;i < 64;i++)
        {
            while(FCTL3 & BUSY)   //检查Flash是否忙碌
            {
            }
            
            FCTL3 = FWKEY;          //清LOCK
            FCTL1 = FWKEY + WRT; //对该段进行写操作初始化
                
            *(addrTmp + i) = flashBuf[i];  //写入Flash
            
            while(FCTL3 & BUSY)   //检查Flash是否忙碌
            {
            }
            
            FCTL1 = FWKEY;   //清除读标志
            FCTL3 = FWKEY + LOCK;  //LOCK
        }
        
        re = 1;
        
    }else if(addr >= 0x1980 && addr <= 0x19fe)  //要写的数据在A段
    {
        addrTmp = (unsigned int*)0x1980;
        
        for(i = 0;i < 64;i++)  //先将数据读出来
        {
            flashBuf[i] = *(addrTmp+i); //数据被一个个读出来到数据缓冲区
        } 
        
        i = (addr - 0x1980) >> 1;   //获得地址的偏移量
        
        addrTmp = (unsigned int*)0x1980;
        
        flashBuf[i] = data;   //数据写入到缓冲区中
        
        while(FCTL3 & BUSY) //判断Flash是否忙，不忙则继续下一步
        {
        }
        
        FCTL3 = FWKEY;          //清LOCK
        FCTL1 = FWKEY + ERASE; //对该段进行擦除操作初始化 
            
        *addrTmp = 0;   //擦除操作
        
        while(FCTL3 & BUSY) //判断Flash是否忙，不忙则继续下一步
        {
        }
        
        FCTL3 = FWKEY + LOCK;
        
        
        for(i = 0;i < 64;i++)
        {
            while(FCTL3 & BUSY)   //检查Flash是否忙碌
            {
            }
            
            FCTL3 = FWKEY;          //清LOCK
            FCTL1 = FWKEY + WRT; //对该段进行写操作初始化
                
            *(addrTmp + i) = flashBuf[i];  //写入Flash
            
            while(FCTL3 & BUSY)   //检查Flash是否忙碌
            {
            }
            
            FCTL1 = FWKEY;   //清除读标志
            FCTL3 = FWKEY + LOCK;  //LOCK
        }
        
        re = 1;
    }
    
    return re;  //返回值
}
