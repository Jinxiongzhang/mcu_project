/******************************************************************
   本程序只供学习使用，未经作者许可，不得用于其它任何用途

        欢迎访问我的USB专区：http://group.ednchina.com/93/
        欢迎访问我的blog：   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org

        感谢PCB赞助商——电子园： http://bbs.cepark.com/

UART.C  file

作者：电脑圈圈
建立日期: 2008.06.27
修改日期: 2008.07.10
版本：V1.1
版权所有，盗版必究。
Copyright(C) 电脑圈圈 2008-2018
All rights reserved            
*******************************************************************/

#include <reg52.h>

#include "include/UART.h"
#include "include/MyType.h"
#include "include/config.h"


volatile uint8 Sending;


/********************************************************************
函数功能：串口初始化。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void InitUART(void)
{
 EA=0; //暂时关闭中断
 TMOD&=0x0F;  //定时器1模式控制在高4位
 TMOD|=0x20;    //定时器1工作在模式2，自动重装模式
 SCON=0x50;     //串口工作在模式1
 TH1=256-Fclk/(BitRate*12*16);  //计算定时器重装值
 TL1=256-Fclk/(BitRate*12*16);
 PCON|=0x80;    //串口波特率加倍
 ES=1;         //串行中断允许
 TR1=1;        //启动定时器1
 REN=1;        //允许接收 
 EA=1;         //允许中断
}
////////////////////////End of function//////////////////////////////

/********************************************************************
函数功能：串口中断处理。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void UartISR(void) interrupt 4
{
 if(RI)    //收到数据
 {
  RI=0;   //清中断请求
 }
 else      //发送完一字节数据
 {
  TI=0;
  Sending=0;  //清正在发送标志
 }
}
////////////////////////End of function//////////////////////////////

/********************************************************************
函数功能：往串口发送一字节数据。
入口参数：d: 要发送的字节数据。
返    回：无。
备    注：无。
********************************************************************/
void UartPutChar(uint8 d)
{
 SBUF=d; //将数据写入到串口缓冲
 Sending=1;	 //设置发送标志
 while(Sending); //等待发送完毕
}
////////////////////////End of function//////////////////////////////

/********************************************************************
函数功能：发送一个字符串。
入口参数：pd：要发送的字符串指针。
返    回：无。
备    注：无。
********************************************************************/
void Prints(uint8 * pd)
{
 while((*pd)!='\0') //发送字符串，直到遇到0才结束
 {
  UartPutChar(*pd); //发送一个字符
  pd++;  //移动到下一个字符
 }
}
////////////////////////End of function//////////////////////////////

#ifdef DEBUG1

/********************************************************************
函数功能：将整数转按十进制字符串发送。
入口参数：x：待显示的整数。
返    回：无。
备    注：无。
********************************************************************/
void PrintLongInt(uint32 x)
{
 int8 i;
 uint8 display_buffer[10];

 for(i=9;i>=0;i--)
 {
  display_buffer[i]='0'+x%10;
  x/=10;
 }
 for(i=0;i<9;i++)
 {
  if(display_buffer[i]!='0')break;
 }
 for(;i<10;i++)UartPutChar(display_buffer[i]);
}
////////////////////////End of function//////////////////////////////

#endif

code uint8 HexTable[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
/********************************************************************
函数功能：将短整数按十六进制发送。
入口参数：待发送的整数。
返    回：无。
备    注：无。
********************************************************************/
void PrintShortIntHex(uint16 x)
{
 uint8 i;
 uint8 display_buffer[7];
 display_buffer[6]=0;
 display_buffer[0]='0';
 display_buffer[1]='x';
 for(i=5;i>=2;i--) //将整数转换为4个字节的HEX值
 {
  display_buffer[i]=HexTable[(x&0xf)];
  x>>=4;
 }
 Prints(display_buffer);
}
////////////////////////End of function//////////////////////////////

#if (defined DEBUG0)||(defined DEBUG1)
/********************************************************************
函数功能：发送一个byte的数据。
入口参数：待发送的数据。
返    回：无。
备    注：无。
********************************************************************/
void Printc(uint8 x)
{
 Sending=1;
 SBUF=x;
 while(Sending);
}
////////////////////////End of function//////////////////////////////

/********************************************************************
函数功能：以HEX格式发送一个byte的数据。
入口参数：待发送的数据
返    回：无。
备    注：无。
********************************************************************/
void PrintHex(uint8 x)
{
 Printc('0');
 Printc('x');
 Printc(HexTable[x>>4]);
 Printc(HexTable[x&0xf]);
 Printc(' ');
}
////////////////////////End of function//////////////////////////////
#endif