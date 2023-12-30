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
#include "ringBuffer.h"
/* 发光二极管定义 */

sbit LED_R = P0^5;    // 红色LED
sbit LED_Y = P0^6;    // 黄色LED
sbit LED_G = P0^7;    // 绿色LED




// 任务堆栈区
os_uint8_t xdata task_stack1[MAX_TASK_DEP];			/*任务1堆栈.*/
os_uint8_t xdata task_stack2[MAX_TASK_DEP];			/*任务2堆栈.*/
// 任务堆栈区



void task1()
{
	while(1)
	{
		LED_R = 1;
		os_delay(100);

		LED_R = 0;
		os_delay(100);
		
		LED_R = 1;
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


RingBufferHandle xdata queue_1 ;
os_uint8_t xdata queue_buff[6];

/* 主函数 */
void main()
{
	
	volatile  os_uint8_t test_data = 0xff;
	volatile  os_uint8_t w_data = 0;
	
	// 初始化一个队列
	ringbuffer_created(&queue_1,&queue_buff,6,sizeof(os_uint8_t));
	// 写五次 是成功的
	w_data++;
	ringbuffer_write(&queue_1,&w_data);
	w_data++;
	ringbuffer_write(&queue_1,&w_data);
	w_data++;
	ringbuffer_write(&queue_1,&w_data);
	w_data++;
	ringbuffer_write(&queue_1,&w_data);
	w_data++;
	ringbuffer_write(&queue_1,&w_data);
	// 写五次 是成功的
	
	// 写五次后 失败
	ringbuffer_write(&queue_1,&(os_uint8_t *)6);
	ringbuffer_write(&queue_1,&(os_uint8_t *)7);	
	// 写五次后 失败

	// 读五次 成功
	ringbuffer_read(&queue_1, &test_data);
	ringbuffer_read(&queue_1, &test_data);
	ringbuffer_read(&queue_1, &test_data);
	ringbuffer_read(&queue_1, &test_data);
	ringbuffer_read(&queue_1, &test_data);
	ringbuffer_read(&queue_1, &test_data);
	// 读五次 成功

	// 读五次后 第一次读 失败并让 读和写指正回到数组0
	ringbuffer_read(&queue_1, &test_data);
	// 读五次后 第二次读 读和写重合  缓冲区数据为空
	ringbuffer_read(&queue_1, &test_data);	


	// 这两句后 缓冲区数据为空
	
	// 写一次成功
	w_data = 0x33;
	ringbuffer_write(&queue_1,&w_data);
	// 读一次成功
	ringbuffer_read(&queue_1, &test_data);	
	
	// 这两句后 缓冲区数据为空

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

