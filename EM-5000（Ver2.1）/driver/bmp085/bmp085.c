//*****************************Copyright(c)************************************* 
//** ��      ˾��         AMAE 
//** ��      Ŀ��         ADS130    
//** Ӳ���汾�ţ�       
//**================================================================== 
//**------------------------------------ �ļ���Ϣ-------------------------------
//** ��  �� ��:      bmp085.c
//** �ļ� ����:      ��������ѹ�����������¶�
//** ��     ��:      v0.1
//** ��  �� ��:      ����
//** ���� ����:      2012.9.17
//**============================================================================
//==========================================================================
//ͷ�ļ�
//==========================================================================
#include "msp430f5438.h"
#include "bmp085.h"
#include "parameter.h"
#include "tick.h"
//==========================================================================
//�궨��
//==========================================================================
#define BMP085_SDA_OUT      P3DIR |= BIT1         //SDA���
#define BMP085_SDA_IN       P3DIR &= ~BIT1        //SDA����
#define BMP085_SDA          (P3IN&BIT1)             //SDA����

#define BMP085_EOC_IN       P3DIR &= ~BIT5        //EOC����
#define BMP085_SCL_OUT      P3DIR |= BIT2         //SCL���
#define BMP085_XCLK_OUT     P3DIR |= BIT4         //��λ���

#define BMP085_XCLK_H       P3OUT |= BIT4       //��λ����
#define BMP085_XCLK_L       P3OUT &= ~BIT4

#define BMP085_SDA_H        P3OUT |= BIT1       //���������ƽ
#define BMP085_SDA_L        P3OUT &= ~BIT1

#define BMP085_SCL_H        P3OUT |= BIT2       //ʱ�������ƽ
#define BMP085_SCL_L        P3OUT &= ~BIT2

//#define BMP085_EOC          (P3IN&BIT5)           //ת����ɱ�־λ

#define BMP085_WRITE_CMD    0xee                //bmp085дָ��
#define BMP085_READ_CMD     0xEF                //bmp085��ָ��

//==========================================================================
//��������
//==========================================================================
static BMP085_PARAMETER bmp085_parameter;

float curPressure = 0.0;                        //��ǰ����ѹǿֵ
//float curTemperature = 0.0;                     //085�õ���ǰ����
//unsigned char Bmp085_State = 0;                 //оƬ��ǰ��״̬


//==========================================================================
//��������
//==========================================================================
long read_bmp085_pressure();                    //δУ׼��ѹ��ֵ
long read_bmp085_temperture();                  //δУ׼���¶�ֵ
void bmp085_start();                            //����BMP085оƬ

//==========================================================================
//��������
//==========================================================================
//===========================================================================
//�������ƣ�void init_Bmp085(void)
//���������void
//���������void
//����˵������ʼ��bmp085оƬ
//����λ�ã�main
//��   ע��
//============================================================================
void init_bmp085(void)
{
	BMP085_SCL_OUT;
	
	bmp085_parameter.ac1 = bmp085_read_word(0xAA);
	bmp085_parameter.ac2 = bmp085_read_word(0xAC);
	bmp085_parameter.ac3 = bmp085_read_word(0xAE);
	bmp085_parameter.ac4 = bmp085_read_word(0xB0);
	bmp085_parameter.ac5 = bmp085_read_word(0xB2);
	bmp085_parameter.ac6 = bmp085_read_word(0xB4);
	bmp085_parameter.b1 =  bmp085_read_word(0xB6);
	bmp085_parameter.b2 =  bmp085_read_word(0xB8);
	bmp085_parameter.mb =  bmp085_read_word(0xBA);
	bmp085_parameter.mc =  bmp085_read_word(0xBC);
	bmp085_parameter.md =  bmp085_read_word(0xBE);
	
	
}

//==============================================================================
//�������ƣ�void bmp085_start(void)
//���������void
//���������void
//����˵����iic�����ź�
//����λ�ã�
//��   ע��
//============================================================================
void bmp085_start(void)
{
	BMP085_SDA_OUT;
	
    BMP085_SDA_H;                    //����������
    BMP085_SCL_H;                    //����ʱ����
	
    delay(2);                 		 //��ʱ
	
    BMP085_SDA_L;                    //�����½���
  //  delay(2);                 		 //��ʱ
    BMP085_SCL_L;                    //����ʱ����
}

//==============================================================================
//�������ƣ�void bmp085_stop(void)
//���������void
//���������void
//����˵����iicֹͣ�ź�
//����λ�ã�
//��   ע��
//============================================================================
void bmp085_stop(void)
{
  BMP085_SDA_OUT;    //BMP085ֹͣ��SCL��ʱ��SDA�ɵ͵�������
  BMP085_SCL_L;
  BMP085_SDA_L;
  delay(2);
  BMP085_SCL_H;
 // delay(2);
  BMP085_SDA_H;
  delay(2); 
}

