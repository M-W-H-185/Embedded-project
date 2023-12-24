/*
标题：LED红绿灯系统
主频：35MHz
用于：51单片机学习机-学电 
资料：https://rationmcu.com
*/

#include "stc8g.h"
#include "intrins.h"
#include "stdio.h"

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

 
#define MAX_TASKS 2       /*任务槽个数.必须和实际任务数一至*/
#define MAX_TASK_DEP 20   /*最大栈深.最低不得少于2个,保守值为12*/
os_uint8_t idata task_stack1[MAX_TASK_DEP];/*任务堆栈.*/
os_uint8_t idata task_stack2[MAX_TASK_DEP];/*任务堆栈.*/
unsigned int task_id;    /*当前活动任务号*/
unsigned int max_task = 0;
 
os_uint8_t idata task_sp[MAX_TASKS];

void task_switch()
{

		task_sp[task_id] = SP;
		task_id++;
    if(task_id == max_task)
		{
			task_id = 0;
		}

    SP = task_sp[task_id];

}
 
void task_load(void(*task)(void) ,os_uint8_t *tstack,int tid)
{

	tstack[0] = (unsigned int)task & 0xff;
	tstack[1] = (unsigned int)task >> 8;

	task_sp[tid] = tstack+1;

	++max_task;
}

void task1()
{
		unsigned int task1_i = 0;

    while(1)
    {
			LED_R = !LED_R;
			task1_i ++;
			task1_i / 120;
			delay_ms(500);
			task_switch();
    }
}

void task2()
{

	unsigned int task2_i = 0;

	while(1)
	{

		LED_Y = !LED_Y;
		task2_i ++;
		task2_i / 120;
		task2_i * 120;
		task2_i % 120;

		delay_ms(500);
		task_switch();
	}
}

void os_Start()
{
	task_id = 0;
	SP = task_sp[task_id];  
	return;
}

unsigned int cut = 0;

void Timer0_Isr(void) interrupt 1
{
unsigned int c1ut = 0;
unsigned int c32ut = 0;
unsigned int c2342ut = 0;

	cut++;

	if(cut == 500)
	{
		cut = 0;
		LED_G = !LED_G;
	}
	
}

void Timer0_Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x80;			//定时器时钟1T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0xCD;				//设置定时初始值
	TH0 = 0xD4;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
	ET0 = 1;				//使能定时器0中断
}


/* 主函数 */
void main()
{
	P0M0 = 0x00;   //设置P0.0~P0.7为双向口模式
	P0M1 = 0x00;
	Timer0_Init();
	EA = 1;
	P_SW2 |= (1<<7);
	task_load(task1, &task_stack1, 0);//将task1函数装入0号槽
	task_load(task2, &task_stack2, 1);//将task2函数装入1号槽
	os_Start();


	while(1)
	{

	}
	

}



