

#include "flash.h"
#include "MSP430x54x.h"    //������Ƭ����ͷ�ļ�


static unsigned int flashBuf[64];    //flashдʱ����





unsigned int readFlash(unsigned int *addr)   //�ӵ�ַaddr�����һ������
{
    unsigned int *addrTemp;
    
    unsigned int re = 0;

    addrTemp = (unsigned int*)addr;  //��õ�ַ
    
    re = *addrTemp;  //ֱ�Ӷ�ȡ����
    
    return re;       //����д��ֵ
}

int writeFlash(unsigned int data,unsigned long addr)   //��Flash��д���ݺ���
{
    int i,re = 0;
    
    unsigned int *addrTmp;
    
    if(addr >= 0x1800 && addr <= 0x187E)  //Ҫд��������D��
    {
        addrTmp = (unsigned int*)0x1800;
        
        for(i = 0;i < 64;i++)  //�Ƚ����ݶ�����
        {
            flashBuf[i] = *(addrTmp+i); //���ݱ�һ���������������ݻ�����
        }
        
        i = (addr - 0x1800) >> 1;   //��õ�ַ��ƫ����
        
        addrTmp = (unsigned int*)0x1800;
        
        flashBuf[i] = data;   //����д�뵽��������
        
        while(FCTL3 & BUSY) //�ж�Flash�Ƿ�æ����æ�������һ��
        {
        }
        
        FCTL3 = FWKEY;          //��LOCK
        FCTL1 = FWKEY + ERASE; //�Ըöν��в���������ʼ�� 
            
        *addrTmp = 0;   //��������
        
        while(FCTL3 & BUSY) //�ж�Flash�Ƿ�æ����æ�������һ��
        {
        }
        
        FCTL3 = FWKEY + LOCK;
        
        addrTmp = (unsigned int*)0x1800;
        
        for(i = 0;i < 64;i++)
        {
            while(FCTL3 & BUSY)   //���Flash�Ƿ�æµ
            {
            }
            
            FCTL3 = FWKEY;          //��LOCK
            FCTL1 = FWKEY + WRT; //�Ըöν���д������ʼ��
                
            *(addrTmp + i) = flashBuf[i];  //д��Flash
            
            while(FCTL3 & BUSY)   //���Flash�Ƿ�æµ
            {
            }
            
            FCTL1 = FWKEY;   //�������־
            FCTL3 = FWKEY + LOCK;  //LOCK
        }
        
        re = 1;
        
    }else if(addr >= 0x1880 && addr <= 0x18FE)  //Ҫд��������C��
    {
        addrTmp = (unsigned int*)0x1880;
        
        for(i = 0;i < 64;i++)  //�Ƚ����ݶ�����
        {
            flashBuf[i] = *(addrTmp+i); //���ݱ�һ���������������ݻ�����
        }
        
        i = (addr - 0x1880) >> 1;   //��õ�ַ��ƫ����
        
        addrTmp = (unsigned int*)0x1880;
        
        flashBuf[i] = data;   //����д�뵽��������
        
        while(FCTL3 & BUSY) //�ж�Flash�Ƿ�æ����æ�������һ��
        {
        }
        
        FCTL3 = FWKEY;          //��LOCK
        FCTL1 = FWKEY + ERASE; //�Ըöν��в���������ʼ�� 
            
        *addrTmp = 0;   //��������
        
        while(FCTL3 & BUSY) //�ж�Flash�Ƿ�æ����æ�������һ��
        {
        }
        
        FCTL3 = FWKEY + LOCK;
        
        
        for(i = 0;i < 64;i++)
        {
            while(FCTL3 & BUSY)   //���Flash�Ƿ�æµ
            {
            }
            
            FCTL3 = FWKEY;          //��LOCK
            FCTL1 = FWKEY + WRT; //�Ըöν���д������ʼ��
                
            *(addrTmp + i) = flashBuf[i];  //д��Flash
            
            while(FCTL3 & BUSY)   //���Flash�Ƿ�æµ
            {
            }
            
            FCTL1 = FWKEY;   //�������־
            FCTL3 = FWKEY + LOCK;  //LOCK
        }
        
        re = 1;
        
    }else if(addr >= 0x1900 && addr <= 0x197e)  //Ҫд��������B��
    {
        addrTmp = (unsigned int*)0x1900;
        
        for(i = 0;i < 64;i++)  //�Ƚ����ݶ�����
        {
            flashBuf[i] = *(addrTmp+i); //���ݱ�һ���������������ݻ�����
        }
        
        i = (addr - 0x1900) >> 1;   //��õ�ַ��ƫ����
        
        addrTmp = (unsigned int*)0x1900;
        
        flashBuf[i] = data;   //����д�뵽��������
        
        while(FCTL3 & BUSY) //�ж�Flash�Ƿ�æ����æ�������һ��
        {
        }
        
        FCTL3 = FWKEY;          //��LOCK
        FCTL1 = FWKEY + ERASE; //�Ըöν��в���������ʼ�� 
            
        *addrTmp = 0;   //��������
        
        while(FCTL3 & BUSY) //�ж�Flash�Ƿ�æ����æ�������һ��
        {
        }
        
        FCTL3 = FWKEY + LOCK;
        
        
        for(i = 0;i < 64;i++)
        {
            while(FCTL3 & BUSY)   //���Flash�Ƿ�æµ
            {
            }
            
            FCTL3 = FWKEY;          //��LOCK
            FCTL1 = FWKEY + WRT; //�Ըöν���д������ʼ��
                
            *(addrTmp + i) = flashBuf[i];  //д��Flash
            
            while(FCTL3 & BUSY)   //���Flash�Ƿ�æµ
            {
            }
            
            FCTL1 = FWKEY;   //�������־
            FCTL3 = FWKEY + LOCK;  //LOCK
        }
        
        re = 1;
        
    }else if(addr >= 0x1980 && addr <= 0x19fe)  //Ҫд��������A��
    {
        addrTmp = (unsigned int*)0x1980;
        
        for(i = 0;i < 64;i++)  //�Ƚ����ݶ�����
        {
            flashBuf[i] = *(addrTmp+i); //���ݱ�һ���������������ݻ�����
        } 
        
        i = (addr - 0x1980) >> 1;   //��õ�ַ��ƫ����
        
        addrTmp = (unsigned int*)0x1980;
        
        flashBuf[i] = data;   //����д�뵽��������
        
        while(FCTL3 & BUSY) //�ж�Flash�Ƿ�æ����æ�������һ��
        {
        }
        
        FCTL3 = FWKEY;          //��LOCK
        FCTL1 = FWKEY + ERASE; //�Ըöν��в���������ʼ�� 
            
        *addrTmp = 0;   //��������
        
        while(FCTL3 & BUSY) //�ж�Flash�Ƿ�æ����æ�������һ��
        {
        }
        
        FCTL3 = FWKEY + LOCK;
        
        
        for(i = 0;i < 64;i++)
        {
            while(FCTL3 & BUSY)   //���Flash�Ƿ�æµ
            {
            }
            
            FCTL3 = FWKEY;          //��LOCK
            FCTL1 = FWKEY + WRT; //�Ըöν���д������ʼ��
                
            *(addrTmp + i) = flashBuf[i];  //д��Flash
            
            while(FCTL3 & BUSY)   //���Flash�Ƿ�æµ
            {
            }
            
            FCTL1 = FWKEY;   //�������־
            FCTL3 = FWKEY + LOCK;  //LOCK
        }
        
        re = 1;
    }
    
    return re;  //����ֵ
}
