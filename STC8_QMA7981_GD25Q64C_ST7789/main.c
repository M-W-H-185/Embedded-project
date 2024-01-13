/*
标题：LED红绿灯系统
主频：35MHz
用于：51单片机学习机-学电 
资料：https://rationmcu.com
*/

#include "stc8g.h"
#include "intrins.h"
#include "stdio.h"
#include "hal_i2c.h"
#include "os_task.h"
#include "os_queue.h"
#include "os_semaphore.h"


/* 发光二极管定义 */

sbit LED_R = P0^5;    // 红色LED
sbit LED_Y = P0^6;    // 黄色LED
sbit LED_G = P0^7;    // 绿色LED

/* 按键引脚定义 */ 
sbit K1 = P3^7;     // 按键K1
sbit K2 = P3^6;     // 按键K2
sbit K3 = P3^5;     // 按键K3


// 任务堆
os_uint8_t xdata task_stack1[80] = {0};			/*任务1堆.*/
os_uint8_t xdata task_stack2[20] = {0};			/*任务2堆.*/
os_uint8_t xdata task_stack3[80] = {0};			/*任务2堆.*/
// 任务堆


typedef struct test_ty{
	os_uint8_t 	k1;
	os_uint16_t k2;
	os_uint32_t k3;
};

QueueHandle xdata queue_1;
struct test_ty xdata queue_buff[5];
SemaphoreHandle xdata sem_handle1;

SemaphoreHandle xdata sem_handle2;

void task1(void)
{
	while(1)
	{
		if(semaphoreTake(&sem_handle1,1000000) == RET_ERROR)
		{
		}
		else
		{
			LED_R = !LED_R;
		}
		OSCtxSw();	// 最好在任务后面放这个
	}
}
unsigned char QMA7981Read(unsigned char reg);

void task2(void)
{
	unsigned char xdata id = 0;

	while(1)
	{
		EA = 0;
		id = Read_Byte(0x00);
		printf("id:%x\r\n",id);
		EA = 1;
		
		os_delay(1000);
		LED_Y = 1;
		os_delay(100);
		os_delay(100);

		LED_Y = 0;
		os_delay(100);
		os_delay(100);
		OSCtxSw();
	}
}


void task3(void)
{
	os_uint8_t ret = 0;
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
				while(!K3);
				test.k3 = 1;	
				ret = semaphoreGive(&sem_handle1);
				LED_G = !LED_G;	
			
			}	
		}
		OSCtxSw();
	}
}



void Timer0_Init(void)		//1毫秒@35MHz
{
	AUXR |= 0x80;			//定时器时钟1T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0x48;				//设置定时初始值
	TH0 = 0x77;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
	ET0 = 1;				//使能定时器0中断
}

// 函数冲重入，由汇编代码调用
void time0_handle(void) large reentrant
{

	time_handleHook();

}

// 串口初始化函数 可以使用printf
void Uart1Init(void)  //115200bps@35MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0xB4;			//设置定时初始值
	T2H = 0xFF;			//设置定时初始值
	AUXR |= 0x10;		//定时器2开始计时
	TI = 1; // 加这句话可以使用printf
}


/* 主函数 */
void main()
{

	EA = 0;
	P0M0 = 0x00;                                //设置P0.0~P0.7为双向口模式
	P0M1 = 0x00;
	P3M0 = 0x00;                                //设置P3.0~P3.7为双向口模式
	P3M1 = 0x00;
	

	Uart1Init();		// 初始化串口1 
	Timer0_Init();		// 利用定时器0作为rtos时钟节拍，处理任务延时以及切换
	LED_G = 0;

	QMA7981Init();
	//	
	os_init();			// 将任务数组写入0
	// 创建一个二值型信号量
	semaphoreCreateBinary(&sem_handle1);
	os_task_create(task1, &task_stack1, 80, 1);//将task1函数装入1号槽
	os_task_create(task2, &task_stack2, 20, 2);//将task2函数装入2号槽
	os_task_create(task3, &task_stack3, 80, 3);//将task3函数装入3号槽
	printf("os_init success\r\n");
	
	
	os_start();
	

	while(1)
	{

	}
	

}

