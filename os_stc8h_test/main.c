/*
标题：LED红绿灯系统
主频：35MHz
用于：51单片机学习机-学电 
资料：https://rationmcu.com
*/

#include "stc8g.h"
#include "intrins.h"
#include "stdio.h"


/* 发光二极管定义 */
sbit LED_R = P0^5;    // 红色LED
sbit LED_Y = P0^6;    // 黄色LED
sbit LED_G = P0^7;    // 绿色LED
void Delay500ms(void)	//@11.0592MHz
{
	unsigned char data i, j, k;

	_nop_();
	_nop_();
	i = 22;
	j = 3;
	k = 227;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}



typedef     unsigned char    os_uint8_t;	//  8 bits 

typedef     unsigned int     os_uint16_t;	// 16 bits 

typedef     unsigned long    os_uint32_t;	// 32 bits 
enum OS_TASK_STATUS_TYPE
{
	OS_READY      = 1,	// 就绪态
	OS_RUNING     = 2,	// 运行态
	OS_BLOCKED    = 3,	// 阻塞态
	OS_SUSPENDED  = 4	// 停止态

};

// 任务控制块
typedef struct os_tcb_t
{
    os_uint8_t  sp;              // sp 堆栈指针存储
    os_uint32_t delay_tick;      // 延时滴答数
    os_uint8_t  os_status_type;  // 任务状态
} os_tcb_t;
// 任务控制块的结构体大小
const os_uint8_t data OS_TCB_SIZE = sizeof(struct os_tcb_t);	// 1 + 4 + 1 = 6个字节
 
#define MAX_TASKS 5       /*任务槽个数.必须和实际任务数一至*/
#define MAX_TASK_DEP 20   /*最大栈深.最低不得少于2个,保守值为12*/
os_uint8_t data task_id = 0;    /*当前活动任务号*/
os_uint8_t max_task = 0;


 
extern void test(void);
// 任务主动放弃cpu资源
extern void OSCtxSw(void);


// 任务控制列表
struct os_tcb_t idata tcb_list[MAX_TASKS];
// 任务堆栈区
os_uint8_t idata task_idle_stack[MAX_TASK_DEP];		/*空闲任务堆栈.*/
os_uint8_t idata task_stack1[MAX_TASK_DEP];			/*任务1堆栈.*/
os_uint8_t idata task_stack2[MAX_TASK_DEP];			/*任务2堆栈.*/
// 任务堆栈区



/************************************************************************* 	
*				 	获取轮询任务id 无优先级
*************************************************************************/
void OSGetPollTask(void)	
{
	os_uint8_t  ost_i = 0;

	// 找出就绪态的一个id
	task_id ++;
	if(task_id > max_task)
	{
		task_id = 0;
	}
	// task_id++ 的任务不是就绪态 进入循环找一个就绪态的出来
	if(tcb_list[task_id].os_status_type != OS_READY)
	{
		for(ost_i = 0; ost_i < MAX_TASKS; ost_i++)
		{
			if(tcb_list[ost_i].os_status_type == OS_READY)
			{
				task_id = ost_i;
				
			}
	
		}	
	}

	
}


 
void os_task_create(void(*task)(void) ,os_uint8_t *tstack,int tid)
{

	tstack[0] = (unsigned int)task & 0xff;
	tstack[1] = (unsigned int)task >> 8;

	tcb_list[tid].sp 				= tstack+1;
	tcb_list[tid].os_status_type 	= OS_READY;

	max_task++;
}
void os_idle_task(void);

void os_start()
{
	// 装载空闲任务
	os_task_create(os_idle_task, &task_idle_stack, MAX_TASKS - 1);//将task1函数装入 号槽

	task_id = MAX_TASKS - 1;
	SP = tcb_list[task_id].sp;  
	return;
}

// 任务延时函数
void os_delay(os_uint32_t tasks)
{	
	if(tasks > 0 && tcb_list[task_id].os_status_type != OS_BLOCKED)
	{
		// 设置延时滴答数
		tcb_list[task_id].delay_tick 	 = 	tasks;
		// 将任务设置为阻塞态
		tcb_list[task_id].os_status_type = 	OS_BLOCKED;
		
		return;
	}

	
}
os_uint8_t idle_cut = 0;
// 空闲函数
void os_idle_task(void)
{

	while(1)
	{
//		idle_cut = 1 + 1;
//		LED_R = 0;
//		Delay500ms();

//		LED_R = 1;
//		Delay500ms();

	}
}

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
		os_delay(500);
		
		LED_Y = 0;
		os_delay(500);
	}
}


unsigned int cut = 0;

void time0_handle(void)large reentrant
//void time0_handle(void)interrupt 1
{
	os_uint8_t ti = 0;
	cut++;

	if(cut == 500)
	{
		cut = 0;
		LED_G = !LED_G;
	}
	// 在这里处理遍历延时
	for(ti = 0; ti < max_task; ti++)
	{
		if(tcb_list[ti].os_status_type != OS_BLOCKED)
		{
			continue;
		}
		// 设置延时滴答数
		if((tcb_list[ti].delay_tick - 1) == 0)
		{
			tcb_list[ti].delay_tick = 0;
			// 将任务设置为阻塞态
			tcb_list[ti].os_status_type = OS_READY;
			continue;
		}
		tcb_list[ti].delay_tick--;
	}

	
	OSGetPollTask();	// 找一个就绪的任务出来
	// 汇编处理任务切换....
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
	volatile  os_uint8_t aaaa = 0;

/* 主函数 */
void main()
{

	P0M0 = 0x00;   //设置P0.0~P0.7为双向口模式
	P0M1 = 0x00;
	Timer0_Init();
	EA = 1;
	//	P_SW2 |= (1<<7);
	tcb_list[0].sp = 0x01;

	test();


	
	os_task_create(task1, &task_stack1, 0);//将task1函数装入0号槽
	os_task_create(task2, &task_stack2, 1);//将task2函数装入1号槽
	

	
	os_start();


	while(1)
	{

	}
	

}



