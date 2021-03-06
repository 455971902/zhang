

#include "lcd.h"


 __no_init static unsigned char lcd_buf_p[1024];                //液晶内存副本指针


//////////////////////////////////////////////////////////////////////////
void write_cmd(unsigned char cmd)  //发送命令到液晶
{
  CS_ON;          //cs信号有效
  E_ON;           //E信号有效
  CMD_ON;         //写命令
  W_ON;           //进行写操作
  LCD_io=cmd;     //发送命令
  //delay(2);       //延时
  E_OFF;          //关E信号
  CS_OFF;         //关CS信号
  
}

void write_data(unsigned char data) //发送数据到液晶
{
  CS_ON;           //cs信号有效
  E_ON;            //E信号有效
  DATA_ON;         //数据
  W_ON;            //进行写操作
  LCD_io=data;     //发送数据
  //delay(2);        //延时
  E_OFF;           //关E信号
  CS_OFF;          //关CS信号
}

void send_byte_pos(unsigned char x,unsigned char page,unsigned char data)
{//*在液晶的x列，一字节的page页处写一个字节数据，其中8点一页，64点液晶有8页
  unsigned char x_h=0;
  unsigned char x_l=0;
  
  write_cmd(page+0xb0);          //设置页地址
  
  x_h=x&0xf0;                    //获得列地址高四位
  x_h>>=4;                       //右移四位
  write_cmd(x_h+0x10);           //设置列地址高四位
  
  x_l=x&0x0f;                    //获得列地址低四位
  write_cmd(x_l);                //设置列地址低四位
  
  write_data(data);              //发送数据
  //*(lcd_buf_p+y*128+x)=data;   //更新标准
}


void init_Lcd(void)  //初始化液晶函数
{
  P4DIR|=0xf8;
  P4OUT|=0xf8;
  
  P8DIR|=0xff; 
  
  RES_ON;                   //进行复位操作
  //delay(10);                //延时一段时间
  RES_OFF;                  //清除复位信号
  //delay(10);                //延时一段时间
  
  write_cmd(0xe2);          //软件复位液晶
  //delay(20);                //延时一段时间
  
  write_cmd(0xa0);          //清除ADC
  write_cmd(0xc8);          //设置SHL
  write_cmd(0xa2);          //清除偏置
  write_cmd(0x28|0x07);     //电源控制
  write_cmd(0x20|0x05);     //参考电阻选择
  write_cmd(0x81);          //设置灰度，字体亮度
  //write_cmd(0x1a);          //LCD型号FS-4
  write_cmd(0x0e);        //LCD型号FS-12
  write_cmd(0x00|0x40);     //初始化行
  write_cmd(0xaf);          //开启液晶
  //delay(100);               //延时一段时间
}

void clr_lcd(void)        //清除整个液晶屏
{
  unsigned char index_x;       //页索引值
  unsigned char index_y;       //行索引值，这里的行指的是大行8点一行
  
  _DINT();
  
  for(index_y=0;index_y<8;index_y++)
  {
    write_cmd(index_y+0xb0);   //设置页地址
    write_cmd(0x10);           //设置列首址高位
    write_cmd(0x00);           //设置列首址地位
    
    for(index_x=0;index_x<128;index_x++) //发送128个0x00数据，列地址自动增1
    {
        
      write_data(0x00);                 //发送数据
      *(lcd_buf_p+index_y*128+index_x)=0x00;
      
    }
  }
  _EINT();
}
//////////////////////////////////////////////////////////////////////////


void clr_lcd_zone(unsigned char x,     //清除以(x,y)为起点，width为宽，height为
                       unsigned char y,        //高的屏幕区域
                       unsigned char width,
                       unsigned char height)
{
  
}


void clr_point(unsigned char x,unsigned char y)    //清除一点函数，(x,y)坐标
{
  unsigned char offset;   //页内偏移变量
  unsigned char page;     //页地址变量
  unsigned char data;     //用于保存液晶内存数据一个字节
  
  
  
  if(x<128 && y<128)
  {
    page=y_page_tbl[y];            //获取页地址
  
    offset=y%8;                    //获取页内偏移
    
    offset=0x01 << offset;         //获得业内偏移比特位
    
    data=*(lcd_buf_p+page*128+x);  //读取液晶该字节处的置
    
    data&=~offset;                 //清除该位
    
    _DINT();
    send_byte_pos(x,page,data);    //发送数据到液晶
    *(lcd_buf_p+page*128+x)=data;  //更新液晶缓存
    _EINT();
  }

}
    
