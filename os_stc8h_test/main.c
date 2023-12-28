/*
标题：LED红绿灯系统
主频：35MHz
用于：51单片机学习机-学电 
资料：https://rationmcu.com
*/

#include "stc8g.h"
#include "intrins.h"
#include "stdio.h"
#include "os_task.h"

/* 发光二极管定义 */
sbit LED_R = P0^5;    // 红色LED
sbit LED_Y = P0^6;    // 黄色LED
sbit LED_G = P0^7;    // 绿色LED




// 任务堆栈区
os_uint8_t idata task_stack1[MAX_TASK_DEP];			/*任务1堆栈.*/
os_uint8_t idata task_stack2[MAX_TASK_DEP];			/*任务2堆栈.*/
// 任务堆栈区



void task1()
{
	while(1)
	{
		LED_R = 1;
		os_delay(500);

		LED_R = 0;
		os_delay(500);
		
		LED_R = 1;
		os_delay(500);

	}
}

void task2()
{

	while(1)
	{
		LED_Y = 1;
		os_delay(1000);
		os_delay(1000);
		
		LED_Y = 0;
		os_delay(1000);
		os_delay(1000);

	}
}


unsigned int cut = 0;

// 函数冲重入，由汇编代码调用
void time0_handle(void)large reentrant
//void time0_handle(void)interrupt 1
{
	cut++;

	if(cut == 500)
	{
		cut = 0;
		LED_G = !LED_G;
	}
	time_handleHook();
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

	os_task_create(task1, &task_stack1, 1);//将task1函数装入0号槽
	os_task_create(task2, &task_stack2, 2);//将task2函数装入1号槽
	os_start();


	while(1)
	{

	}
	

}