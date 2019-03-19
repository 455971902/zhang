//*****************************Copyright(c)************************************* 
//** ��      ˾��         AMAE 
//** ��      Ŀ��         ADS130    
//** Ӳ���汾�ţ�       
//**================================================================== 
//**------------------------------------ �ļ���Ϣ-------------------------------
//** ��  �� ��:      iic.c
//** �ļ� ����:      iicͨѶ�ӿ�
//** ��     ��:      v0.1
//** ��  �� ��:      ����
//** ���� ����:      2012.8.29
//============================================================================
//ͷ�ļ�
//=============================================================================
#include "iic.h"
#include "msp430f5438.h"
#include "tick.h"
#include "chip.h"
//============================================================================
//�궨��
//=============================================================================

#define SDA_IN    P9DIR &= ~BIT1          //P3.1����
#define SDA_OUT   P9DIR |= BIT1           //P3.1���
#define SCL_OUT   P9DIR |= BIT2           //P3.2���


#define SDA_H     P9OUT |= BIT1           //P3.1 = 1
#define SDA_L     P9OUT &=~ BIT1          //P3.1 = 0
#define SCL_H     P9OUT |= BIT2           //P3.2 = 1
#define SCL_L     P9OUT &= ~BIT2          //P3.2 = 0

#define EP_WP_OUT	P9DIR |= BIT0           //P3.1���	  
#define EP_WP_L		P9OUT &= ~BIT0           //P3.1���	

#define IIC_READ_CMD      0xa1                //������
#define IIC_WRITE_CMD     0xa0                //д����

#define SDA_DATA  (P9IN & BIT1)             //P3.1�ĵ�ƽ
//============================================================================
//��������
//=============================================================================

//==============================================================================
//�������ƣ�void init_IIC(void)
//���������void
//���������void
//����˵����iic��ʼ��
//����λ�ã�main
//��   ע��
//============================================================================
void init_IIC(void)
{

	EP_WP_OUT;
	SDA_OUT;
	SCL_OUT;
	delay(2);
	
	SDA_H;
	SCL_H;  
	EP_WP_L;
}
//==============================================================================
//�������ƣ�void iic_Start(void)
//���������void
//���������void
//����˵����iic��������
//����λ�ã�main
//��   ע��
//============================================================================
void iic_Start(void)
{
  SDA_OUT; 
  SDA_H;
  SCL_H;
  delay(10);
  SDA_L;
  delay(10);
  SCL_L;
  
}



//==============================================================================
//�������ƣ�void iic_Stop(void)
//���������void
//���������void
//����˵����iicֹͣ����
//����λ�ã�main
//��   ע��
//============================================================================
void iic_Stop(void)
{
  SDA_OUT;
  SCL_L;
  SDA_L;
  delay(10);
  SCL_H;
  delay(10);
  SDA_H;  
  delay(10);
}
//==============================================================================
//�������ƣ�void iic_Stop(void)
//���������void
//���������void
//����˵����iicֹͣ����
//����λ�ã�main
//��   ע��
//============================================================================
void iic_Ask(void)
{
  SDA_OUT;delay(10);
  
  SCL_L;delay(10);
  SDA_L;delay(10);
  SCL_H;delay(10);
  SCL_L; delay(10);
}

//==============================================================================
//�������ƣ�void iic_Stop(void)
//���������void
//���������void
//����˵����iicֹͣ����
//����λ�ã�main
//��   ע��
//============================================================================
void iic_NoAsk(void)
{
  SDA_OUT;delay(10);
  SCL_L;delay(10);
  SDA_H;delay(10);
  SCL_H;delay(10);
  SCL_L; delay(10);
}

//==============================================================================
//�������ƣ�int iic_GetAsk(void)
//���������void
//���������
//����˵����
//����λ�ã�
//��   ע��
//============================================================================
unsigned char iic_GetAsk(void)
{
  unsigned char temp = 0;

  SCL_L;
  SDA_H;
  SCL_H;
  SDA_IN; 
  delay(10);  
  
  temp = SDA_DATA;
  
  SCL_L;  
  
  return(temp & BIT1);
}

