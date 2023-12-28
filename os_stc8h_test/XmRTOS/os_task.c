#include "os_task.h"

#include "stc8g.h"
#include "intrins.h"
#include "stdio.h"
extern void push_all_(void);
extern void pop_all_(void);
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
	os_uint8_t 			sp;					// sp 堆栈指针存储
	os_uint32_t 		delay_tick;			// 延时滴答数
	os_uint8_t 			os_status_type;		// 任务状态
};

os_uint8_t data task_id = 0;    /*当前活动任务号*/
os_uint8_t max_task = 0;

// 任务控制列表
struct os_tcb_t idata tcb_list[MAX_TASKS];

os_uint8_t idata task_idle_stack[MAX_TASK_DEP];		/*空闲任务堆栈.*/
//// 获取下一个任务id
//void os_getNextTaskId(void)
//{
//	
////	
////	
////	

//}
// 任务切换函数
	os_uint8_t  ost_i = 0;

void OSCtxSw()
{

	EA = 0;//关中断

	
	
	// 入栈
	// 入栈
	
	
	
	
	
	
	
	tcb_list[task_id].sp = SP;
	// 找出就绪态的一个id
	for(ost_i = 0; ost_i < max_task; ost_i++)
	{
		if(tcb_list[ost_i].os_status_type == OS_READY)
		{
			task_id = ost_i;
			continue;

		}
	}
	if(task_id == max_task)
	{
		task_id = 0;
	}
    SP = tcb_list[task_id].sp;
	
	
	// 出栈
	// 出栈
	
	
	
	
	EA = 1;//开中断
}
 
void os_task_create(void(*task)(void) ,os_uint8_t *tstack,int tid)
{

	tstack[0] = (unsigned int)task & 0xff;
	tstack[1] = (unsigned int)task >> 8;

	
	
	
	tcb_list[tid].sp 				= tstack + 1;	// 这里加一实际上就是取了将taskck[1]的地址保存了。sp指向它就相当于指向了任务函数
	tcb_list[tid].os_status_type 	= OS_READY;

	
	
	
	
	
	
	
	max_task++;
}
void os_idle_task(void);

void os_start()
{
	EA = 0;//关中断
	// 装载空闲任务
	os_task_create(os_idle_task, &task_idle_stack, 0);//将task1函数装入0号槽
	task_id = 0;
	SP = tcb_list[task_id].sp;
	
	// 出栈
	// 出栈
	
	EA = 1;//开中断
	return;
}
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

// 任务延时函数
void os_delay(os_uint32_t tasks)
{	
	tasks = tasks;
	// 设置延时滴答数
	tcb_list[task_id].delay_tick 	 = 	tasks;
	// 将任务设置为阻塞态
	tcb_list[task_id].os_status_type = 	OS_BLOCKED;
	// 只要任务延时了，就马上切换出去
	OSCtxSw();

}
// 空闲函数
void os_idle_task(void)
{
	while(1)
	{
		OSCtxSw();
	}
}
void time_handleHook(void)
{
	os_uint8_t ti = 0;

		// 在这里处理遍历延时
	for(ti = 0; ti<max_task; ti++)
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
	
}