void draw_point(unsigned char x,unsigned char y)   //描一点函数，(x,y)坐标
{
  unsigned char offset;   //页内偏移变量
  unsigned char page;     //页地址变量
  unsigned char data;     //用于保存液晶内存数据一个字节
  
  if(x<128 && y<64)
  {
    page=y_page_tbl[y];   //获取页地址
  
    offset=y%8;           //获取页内偏移
    
    offset=0x01 << offset;   //获得业内偏移比特位
    
    data=*(lcd_buf_p+page*128+x);  //读取液晶该字节处的置
    
    data|=offset;                 //设置该位
    
    _DINT();
    send_byte_pos(x,page,data);    //发送数据到液晶
    *(lcd_buf_p+page*128+x)=data;  //更新液晶缓存
    _EINT();
  }
}
  
void draw_line(unsigned char x,  //从(x,y)处到(end_x,end_y)处描一条线函数
                    unsigned char y,
                    unsigned char end_x,
                    unsigned char end_y)
{
    double i;
    unsigned int j;
    double k;  
    double x_2;
    double y_2;
    
    if(y==end_y && x!=end_x)
    {
      if(x<=end_x)
      {
        for(j=x;j<=end_x;j++)
        {
          draw_point(j,y);
        }
      }
      else
      {
        for(j=end_x;j<=x;j++)
        {
          draw_point(j,y);
        }
      }
    }else if(x==end_x && y!=end_y)
    {
      if(y<=end_y)
      {
        for(j=y;j<=end_y;j++)
        {
          draw_point(x,j);
        }
      }else
      {
        for(j=end_y;j<=y;j++)
        {
          draw_point(x,j);
        }
      }
    }else if(x==end_x&&y==end_y)
    {
      draw_point(x,y);
    }else
    {
      if(x<end_x)
      {
        x_2=(end_x-x)*(end_x-x);
        y_2=(end_y-y)*(end_y-y);
        k=sqrt(x_2+y_2);
        for(i=0;i<k;i+=0.5)
        {
          x_2=(end_x-x)/k*i+x;
          y_2=(end_y-y)/k*i+y;
          draw_point((unsigned char)x_2,(unsigned char)y_2);
        }
      }
    }
}

  
void draw_rect(unsigned char x, //从(x,y)处描一个宽为width，高为height的矩形框
               unsigned char y,
               unsigned char width,
               unsigned char height)
{
  unsigned int i,j;   //i用于列计数，j用于行计数
  if(x<128 && y<64)
  {
    if((x+width)>128)    //限制宽度溢出
    {
      width=128-x;
    }
     
    if((y+height)>64)   //限制高度溢出
    {
      height=64-y;
    }
    
    for(i=0;i<width;i++)   //描矩形框的上下两边
    {
      draw_point(x+i,y);  
      draw_point(x+i,y+height-1);
    }
    for(j=0;j<height;j++)   //描矩形框的左右两边框
    {
      draw_point(x,y+j);
      draw_point(x+width-1,y+j);
    }
  }
}

  
void clr_rect(unsigned char x,   //清除以(x,y)为左上角清除一个宽为width，高为height
              unsigned char y,   //的矩形区域
              unsigned char width,
              unsigned char height)
{
  unsigned char page;          //用于记录y所属的页
  unsigned char page_num;      //用于记录处首部和尾部外中间所含的页数
  unsigned char data;          //用于保存在page页，x列处的字节数据
  unsigned char top_offset;    //用于记录头部，也即y行所属页的偏移值
  unsigned char bot_offset;    //用于记录尾部，也即是最后一行所属页内偏移值
  
  unsigned char new_height;    //用于记录处首部，尾部之外中间所有行行数
  unsigned char i,j;           //i用于列计数，j用于行计数
  
  _DINT();
  
  if(x<128 && y<64)
  {
    if(width>(128-x))       //限制宽度溢出
    {
      width=128-x;
    }
    if(height>(128-y))     //限制高度溢出
    {
      height=128-y;
    }
    
    page=y_page_tbl[y];           //获取y所属页
    top_offset=y%8;               //获取y在所属页内的偏移值
    bot_offset=(y+height)%8;      //获取最后一行所属页内的偏移值
    new_height=height;            //保存new_height
    
    if(height<=(8-top_offset))   //如果填充的区域在一个页内
    {
      write_cmd(page+0xb0);          //设置页地址
      write_cmd(((x&0xf0)>>4)+0x10); //设置列地址高四位
      write_cmd(x&0x0f);             //设置列地址低四位,其地址会自动增1
      for(i=0;i<width;i++)           //一列一列的清除
      {
        data=*(lcd_buf_p+page*128+x+i);         //从副本中读取液晶中数据
        data&=~(bit_set[height-1]<<top_offset); //清除相应的位
        write_data(data);                       //发送修改后的数据
        *(lcd_buf_p+page*128+x+i)=data;         //保存到副本
      }
    }else                     //如果要填充的区域在多个页内
    {
        if(top_offset)   //如果第一页是没有满页，则先将其处理
        {
          write_cmd(page+0xb0);          //设置页地址
          write_cmd(((x&0xf0)>>4)+0x10); //设置列地址高四位
          write_cmd(x&0x0f);             //设置页地址低四位
          for(i=0;i<width;i++)
          {
            data=*(lcd_buf_p+page*128+x+i);   //从副本中获取其一个字节数据
            data&=bit_set[top_offset-1];      //清除相应的位数据
            write_data(data);                 //写数据到液晶
            *(lcd_buf_p+page*128+x+i)=data;   //将修改后的字节保存到副本
          }
          page++;                             //页加1操作
          new_height=height-(8-top_offset);   //new_height减去头部未填满页的高度
        }
      if(new_height>=8)    //如果要填充的区域多于或刚刚好等于两页范围
      {
        new_height-=bot_offset;   //减去尾部未填满页的高度
        page_num=new_height>>3;   //高度值除以3，得到总的页数
        for(j=0;j<page_num;j++)   //一页一页的描
        {
          write_cmd(page+0xb0);           //设置页地址
          write_cmd(((x&0xf0)>>4)+0x10);  //设置列地址高四位
          write_cmd(x&0x0f);              //设置列地址低四位
          for(i=0;i<width;i++)
          {
            data=0x00;                    //直接使数据位0
            write_data(data);             //发送到液晶屏
            *(lcd_buf_p+page*128+x+i)=data;  //保存到副本
          }
          page++;                         //页数自动增1
        }
      }
      if(bot_offset)
      {
        write_cmd(page+0xb0);   //设置页地址
        write_cmd(((x&0xf0)>>4)+0x10);   //设置列地址高四位
        write_cmd(x&0x0f);       //设置列地址的低四位
        for(i=0;i<width;i++)
        {
          data=*(lcd_buf_p+page*128+x+i);    //读取LCD副本中page页，x+i列中的一个字节
          data&=~(bit_set[bot_offset-1]);    //清除相应的位
          write_data(data);                  //发送数据到液晶
          *(lcd_buf_p+page*128+x+i)=data;    //保存到副本
        }
      }
    }
  }
  
  _EINT();
}

