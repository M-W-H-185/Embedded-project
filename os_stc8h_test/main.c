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
#include "os_queue.h"
/* 发光二极管定义 */

sbit LED_R = P0^5;    // 红色LED
sbit LED_Y = P0^6;    // 黄色LED
sbit LED_G = P0^7;    // 绿色LED

/* 按键引脚定义 */ 
sbit K1 = P3^7;     // 按键K1
sbit K2 = P3^6;     // 按键K2
sbit K3 = P3^5;     // 按键K3


// 任务堆栈区
os_uint8_t xdata task_stack1[MAX_TASK_DEP];			/*任务1堆栈.*/
os_uint8_t xdata task_stack2[MAX_TASK_DEP];			/*任务2堆栈.*/
os_uint8_t xdata task_stack3[MAX_TASK_DEP];			/*任务2堆栈.*/
// 任务堆栈区



void task1()
{
	while(1)
	{

		
		// LED_R = 1;
		os_delay(100);
		

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

typedef struct test_ty{
	os_uint8_t 	k1;
	os_uint16_t k2;
	os_uint32_t k3;
};

QueueHandle xdata queue_1;
struct test_ty xdata queue_buff[5];
void task3()
{
	os_uint8_t ret = 0;
	

		// 创建队列
	os_queueCreate(&queue_1, &queue_buff, 5, sizeof(struct test_ty));
	
	while(1)
	{
		struct test_ty test;
		test.k1 = 0;
		test.k2 = 0;
		test.k3 = 0;
		if(K3 == 0)
		{	
			os_delay(15);
			if(K3 == 0)
			{	
				test.k3 = 1;
				
				
				ret = os_queueSend(&queue_1,&test,10000);
				if(ret = RINGBUFF_SUCCESS)
				{
					LED_G = !LED_G;	
				}
				while(!K3);
			}	
		}
		LED_R = 1;
		os_delay(100);

		LED_R = 0;
		os_delay(100);
//		os_taskSwtich();
		
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

unsigned int cut = 0;
// 函数冲重入，由汇编代码调用
void time0_handle(void) large reentrant
//void time0_handle(void)interrupt 1
{
//	cut++;

//	if(cut == 500)
//	{
//		cut = 0;
//		LED_G = !LED_G;
//	}
	time_handleHook();

}



/* 主函数 */
void main()
{
	EA = 0;
	P0M0 = 0x00;   //设置P0.0~P0.7为双向口模式
	P0M1 = 0x00;
	
	// 将 P3.5、P3.6、P3.7 端口初始化为 准双向模式
	P3M0 = 0x00;// 0000 0000
	P3M1 &= 0x1F;// 0001 1111
	
	
	Timer0_Init();
	os_init();
	P_SW2 |= (1<<7);
	
	os_task_create(task1, &task_stack1, 1);//将task1函数装入1号槽
	os_task_create(task2, &task_stack2, 2);//将task2函数装入2号槽
	os_task_create(task3, &task_stack3, 3);//将task3函数装入3号槽
	
	os_start();


	while(1)
	{

	}
	

}

