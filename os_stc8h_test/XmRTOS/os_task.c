#include "os_task.h"
// 任务创建、切换、延时文件

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
void os_idle_task(void);


/***********************************************************************
*	函数描述：任务调度函数
*	传入参数：
*				
* 返回参数：
*				void: 无返回值
***********************************************************************/
void os_switch(void)
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

/***********************************************************************
*	函数描述：创建一个任务
*	传入参数：
*				task：		任务函数指针，指向任务的入口函数。
*				t_stack: 	指向任务函数堆栈指针
*				tid:			任务槽位置
* 返回参数：
*				void: 无返回值
***********************************************************************/
void os_task_create(void(*task)(void), os_uint8_t *t_stack, int tid)
{
	if((max_task+1) >= MAX_TASKS)
	{
		return;
	}
	t_stack[0] = (unsigned int)task & 0xff;		// DPL
	t_stack[1] = (unsigned int)task >> 8;  		// DPH

	tcb_list[tid].sp 				= (public_stack+1);	// 全部指向这个公共的堆栈
	tcb_list[tid].os_status_type 	= OS_READY;

	tcb_list[tid].stack = t_stack;
	
	
	max_task++;
}
/***********************************************************************
*	函数描述：启动rtos，开始任务调度
*	传入参数：
*				
* 返回参数：
*				void: 无返回值
***********************************************************************/
void os_start(void)
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

/***********************************************************************
*	函数描述：任务延时函数，调用后马上进行一次任务调度
*	传入参数：
*				tasks：需要延时的滴答数。 滴答一次1ms 
* 返回参数：
*				void: 无返回值
***********************************************************************/
void os_delay(os_uint32_t tasks)
{	
	tasks = tasks;
	// 设置延时滴答数
	tcb_list[task_id].delay_tick 	 = 	tasks;
	// 将任务设置为阻塞态
	tcb_list[task_id].os_status_type = 	OS_BLOCKED;
	// 只要任务延时了，就马上任务调度
	os_switch();

}

/***********************************************************************
*	函数描述：空闲任务函数
*	传入参数：
*				
* 返回参数：
*				void: 无返回值
***********************************************************************/
void os_idle_task(void)
{
	while(1);
}

/***********************************************************************
*	函数描述：系统滴答函数，放入定时器中断中执行。 每次执行代表滴答一次1ms
*	传入参数：
*				
* 返回参数：
*				void: 无返回值
***********************************************************************/
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