void fill_rect(unsigned char x, //以(x,y)为左上角填充一个宽为width，高为height的矩形
               unsigned char y, //区域
               unsigned char width,
               unsigned char height)
{
  unsigned char page;       //用于记录y所属的页
  unsigned char page_num;   //高度范围内包含的整页页数
  unsigned char data;       //用于保存从lcd_buffer中取得字节数据
  unsigned char top_offset;     //用于记录开头行在其所属的页的偏移值
  unsigned char bot_offset;     //用于记录结尾行在其所属的页的偏移值
  
  unsigned char new_height=height;
  
  unsigned char i,j;
  
  
  _DINT();
  
  if(x<128 && y<64 && width>0 && height >0)
  {
    if(width>(128-x))   //限制width溢出范围
    {
      width=128-x;    
    }
    
    if(height>(64-y))  //限制height溢出范围
    {
      height=64-y;
    }
    
    top_offset=y%8;                //获取首行所属页内偏移值
    bot_offset=(y+height)%8;       //获得尾行所属页内偏移值
    for(i=0;i<width;i++)           //一列一列的循环描
    {
      page=y_page_tbl[y];          //获得y行所属的页地址
      new_height=height;
      if(height<=(8-top_offset))   //如果所填充的点在同一页以内
      {
        data=*(lcd_buf_p+page*128+x+i);         //从lcd数据副本中获取所属poage页x行的字节数据
        data|=bit_set[height-1] << top_offset;  //设置页内要填充的几个位
        send_byte_pos(x+i,page,data);           //将该数据发送
        *(lcd_buf_p+page*128+x+i)=data;         //保存到副本
      }else                       //如果所填充的点属于多个页
      {
        if(top_offset)  //先填充第一页，并且是没有被填满的页
        {
           data=*(lcd_buf_p+page*128+x+i);         //从lcd数据副本中获取所属poage页x行的字节数据
           data|=~bit_set[top_offset-1];           //设置页内要填充的几个位
           send_byte_pos(x+i,page,data);           //将该数据发送
           *(lcd_buf_p+page*128+x+i)=data;         //保存到副本
           page++;
           new_height=height-(8-top_offset);       //除头部部分未填满字节的部分高度
         }

        new_height-=bot_offset;                    //除尾部的未填满字节部分的高度
        if(new_height>=8)
        {
          page_num=new_height>>3;                    //获得中间部分的页数
          for(j=0;j<page_num;j++)
          {
            data=0xff;
            send_byte_pos(x+i,page+j,data);
            *(lcd_buf_p+(page+j)*128+x+i)=data;
          }
          page+=j;                                   //让页地址加上中间部分的值
        }
        if(bot_offset)
        {
           data=*(lcd_buf_p+page*128+x+i);         //从lcd数据副本中获取所属poage页x行的字节数据
           data|=bit_set[bot_offset-1];           //设置页内要填充的几个位
           send_byte_pos(x+i,page,data);           //将该数据发送
           *(lcd_buf_p+page*128+x+i)=data;         //保存到副本
        }
      }
    }
  }
  
  _EINT();
}
                        
                        
  
