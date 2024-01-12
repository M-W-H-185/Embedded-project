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
			EA = 0;
			printf("task1 task1 !!\r\n");
			EA = 1;
			LED_R = !LED_R;
		}
		

		OSCtxSw();	// 最好在任务后面放这个
	}
}

void task2(void)
{

	while(1)
	{
//		struct test_ty test;
//		os_queueRead(&queue_1,&test,10000);	// 队列读取失败 延时100秒
		if(semaphoreTake(&sem_handle2,1000000) == RET_ERROR)
		{

		}
		else
		{
			LED_Y = 1;
			os_delay(100);
			os_delay(100);

			LED_Y = 0;
			os_delay(100);
			os_delay(100);
		}
		
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
				ret = semaphoreGive(&sem_handle2);
//				ret = os_queueSend(&queue_1,&test,0);	// 当队列满后 延时0秒。尽量不要在此延时太长时间，否则写入过快进入阻塞态，读取任务读取完了也进入阻塞态那就很尴尬了
				EA = 0;
				printf("k3 pressk3 pressk3 pressk3 pressk3 pressk3 pressk3 pressk3 pressk3 p\r\n");
				printf("k3 pressk3 pressk3 pressk3 pressk3 pressk3 pressk3 pressk3 pressk3 p\r\n");
				printf("k3 pressk3 pressk3 pressk3 pressk3 pressk3 pressk3 pressk3 pressk3 p\r\n");
				EA = 1;
				LED_G = !LED_G;	
			
			}	
		}
		
		
		OSCtxSw();
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

// 串口初始化函数 可以使用printf
void Uart1Init(void)  //115200bps@11.0592MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0xE8;			//设置定时初始值
	T2H = 0xFF;			//设置定时初始值
	AUXR |= 0x10;		//定时器2开始计时
	TI = 1; // 加这句话可以使用printf
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
	
	Uart1Init();
	Timer0_Init();		// 利用定时器0作为rtos时钟节拍，处理任务延时以及切换
	os_init();			// 将任务数组写入0
	P_SW2 |= (1<<7);
	
	
	// 创建队列
	// os_queueCreate(&queue_1, &queue_buff, 5, sizeof(struct test_ty));

	// 创建一个二值型信号量
	semaphoreCreateBinary(&sem_handle1);
	// 计数型
	semaphoreCreateCount(&sem_handle2,5,0);
	
	os_task_create(task1, &task_stack1, 80, 1);//将task1函数装入1号槽
	os_task_create(task2, &task_stack2, 20, 2);//将task2函数装入2号槽
	os_task_create(task3, &task_stack3, 80, 3);//将task3函数装入3号槽
	printf("os_init success\r\n");
	os_start();


	while(1)
	{

	}
	

}