//==============================================================================
//�������ƣ�void bmp085_ask(void)
//���������void
//���������void
//����˵����������Ӧȷ����Ϣ
//����λ�ã�
//��   ע��
//============================================================================
void bmp085_ask(void)
{
  BMP085_SDA_OUT;         //ȷ��ʱ��SDA��
  BMP085_SCL_L;
  BMP085_SDA_L;
  delay(2);
  BMP085_SCL_H;
  delay(2);
  BMP085_SCL_L; 
 // delay(3);
}

//==============================================================================
//�������ƣ�void bmp085_ask(void)
//���������void
//���������void
//����˵������������Ӧȷ����Ϣ
//����λ�ã�
//��   ע��
//============================================================================
void bmp085_no_ask(void)
{
  BMP085_SDA_OUT;         //��ȷ��ʱ��SDA��
  BMP085_SCL_L;
  BMP085_SDA_H;
  delay(2);
  BMP085_SCL_H;
  delay(2);
  BMP085_SCL_L; 
  //delay(3);
}

//==============================================================================
//�������ƣ�unsigned char bmp085_get_ask(void)
//���������void
//���������Ӧ���ź� 0��ʧ�� 1���ɹ�
//����˵�����ӻ��ظ�Ӧ���ź�
//����λ�ã�
//��   ע��
//============================================================================
unsigned char bmp085_get_ask(void)
{
  unsigned char temp = 0;                       
  BMP085_SCL_L; 
  delay(2);
  BMP085_SCL_H;
  delay(2);
  BMP085_SDA_IN;   
  temp = BMP085_SDA;
  delay(2);
  BMP085_SCL_L;  
  delay(2);
  return(temp & BIT1);
}

//==============================================================================
//�������ƣ�void bmp085_send_byte(unsigned char data)
//���������data:��������
//���������void
//����˵����IIC���߷�������
//����λ�ã�
//��   ע��
//============================================================================
void bmp085_send_byte(unsigned char data)
{
  unsigned char i = 0;
  
  BMP085_SDA_OUT;

  for(i = 0; i < 8; i++)
  {
      
    BMP085_SCL_L;
    if(data &0x80)                                  //��λ�ȷ�����λ��Ч
    {
      BMP085_SDA_H;
    }
    else
    {
      BMP085_SDA_L;
    }
    BMP085_SCL_H;
    data <<= 1;
  }
  BMP085_SCL_L;
}

//==============================================================================
//�������ƣ�unsigned char bmp085_recv_byte(void)
//���������void
//���������data:��������
//����˵����IIC���߽�������
//����λ�ã�
//��   ע��
//============================================================================
unsigned char bmp085_recv_byte(void)
{
  unsigned char i = 0;
  unsigned char temp = 0;  
  
  BMP085_SDA_IN;
  
  for(i = 0; i < 8; i++)
  {
      BMP085_SCL_H;      
      if(BMP085_SDA)    
      {
        temp |= (0x80 >> i);      
      }
      BMP085_SCL_L;
   }    
   return temp;  
}