void draw_symbol(unsigned char x,      //从(x,y)处描一个宽为width，高为height的
                 unsigned char y,      //图标，symbol_p为图标数据的指针
                 unsigned char width,  //mod为方式，mod=0正常描，mod=1反色
                 unsigned char height,
                 const unsigned char* symbol_p,
                 unsigned char mod)
{
   unsigned char i;
   unsigned char j;
   unsigned char m,n;
   unsigned char k,offset;
   unsigned char data=0;
   
   if(width!=0 && height!=0 && x<128 && y<64)
   {
     
     if(width>(128-x))   //防止宽度溢出
     {
       width=128-x;
     }
     if(height>(64-y))  //防止高度溢出
     {
       height=64-y;
     }
     
     k=y_page_tbl[height];  //获取height高度所含字节数
     offset=height%8;       //获得高度最后一个字节内的偏移值
     if(!mod)
     {
        for(i=0;i<width;i++)   //一列一列的描
        {
          for(j=0;j<k;j++)
          {
            if(offset>0)
            {
               data=symbol_p[i*(k+1)+j];     //读取字模数据
            }
			else
            {
               data=symbol_p[i*k+j];
            }
            m=0x01;
            for(n=0;n<8;n++)
            {
              if(data&m)
              {
                 draw_point(x+i,y+j*8+n);
               }
			   else
               {
                 clr_point(x+i,y+j*8+n);
               }
                 m<<=1;
             }
           }    
		  
           if(offset>0)
           {
              data=symbol_p[i*(k+1)+j];
              m=0x01;
              for(n=0;n<offset;n++)
              {
                if(data&m)
                {
                  draw_point(x+i,y+j*8+n);
                }else
                {
                  clr_point(x+i,y+j*8+n);
                }
                  m<<=1;
               }
           }   
         }
      }	 
	 
	 else
      {
        for(i=0;i<width;i++)   //一列一列的描
        {
          for(j=0;j<k;j++)
          {
            if(offset>0)
            {
               data=symbol_p[i*(k+1)+j];     //读取字模数据
            }else
            {
               data=symbol_p[i*k+j];
            }
            m=0x01;
            for(n=0;n<8;n++)
            {
              if(data&m)
              {
                 clr_point(x+i,y+j*8+n);
               }else
               {
                 draw_point(x+i,y+j*8+n);
               }
                 m<<=1;
             }
           }    
           if(offset>0)
           {
              data=symbol_p[i*(k+1)+j];
              m=0x01;
              for(n=0;n<offset;n++)
              {
                if(data&m)
                {
                  clr_point(x+i,y+j*8+n);
                }else
                {
                  draw_point(x+i,y+j*8+n);
                }
                  m<<=1;
               }
           }   
         }
      }
    }
}


////////////////////////以下为字符打印类函数////////////////////////////

  //往液晶打印字符函数以(x,y)为左上角,mod为打印的方式，mod=0为正常打印
  //mod=1为反色打印
