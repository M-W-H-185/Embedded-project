/*
标题：LED红绿灯系统
主频：35MHz
用于：51单片机学习机-学电 
资料：https://rationmcu.com
*/

#include "stc8g.h"
#include "intrins.h"

#include "os_typedef.h"

/* 发光二极管定义 */
sbit LED_R = P0^5;    // 红色LED
sbit LED_Y = P0^6;    // 黄色LED
sbit LED_G = P0^7;    // 绿色LED

/* 固定延时1毫秒 @35MHz*/
void Delay1ms()		//@35MHz 
{
	unsigned char i, j;

	_nop_();
	_nop_();
	i = 46;
	j = 113;
	do
	{
		while (--j);
	} while (--i);
}

/* n毫秒延时函数 参数给几 就延时几毫秒 */
void delay_ms(unsigned int ms) 
{
	while(ms--)
	{
		Delay1ms();
	}
}
os_uint8_t ti0_cut = 0;
void Timer0_Isr(void) interrupt 1
{
	ti0_cut++;
	if(ti0_cut == 253)
	{
		LED_Y = !LED_Y;
		ti0_cut = 0;
	}

}

/* 定时器T0初始化为1ms产生中断 @35MHz */
void Timer0Init(void)		
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x48;		//设置定时初值 65536-35*1000
	TH0 = 0x77;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;    //允许定时器T0溢出中断
	EA = 1;     // 打开总中断
}

/* 主函数 */
void main()
{
	P0M0 = 0x00;   //设置P0.0~P0.7为双向口模式
	P0M1 = 0x00;
	Timer0Init()	;
	EA = 1;
	

	
	
	while(1)
	{

	}
}