//==============================================================================
//�������ƣ�unsigned char bmp085_read_word( unsigned char addr,unsigned int *data)
//���������addr���Ĵ�����ַ data����ȡ����
//���������0����ʧ��  ��0�����ɹ�
//����˵�����������ֽ�����
//����λ�ã�
//��   ע��
//============================================================================
unsigned int bmp085_read_word( unsigned char addr)
{
  unsigned char msb = 0;
  unsigned char lsb = 0;
  unsigned data = 0;
  unsigned char temp = 0;
  
  bmp085_start();
  
  bmp085_send_byte(BMP085_WRITE_CMD);                   //����д����������
  temp = bmp085_get_ask(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  bmp085_send_byte(addr);                        //���ͼĴ�����ַ    
  temp = bmp085_get_ask();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  bmp085_start();
  bmp085_send_byte(BMP085_READ_CMD);             //���Ͷ����������� 
  temp = bmp085_get_ask();  
  if(temp & BIT1)
  {
    return 0;
  }

  msb = bmp085_recv_byte();                      //��ȡ��8λ
  bmp085_ask();
  
  lsb = bmp085_recv_byte();                      //��ȡ��8λ����
  bmp085_no_ask();
  bmp085_stop();
  delay(6);
  data = (unsigned int)msb << 8;
  data |= lsb;
  return data;
	
}
//==============================================================================
//�������ƣ�long read_bmp085_temp(void)
//���������void
//�����������ǰ�¶Ȳ���ֵ
//����˵������ȡδУ׼�¶�ֵ
//����λ�ã�
//��   ע��
//============================================================================
long read_bmp085_temp(void)
{
  unsigned int temp = 0;
  
  bmp085_start();
  bmp085_send_byte(BMP085_WRITE_CMD);            //����д����������
  temp = bmp085_get_ask(); 
  if(temp & BIT1)
  {
    return 0;
  }
  
  bmp085_send_byte(0xf4);                        //���Ϳ��ƼĴ�����ַ    
  temp = bmp085_get_ask();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  bmp085_send_byte(0x2e);                        //����ѹǿ����    
  temp = bmp085_get_ask();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  bmp085_stop();                    
  
 /* while(!BMP085_EOC)                               //�ȴ�ת�����
  {
  	;
  } */
  delayTick(10);
  
  bmp085_start();                               //����ѹ����������      
  temp = bmp085_read_word(0xf6);
  return ((long)temp);
}

//==============================================================================
//�������ƣ�long read_bmp085_pressure(void)
//���������void
//�����������ǰѹǿ����ֵ
//����˵������ȡδУ׼ѹǿֵ
//����λ�ã�
//��   ע��
//============================================================================
long read_bmp085_pressure(void)
{
  unsigned int press = 0;
  unsigned char temp = 0;
  
  bmp085_start();
  bmp085_send_byte(BMP085_WRITE_CMD);            //����д����������
  temp = bmp085_get_ask();   
  if(temp & BIT1)
  {
    return 0;
  }
  
  bmp085_send_byte(0xf4);                        //���Ϳ��ƼĴ�����ַ    
  temp = bmp085_get_ask();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  bmp085_send_byte(0x34);                        //����ѹǿ����    
  temp = bmp085_get_ask();  
  if(temp & BIT1)
  {
    return 0;
  }
  
  bmp085_stop();                    
                                //�ȴ�ת�����
 /* while(!BMP085_EOC)
  {
  	;
  }*/
  delayTick(10);
  
  bmp085_start();                               //����ѹ����������
  
  press = bmp085_read_word(0xf6);
 
  return ((long)press & 0x0000ffff);
}

//==============================================================================
//�������ƣ�unsigned char check_press_trans(void)
//���������void
//�����������������״̬��0���쳣 1������
//����˵������ȡδУ׼�¶�ֵ
//����λ�ã�
//��   ע��
//============================================================================
unsigned char check_press_trans(void)
{
	init_bmp085();
	
	if((bmp085_parameter.ac1 == 0xff)||(bmp085_parameter.ac2 == 0xff)
	   ||(bmp085_parameter.ac3 == 0xff)||(bmp085_parameter.ac4 == 0xff)
		   ||(bmp085_parameter.ac5 == 0xff)||(bmp085_parameter.ac6 == 0xff)
			   ||(bmp085_parameter.b1 == 0xff)||(bmp085_parameter.b2 == 0xff)
				   ||(bmp085_parameter.mb == 0xff)||(bmp085_parameter.mc == 0xff)
					   ||(bmp085_parameter.md == 0xff))
	{
		return 0;
	}
	return 1;
}

//==============================================================================
//�������ƣ�long read_bmp085_temp(void)
//���������void
//�����������ǰ�¶Ȳ���ֵ
//����˵������ȡδУ׼�¶�ֵ
//����λ�ã�
//��   ע��
//============================================================================
void pressTask(void)
{
  long x1 = 0;
  long x2 = 0;
  long x3 = 0;
  long b5 = 0;
  long b3 = 0;
  long b6 = 0;
  unsigned long b4 = 0;
  unsigned long b7 = 0;  
  long temper = 0;                              //����¶�
  long pressure = 0;                            //���ѹǿ
     
  init_bmp085();
  while(1)
  {
    temper = read_bmp085_temp();                //��ȡ�¶ȡ�ѹǿ����ֵ
    pressure = read_bmp085_pressure();    

    x1 = ((temper - bmp085_parameter.ac6)*bmp085_parameter.ac5) >> 15;    //�����¶�ֵ
    x2 = ((long)bmp085_parameter.mc << 11)/(x1 + bmp085_parameter.md);
    b5 = x1 + x2;
     
    b6 = b5 - 4000;                                             //����ѹǿֵ  
    x1 = (b6 * b6) >> 12;
    x1 *= bmp085_parameter.b2;
    x1 >>= 11;
    x2 = bmp085_parameter.ac2 * b6;
    x2 >>= 11;
    x3 = x1 + x2;
    
    b3 = (((long)bmp085_parameter.ac1 * 4 + x3) + 2) >> 2;
    x1 = (bmp085_parameter.ac3 * b6) >> 13;
    x2 = (bmp085_parameter.b1 * ((b6 * b6) >> 12)) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    b4 = (bmp085_parameter.ac4 *(unsigned long)(x3 + 32768)) >> 15;
    b7 = (unsigned long)(pressure - b3) * 50000;
    
    if(b7 < 0x80000000)
    {
      pressure = (b7 << 1) / b4;
    }
    else
    {
      pressure = (b7 / b4) << 1;    
    }
    x1 = pressure >> 8;
    x1 *= x1;
    x1 = (x1 * 3038) >> 16;
    x2 = (pressure * (-7357)) >> 16;
    pressure += (x1 + x2 +3791) >> 4;
	
    curPressure = (float)pressure / 1000;                   //��ȡ��ǰѹǿ KPa
	delayTick(500);
  }

}