int putchar(unsigned char x,unsigned char y,char c,unsigned char mod)
{
  int re=0; //用于返回的变量
  int i,j;  //i用于打印的列计数，j用于打印行计数的
  unsigned char data;       //用于获取字模的每一个字节
  unsigned char k;          //用于获取字模字节里的位数据
  
  if(x<122 && y<52 && c>=32)  //由于每个字符大小为6x12
  {
    re=1;     //条件符合则使返回值为1
    if(!mod)   //如果mod=0，则正常显示
    {
      for(i=0;i<6;i++)  //对列进行循环
      {
        data=asics[c-32][i*2];   //获得字模数据
        k=0x01;                  //第一次使用使k=0x01，也即保留data的第0位，
                                 //其他清除
        
        for(j=0;j<8;j++)         //对行进行循环，由于字高为12，因此第一个字节取
        {                        //完8位，后一个字节取四位
          if(k&data) //k与data进行相与，也即取位操作，若果为1，则描该点
          {
            draw_point(x+i,y+j);    //描点操作，位置为(x+i,y+i)
          }else                     //反之则清除该点
          { 
            clr_point(x+i,y+j);     //清除该点，位置为(x+i,y+i)
          }
          k<<=1;        //k左移一位
        }
        
        data=asics[c-32][i*2+1];      //取一列中的第二个字节，但只用其前四位
        k=0x01;
        for(j=8;j<12;j++)
        {
          if(k&data)
          {
            draw_point(x+i,y+j);
          }else
          {
            clr_point(x+i,y+j);
          }
          k<<=1;
        }
      }
      
    }else     //如果mod=1，则进行反色显示，只是draw与clr进行调换
    {
      for(i=0;i<6;i++)
      {
        data=asics[c-32][i*2];
        k=0x01;
        for(j=0;j<8;j++)
        {
          if(k&data)
          {
            clr_point(x+i,y+j);
          }else
          {
            draw_point(x+i,y+j);
          }
          k<<=1;
        }
        
        data=asics[c-32][i*2+1];
        k=0x01;
        for(j=8;j<12;j++)
        {
          if(k&data)
          {
            clr_point(x+i,y+j);
          }else
          {
            draw_point(x+i,y+j);
          }
          k<<=1;
        }
      }
    }
  }
  return re;
}


void print_num(unsigned x,unsigned y,unsigned char num,unsigned char mod)
{
   if(!mod)   //正显
   {
       draw_symbol(x,y,8,16,num_char[num],0);
   }
   else   
   {
       draw_symbol(x,y,8,16,num_char[num],1);
       
   }
}
void print_int_num(unsigned x,unsigned y,unsigned int num,unsigned char mod)
{  
   if(!mod)   //正显
    {           
           draw_symbol(x,y,9,16,num_char[num/1000],0); 
           draw_symbol(x+9,y,9,16,num_char[num%1000/100],0);
           draw_symbol(x+18,y,9,16,num_char[num%100/10],0);
           draw_symbol(x+27,y,9,16,num_char[num%10],0);
    }
   else   
   {
           draw_symbol(x,y,9,16,num_char[num/1000],1); 
           draw_symbol(x+9,y,9,16,num_char[num%1000/100],1);
           draw_symbol(x+18,y,9,16,num_char[num%100/10],1);
           draw_symbol(x+27,y,9,16,num_char[num%10],1);     
   }        
}




/*
int putchar_24_24(int x,int y,char c,int mod)
{
	  int re=0; //用于返回的变量
	  int i,j;  //i用于打印的列计数，j用于打印行计数的
	  unsigned char data;       //用于获取字模的每一个字节
	  unsigned char k;          //用于获取字模字节里的位数据

	  if(x<117 && y<40 && c>=32)  //由于每个字符大小为6x12
	  {
	    re=1;     //条件符合则使返回值为1
	    if(!mod)   //如果mod=0，则正常显示
	    {
	      for(i=0;i<12;i++)  //对列进行循环
	      {
	        data=asics_24_24[c-32][i*3];   //获得字模数据
	        k=0x01;                  //第一次使用使k=0x01，也即保留data的第0位，
	                                 //其他清除
	        for(j=0;j<8;j++)         //对行进行循环，由于字高为12，因此第一个字节取
	        {                        //完8位，后一个字节取四位
	          if(k&data) //k与data进行相与，也即取位操作，若果为1，则描该点
	          {
	            draw_point(x+i,y+j);    //描点操作，位置为(x+i,y+i)
	          }else                     //反之则清除该点
	          {
	            clr_point(x+i,y+j);     //清除该点，位置为(x+i,y+i)
	          }
	          k<<=1;        //k左移一位
	        }

	        data=asics_24_24[c-32][i*3+1];      //取一列中的第二个字节，但只用其前四位
	        k=0x01;
	        for(j=8;j<16;j++)
	        {
	          if(k&data)
	          {
	            draw_point(x+i,y+j);
	          }else
	          {
	            clr_point(x+i,y+j);
	          }
	          k<<=1;
	        }

	        data = asics_24_24[c-32][i*3+2];
	        k=0x01;
	        for(j=16;j<24;j++)
	        {
	          if(k&data)
	          {
	            draw_point(x+i,y+j);
	          }else
	          {
	            clr_point(x+i,y+j);
	          }
	          k<<=1;
	        }
	      }
	    }else     //如果mod=1，则进行反色显示，只是draw与clr进行调换
	    {
	      for(i=0;i<12;i++)
	      {
	        data=asics_24_24[c-32][i*3];
	        k=0x01;
	        for(j=0;j<8;j++)
	        {
	          if(k&data)
	          {
	            clr_point(x+i,y+j);
	          }else
	          {
	            draw_point(x+i,y+j);
	          }
	          k<<=1;
	        }

	        data=asics_24_24[c-32][i*3+1];
	        k=0x01;
	        for(j=8;j<16;j++)
	        {
	          if(k&data)
	          {
	            clr_point(x+i,y+j);
	          }else
	          {
	            draw_point(x+i,y+j);
	          }
	          k<<=1;
	        }

	        data=asics_24_24[c-32][i*3+2];
	        k=0x01;
	        for(j=16;j<24;j++)
	        {
	          if(k&data)
	          {
	            clr_point(x+i,y+j);
	          }else
	          {
	            draw_point(x+i,y+j);
	          }
	          k<<=1;
	        }
	      }
	    }
	  }
	  return re;
}


*/



  //往液晶打印中文字符，以(x,y)为左上角，mod为打印方式，mod=0为正常打印
  //mod=0为反色打印
