#include "os_task.h"

#include "stc8g.h"
#include "intrins.h"
#include "stdio.h"
#include "string.h"

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
	os_uint16_t 			sp;					// sp 堆栈指针存储
	os_uint32_t 			delay_tick;			// 延时滴答数
	os_uint8_t 				os_status_type;		// 任务状态
	os_uint8_t				*stack;				// 任务的私有堆栈
};

os_uint8_t data task_id = 0;    /*当前活动任务号*/
os_uint8_t max_task = 0;

/* 任务控制列表 */
struct os_tcb_t xdata tcb_list[MAX_TASKS];
/* 
	公共堆栈
	1、中断压栈会将数据压到这里面，切换任务前需要拷贝到 私有任务堆栈 内
	2、出栈前先将SP指向需要切换的任务控制块的sp内，然后在将 私有任务堆栈 拷贝到公共堆栈中
*/
static os_uint8_t idata public_stack[MAX_TASK_DEP];

/*空闲任务堆栈.*/
os_uint8_t xdata task_idle_stack[MAX_TASK_DEP];		


// 任务切换函数
void os_switch()
{
	// 上面中断已经入栈了
	os_uint8_t  ost_i = 0;

	// 已经入栈到了公共堆栈内了
	// 将公共堆栈拷贝到任务堆栈内
	memcpy(tcb_list[task_id].stack, public_stack,MAX_TASK_DEP);
	
	
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
	// 先将下一个id的sp指向到SP
	SP = tcb_list[task_id].sp;

	// 将任务堆栈拷贝到公共堆栈里面
	memcpy(public_stack,tcb_list[task_id].stack,MAX_TASK_DEP);
	

	
	// 下面中断汇编已经出栈
}
 
void os_task_create(void(*task)(void) ,os_uint8_t *tstack,int tid)
{
	
	tstack[0] = (unsigned int)task & 0xff;		// DPL
	tstack[1] = (unsigned int)task >> 8;  		// DPH

	tcb_list[tid].sp 				= (public_stack+1);	// 全部指向这个公共的堆栈
	tcb_list[tid].os_status_type 	= OS_READY;

	tcb_list[tid].stack = tstack;
	
	
	max_task++;
}
void os_idle_task(void);

void os_start()
{
	EA = 0;//关中断
	// 装载空闲任务
	os_task_create(os_idle_task, &task_idle_stack, 0);//将task1函数装入0号槽
	
	// 空闲任务先运行
	task_id = 0;
	// 将任务堆栈拷贝到公共堆栈里面
	memcpy(public_stack,tcb_list[task_id].stack,MAX_TASK_DEP);
	
	SP = tcb_list[task_id].sp;
	

	EA = 1;//开中断
	return;
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
	os_switch();

}
// 空闲函数
void os_idle_task(void)
{
	while(1);
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
	os_switch();
}