//==============================================================================
//�������ƣ�void iic_SendByte(unsigned int data)
//���������void
//���������void
//����˵����iic��������
//����λ�ã�main
//��   ע����λ�ȷ�
//============================================================================
void iic_SendByte(unsigned char data)
{
    int i = 7;
	
    SDA_OUT; 	    
    
    for(i = 0; i < 8; i++)
    {   
        SCL_L;
        if(data & 0x80)
        {
          SDA_H;          
        }
        else
        {
          SDA_L;
        }
        delay(10);
        SCL_H;      
        data <<= 1;
    }    
    SCL_L;  
    delay(20);
}
//==============================================================================
//�������ƣ�void iic_SendByte(unsigned int data)
//���������void
//���������void
//����˵����iic��������
//����λ�ã�main
//��   ע��
//============================================================================
 unsigned char  iic_RcvByte(void)
{
  int i = 0;
  unsigned char temp = 0;  
  
  SDA_IN;
  delay(10);
  
  for(i = 0; i < 8; i++)
  {
      temp <<= 1;
      SCL_H;      
      delay(2);
      if(SDA_DATA)
      {
        temp |= 1;      
      }
      SCL_L;
     delay(20);
   }    
   return temp;   
}


//==============================================================================
//�������ƣ�int writeByte(int addr, char data)
//���������addr����ַ data��д������
//���������0��дʧ��  ��0��д�ɹ�
//����˵����д�����ֽ�����
//����λ�ã�main
//��   ע��
//============================================================================
int writeByte(int addr,unsigned char data)
{
  unsigned char temp = 0;
  
  iic_Start();
  
  iic_SendByte(IIC_WRITE_CMD);               //����д������
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr >> 8);
  iic_SendByte(temp);          //���͸ߵ�ַ  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr);
  iic_SendByte(temp);        //���͵͵�ַ    
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  iic_SendByte(data);             //��������  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  iic_Stop();
  delay(200);
  return 1;
}

//==============================================================================
//�������ƣ�int writeByte(int addr, char data)
//���������addr����ַ data��д������
//���������0��дʧ��  ��0��д�ɹ�
//����˵����д����������
//����λ�ã�main
//��   ע��
//============================================================================
int writeWord(int addr,unsigned int data)
{
  unsigned char temp = 0;
  
  iic_Start();
  
  iic_SendByte(IIC_WRITE_CMD);               //����д������
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr >> 8);
  iic_SendByte(temp);          //���͸ߵ�ַ  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr);
  iic_SendByte(temp);        //���͵͵�ַ    
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  temp = data >> 8;               //���ݸ�λ���
  iic_SendByte(temp);             //��������  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = data;  
  iic_SendByte(temp);             //��������  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  iic_Stop();
  delay(500);
  return 1;
}
//==============================================================================
//�������ƣ�int writeByte(int addr, char data)
//���������addr����ַ data��д������
//���������0��дʧ��  ��0��д�ɹ�
//����˵����д����������
//����λ�ã�main
//��   ע��
//============================================================================
int writeDWord(int addr,unsigned long data)
{
  unsigned char temp = 0;
  
  iic_Start();
  
  iic_SendByte(IIC_WRITE_CMD);               //����д������
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr >> 8);
  iic_SendByte(temp);          //���͸ߵ�ַ  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr);
  iic_SendByte(temp);        //���͵͵�ַ    
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  temp = data >> 24;               //���ݸ�λ���
  iic_SendByte(temp);             //��������  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = data >> 16;  
  iic_SendByte(temp);             //��������  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = data >> 8;               //���ݸ�λ���
  iic_SendByte(temp);             //��������  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = data;  
  iic_SendByte(temp);             //��������  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  iic_Stop();
  delay(500);
  return 1;
}
//==============================================================================
//�������ƣ�int writeByte(int addr, char data)
//���������addr����ַ data��д������
//���������0��дʧ��  ��0��д�ɹ�
//����˵����д�����ֽ�����
//����λ�ã�main
//��   ע��
//============================================================================
int writeArrayData(int addr,unsigned char *data,int length)
{
  unsigned char temp = 0;
  int i = 0;
  iic_Start();  
  iic_SendByte(IIC_WRITE_CMD);           //����д������
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr >> 8);
  iic_SendByte(temp);      //���͸ߵ�ַ  
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr);
  iic_SendByte(temp);  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }

   for(i = 0; i < length; i++)
   {
      iic_SendByte(*(data++));             //��������     
      temp = iic_GetAsk();  
      if(temp & BIT1)
      {
        return 0;
      }
   }
  iic_Stop();
  return 1;
}

//==============================================================================
//�������ƣ�int readByte(int addr, char *data)
//���������addr :��ȡ�ĵ�ַ  *data ��ȡ���ݴ�ŵ�ַ
//���������0��ʧ�� 1���ɹ�
//����˵������ȡ�����ֽ�����
//����λ�ã�main
//��   ע����д�ӻ���ַ 
//============================================================================
int readByte(int addr,unsigned  char *data)
{
  unsigned char temp = 0;

  iic_Start();  
  iic_SendByte(IIC_WRITE_CMD);           //����д������
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr >> 8);
  iic_SendByte(temp);      //���͸ߵ�ַ  
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr);
  iic_SendByte(temp);  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  
  iic_Start();
  iic_SendByte(IIC_READ_CMD);           //���ͽ���������  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }

  *data = (unsigned char)iic_RcvByte();
  iic_Stop();

  return 1;
}