int putchinese(unsigned x,unsigned char y,unsigned int unicode,char mod)
{
  int re=0;
  //int i,j;   //i为用于对列进行计数，j用于对行进行计数
  //unsigned char data;     //用于存储从自摸中取得的字节数据
  //unsigned char k;        //用于从字节中取得位信息
  unsigned int index;     //用于搜索unicode中文数组chinese中unicode码值
  
  if(x<=116 && y<53)
  {
    for(index=0;index<180;index++)   //在字模数组内寻找相应unicode码的字模索引index
    {
      if(unicode==chinese[index].unicode) //如果要找的unicode与字模数组内的unicode
      {                                   //匹配，则中断此次循环
        re=1;
        break;
      }
    }
    if(index==180)           //如果index寻找到了最后一个都没找到，则直接返回0
    {
      return 0;              //返回0
    }
      
    draw_symbol(x,y,12,12,chinese[index].data,mod);  //打印该字模
  }
  return re;  //返回re，如果打印字符，则返回1，如果没有打印，则返回0
}


unsigned int chinese_to_Uint16(const char* s)  //将中文转换为unsigned int类型
{
  unsigned int re;    //此为返回值
  unsigned int tmp;   //临时变量
  
  tmp=*s++;           //取中文字符串的第一个字节，并令指针s自加1
  re=(*s)<<8;         //取第二个字节，由于unicode码为16位，因此第二个字节里存放
                      //高位，因此第二个字节应该左移8位
  re=re+tmp;          //高位与低位相加
  return re;          //返回unicode码值
}


void print_str(unsigned char x,      //打印一行字符串函数，以(x,y)为左上角
               unsigned char y,      //s为指向字符串的指针，mod为打印的方式，
               const char* s,        //mod=0为正常打印，mod=1为反色打印
               unsigned char mod)
{
  int i=0;
  const char* s_p=s;
  
  for(;*s_p!='\0';s_p++)
  {
    putchar(x+i*6,y,*s_p,mod);
    i++;
  }
}


void print_str_24_24(int x,int y,const char *s,unsigned char mod)
{
	  int i=0;
	  const char* s_p=s;

	  for(;*s_p!='\0';s_p++)
	  {
	    putchar_24_24(x+i*12,y,*s_p,mod);
	    i++;
	  }
}


void print_ch_str(unsigned x,   //打印一行中文字符串函数，(x,y)为左上角
                  unsigned y,   //s为指向字符串的指针，mod为打印方式
                  const char* s,  //mod=0则正常打印，为1则反色打印
                  unsigned char mod)
{
  int i=0;
  const char* s_p=s;
  
  for(;*s_p!='\0';s_p+=2)   //检测到'\0'字符则表示字符串结束
  {
    putchinese(x+i*12,y,chinese_to_Uint16(s_p),mod);  //一个一个的打印字符
    i++;
  }
}


void print_int(unsigned x,      //打印整数函数,以(x,y)为左上角
               unsigned y,
               int val,         //mod为模式，mod=0为正常模式，为1则为反色模式
               unsigned char mod)
{
  char buf[32];
  char *buf_p;
  int i=0;
  
  buf_p=int_to_char(val,buf);
  
  for(;*buf_p!='\0';buf_p++)
  {
    putchar(x+i*6,y,*buf_p,mod);
    i++;
  }
}


void print_int_24_24( unsigned x,    // 打印整数函数,以(x,y)为左上角
					  unsigned y,    // mod为模式，mod=0为正常模式，为1则为反色模式
					  int val,
					  unsigned char mod )
{
	char buf[32];
	char *buf_p;
	int  i = 0;

	buf_p = int_to_char( val, buf );

	for ( ; *buf_p!='\0'; buf_p++ )
	{
		putchar_24_24( x+i*12, y, *buf_p, mod );
		i++;
	}
}




void print_float(unsigned char x,  //打印浮点数函数，
                 unsigned char y,  //以(x,y)点为左上角，mod到为模式
                 float val,
                 unsigned char prec,
                 unsigned char mod)
{
  char buf[32];
  char *buf_p;
  int i=0;
  
  buf_p=ftoa(val,buf,prec);
  
  for(;*buf_p!='\0';buf_p++)
  {
    putchar(x+i*6,y,*buf_p,mod);
    i++;
  }
}


void print_float_24_24( unsigned char x,     // 打印浮点数函数，
					    unsigned char y,     // 以(x,y)点为左上角，mod到为模式
					    float         val,   // 打印的值
					    unsigned char prec,  // prec 为精度
					    unsigned char mod )
{
	char buf[32];
	char *buf_p;
	int  i = 0;

	buf_p = ftoa( val, buf, prec );

	for( ; *buf_p!='\0'; buf_p++ )
	{
		putchar_24_24( x+i*12, y, *buf_p, mod );
		i++;
	}
}




char *int_to_char(int val,char *buf)   //整数转字符串函数
{
  char* bp;
  char buf_tmp[32];
  int tmp;
  int min=0;
  
  bp=buf_tmp;
  
  *bp++='\0';
  
  if(val==0)   //如果要打印的数为0,则直接得到0
  {
    *buf='0';               //使第一个字符为0
    *(buf+1)='\0';          //第二个字符为结束字符
    return (buf);
  }
  
  if(val<0)      //如果为负数，则添加‘-’号
  {
    min=1;
    val=-val;    //取反，得到整数
  }
  
  while(val)    
  {
    tmp=val%10;  //获取个位
    *bp++=tmp+'0'; 
    val-=tmp;   //减去个位
    val/=10;   
  }
  
  if(min) *bp++='-';
  
  bp--;
  
  for(tmp=0;*bp!='\0';bp--)
  {
    *(buf+tmp)=*bp;
    tmp++;
  }
  
  
  *(buf+tmp)='\0';
  
  return (buf);
}


char * ftoa(float f ,char *buf ,int prec)   //浮点数转化字符函数
{
        char *bp;
        int exp, digit;

        prec = (prec <= 0) ? 1 : (prec <= 9) ? prec : 9;   //判断精度
        bp = buf;
        if (f < 0.0) {          //如果是小于0的数
                *bp++ = '-';
                f = -f;         //将其转换为正数
        }
        if (f == 0.0) {         //如果为0
                *bp++ = '0';    *bp++ = '.';    //获得0.字符
                while (prec--)                  //获得精度后面几个0
                        *bp++ = '0';
                *bp = '\0';                     //使其最后一个字符为'\0'字符  
                return(buf);                    //返回字符指针
        }
        for (exp=0; f < 1.0; f = f * 10.0)      //如果f<1,则求其小数点后有多少个0
                exp--;
        for ( ; f >= 1.0; f = f / 10.0)         //如果f>=1则求整数的位数
                exp++;

        if (exp<=0)                             //如果只有一位有效数字
                *bp++ = '0';
        for ( ; exp>0; exp--) {                 //获取有效数字
                f = f * 10.0;
                digit =(int) f;                 //获取一位有效数字
                f = f - digit;
                *bp++ = digit + '0';
        }
        *bp++ = '.';                            //获得.字符
        for( ; exp<0 && prec; prec--, exp++)    //有效位数为0
                *bp++ = '0';
        while(prec-- > 0) {
                f = f * 10.0;
                digit = (int)f;      //获得一个有效位
                f = f - digit;
                *bp++ = digit + '0';
        }
        *bp = '\0';
        return (buf);
}




void print_time(int hour,int min,unsigned char x,unsigned char y,int mod)  //显示时间函数
{ 
  unsigned int shi,ge;
  switch(mod)
  {
  case HOUR:
    {
      clr_rect(x-1,y,40,12);
      fill_rect(x-1,y,16,12);
      shi=(unsigned int)(hour/10);
      putchar(x,y,shi+'0',1);
      ge=(unsigned int)(hour%10);
      putchar(x+8,y,ge+'0',1);
      
      putchar(x+16,y,':',0);
      
      shi=(unsigned int)(min/10);
      putchar(x+24,y,shi+'0',0);
      ge=(unsigned int)(min%10);
      putchar(x+32,y,ge+'0',0);
      break;
    }
  case MIN:
    {
      clr_rect(x-1,y,40,12);
      fill_rect(x+23,y,16,12);
      shi=(unsigned int)(hour/10);
      putchar(x,y,shi+'0',0);
      ge=(unsigned int)(hour%10);
      putchar(x+8,y,ge+'0',0);
      
      putchar(x+16,y,':',0);
      
      shi=(unsigned int)(min/10);
      putchar(x+24,y,shi+'0',1);
      ge=(unsigned int)(min%10);
      putchar(x+32,y,ge+'0',1);
      
      break;
    }
  case HOUR_MIN:
    {
      clr_rect(x-1,y,40,12);
      shi=(unsigned int)(hour/10);
      putchar(x,y,shi+'0',0);
      ge=(unsigned int)(hour%10);
      putchar(x+8,y,ge+'0',0);
      
      putchar(x+16,y,':',0);
      
      shi=(unsigned int)(min/10);
      putchar(x+24,y,shi+'0',0);
      ge=(unsigned int)(min%10);
      putchar(x+32,y,ge+'0',0);
      
      break;
    }
  default:break;
  }
}