//==============================================================================
//�������ƣ�int readByte(int addr, char *data)
//���������addr :��ȡ�ĵ�ַ  *data ��ȡ���ݴ�ŵ�ַ
//���������0��ʧ�� 1���ɹ�
//����˵������ȡ�����ֽ�����
//����λ�ã�main
//��   ע����д�ӻ���ַ 
//============================================================================
int readWord(int addr,unsigned  int *data)
{
  unsigned char temp = 0;
  unsigned int readword = 0;
  
  iic_Start();  
  iic_SendByte(IIC_WRITE_CMD);           //����д������
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr >> 8);
  iic_SendByte(temp);      //���͸ߵ�ַ  
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr);
  iic_SendByte(temp);  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  
  iic_Start();
  iic_SendByte(IIC_READ_CMD);           //���ͽ���������  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }

  temp = iic_RcvByte();  
  readword = (unsigned int)temp << 8;
  iic_Ask();  
  
  temp = iic_RcvByte();
  readword |= temp;
  iic_NoAsk();
  *data = readword;
  iic_Stop();
  return 1;
}

//==============================================================================
//�������ƣ�int readByte(int addr, char *data)
//���������addr :��ȡ�ĵ�ַ  *data ��ȡ���ݴ�ŵ�ַ
//���������0��ʧ�� 1���ɹ�
//����˵������ȡ�����ֽ�����
//����λ�ã�main
//��   ע����д�ӻ���ַ 
//============================================================================
int readDWord(int addr,unsigned  long *data)
{
  unsigned char temp = 0;
  unsigned long readword = 0;
  
  iic_Start();  
  iic_SendByte(IIC_WRITE_CMD);           //����д������
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr >> 8);
  iic_SendByte(temp);      //���͸ߵ�ַ  
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr);
  iic_SendByte(temp);  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  iic_Start();
  iic_SendByte(IIC_READ_CMD);           //���ͽ���������  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }

  temp = iic_RcvByte();
  iic_Ask();
  readword = (unsigned long)temp << 24;
  
  temp = iic_RcvByte();
  iic_Ask();
  readword |= (unsigned long)temp << 16;
  
  temp = iic_RcvByte();
  iic_Ask();
  readword |= (unsigned long)temp << 8;
  
  temp = iic_RcvByte();  
  iic_NoAsk();
  readword |= temp;
  
  *data = readword;
  iic_Stop();
  return 1;
}

//==============================================================================
//�������ƣ�int readByte(int addr, char *data)
//���������addr :��ȡ�ĵ�ַ  *data ��ȡ���ݴ�ŵ�ַ
//���������0��ʧ�� 1���ɹ�
//����˵������ȡ�����ֽ�����
//����λ�ã�main
//��   ע����д�ӻ���ַ 
//============================================================================
int readArrayData(int addr,unsigned  char *data,unsigned int length)
{
  unsigned char temp  = 0;          //�м����        
  int i= 0;                         //ѭ������
  
  iic_Start();  
  iic_SendByte(IIC_WRITE_CMD);
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr >> 8);
  iic_SendByte(temp);      //���͸ߵ�ַ  
  temp = iic_GetAsk(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  temp = (unsigned char)(addr);     //���͵͵�ַ
  iic_SendByte(temp);  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  } 
    
  iic_Start();
  iic_SendByte(IIC_READ_CMD);           //���ͽ���������  
  temp = iic_GetAsk();  
  if(temp & BIT1)
  {
    return 0;
  }

  for(i = 0; i < length-1; i++)
  {
    *(data+i) = iic_RcvByte();
    iic_Ask();
  }
 *(data+length-1) = iic_RcvByte();
 
  iic_NoAsk();  
  iic_Stop();
  return 1;
}
//==============================================================================
//�������ƣ�int readByte(int addr, char *data)
//���������addr :��ȡ�ĵ�ַ  *data ��ȡ���ݴ�ŵ�ַ
//���������0��ʧ�� 1���ɹ�
//����˵������ȡ�����ֽ�����
//����λ�ã�main
//��   ע����д�ӻ���ַ 
//============================================================================
void clr_FM24C64(void)
{
    unsigned int i = 0;
    
    for(i = 0; i < 0x1fff; i++)
    {
       writeByte(i, 0xff); 
    } 
}