void print_mtime(int hour,int min,unsigned char x,unsigned char y,int mod)  //显示时间函数
{ 
  unsigned int shi,ge;
  switch(mod)
  {
  case HOUR:
    {
     // clr_rect(x-1,y,40,12);
     // fill_rect(x-1,y,16,12);
      shi=(unsigned int)(hour/10);
      print_num(x,y,shi,1);
      ge=(unsigned int)(hour%10);
      print_num(x+8+2,y,ge,1);
      
      putchar(x+16+5,y,':',0);
      
      shi=(unsigned int)(min/10);
      print_num(x+24+6,y,shi,0);
      ge=(unsigned int)(min%10);
      print_num(x+32+8,y,ge,0);
      break;
    }
  case MIN:
    {
     // clr_rect(x-1,y,40,12);
     // fill_rect(x+23,y,16,12);
      shi=(unsigned int)(hour/10);
      print_num(x,y,shi,0);
      ge=(unsigned int)(hour%10);
      print_num(x+8+2,y,ge,0);
      
      putchar(x+16+6,y,':',0);
      
      shi=(unsigned int)(min/10);
     print_num(x+24+6,y,shi,1);
      ge=(unsigned int)(min%10);
      print_num(x+32+8,y,ge,1);
      
      break;
    }
  case HOUR_MIN:
    {
      //clr_rect(x-1,y,40,12);
      shi=(unsigned int)(hour/10);
      print_num(x,y,shi,0);
      ge=(unsigned int)(hour%10);
      print_num(x+8+2,y,ge,0);
      
      putchar(x+16+6,y,':',0);
      
      shi=(unsigned int)(min/10);
      print_num(x+24+6,y,shi,0);
      ge=(unsigned int)(min%10);
      print_num(x+32+8,y,ge,0);
      
      break;
    }
  default:break;
  }
}
///////////////////////////////////////////////////////////////////////////////
void draw_round(unsigned char x,  
                unsigned  char y,
               unsigned char num,//0---9   14*14
                unsigned char mod)
{
    
    
  
   if(!mod)   //正显
    {
       draw_symbol(x  ,y  ,14,14,round_mu[0],0);   //描按钮框
       draw_symbol(x+2,y+2,10,10,round_char[num],0);
    }
   else
   
   {
       draw_symbol(x,y,14 ,14,round_mu[1],0);
       draw_symbol(x+2,y+2,10,10,round_char[num],1);
       
   }
    
    
}




void draw_button(unsigned char x,   //以(x,y)为左上角描一个按钮，大小固定
                 unsigned char y,   //text为按钮显示时的标题，比如command
                 const char* text,  //mod为方式，mod=0，则表示按钮未被选中
                 unsigned char mod, //mod=1则表示按钮被选中
                 unsigned char lag) //lag为语言方式，lag=0为英文，lag为1则为中文
{
  const char* s_p=text;
  unsigned int i;
  
  if(!mod)  //如果为方式0，mod=0
  {
    if(lag==0)
    {
      i=0;
      for(;*s_p!='\0';s_p++)  //检查字符串长
      {
        i++;
      }
      if(i)
      {
        draw_symbol(x,y,32,16,button_mu[0],0);   //描按钮框
        print_str(x+(5-i)*3+1,y+2,text,0);       //描字符串
      }
      
    }else
    {
      draw_symbol(x,y,32,16,button_mu[0],0);
      if(text)
      {
        print_ch_str(x+4,y+2,text,0);
      }
    }
  }else   //如果为方式1，mod=1
  {
    if(lag==0)
    {
      i=0;
      for(;*s_p!='\0';s_p++)
      {
        i++;
      }
      if(i)
      {
        draw_symbol(x,y,32,16,button_mu[1],0);
        print_str(x+(5-i)*3+1,y+2,text,1);
      } 
    }else
    {
      draw_symbol(x,y,32,16,button_mu[1],0);
      if(text)
      {
        print_ch_str(x+4,y+2,text,1);
      }
    }
  }
}