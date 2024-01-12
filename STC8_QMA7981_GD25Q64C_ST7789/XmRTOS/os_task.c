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
os_uint8_t xdata task_idle_stack[20];		
void os_idle_task(void);



void os_getTaskIdHighPriority(void)
{
	os_uint8_t  ost_i = 0;
	// 找出就绪态的一个id
	for(ost_i = 0; ost_i < MAX_TASKS; ost_i++)
	{
		
		if(ost_i == task_id)
		{
			continue;
		}
		if( tcb_list[ost_i].os_status_type == OS_READY ) 
		{
			task_id = ost_i;
			break;
		}
	}
}
/*****************************************************************************
*	函数描述：任务调度函数。允许在任务内调用函数
*	传入参数：
*				
* 返回参数：
*				void: 无返回值
*****************************************************************************/
extern void OSCtxSw(void);



/*****************************************************************************
*	函数描述：任务调度函数，除了time_handleHook以外在c函数内禁止调用本函数！！！！
*				如需任务自愿放弃cpu自愿的请使用 OSCtxSw 汇编内的函数
*	传入参数：
*				
* 返回参数：
*				void: 无返回值
*****************************************************************************/
void os_taskSwtich(void)  large reentrant
{
	// 上面中断已经入栈了


	// 已经入栈到了公共堆栈内了
	// 将公共堆栈拷贝到任务堆栈内
	memcpy(tcb_list[task_id].stack, public_stack,tcb_list[task_id].stack_size);
	
	
	tcb_list[task_id].sp = SP;

	os_getTaskIdHighPriority();
	
	// 先将下一个id的sp指向到SP
	SP = tcb_list[task_id].sp;

	// 将任务堆栈拷贝到公共堆栈里面
	memcpy(public_stack,tcb_list[task_id].stack,tcb_list[task_id].stack_size);
	

	
	// 下面中断汇编已经出栈
}


/***********************************************************************
*	函数描述：创建一个任务
*	传入参数：
*				task：		任务函数指针，指向任务的入口函数。
*				t_stack: 	指向任务函数堆栈指针
*				tid:		任务槽位置 按照顺序 
* 返回参数：
*				void: 无返回值
***********************************************************************/
void os_task_create(void(*task)(void), os_uint8_t *t_stack, os_uint8_t stack_size, int tid)
{
	if((max_task+1) >= MAX_TASKS - 1)
	{
		return;
	}
	memset(t_stack, 0, (stack_size * sizeof(os_uint8_t) )  ); // 写入0
	t_stack[0] = (unsigned int)task & 0xff;		// DPL
	t_stack[1] = (unsigned int)task >> 8;  		// DPH

	tcb_list[tid].sp 				= (public_stack+1);	// 全部指向这个公共的堆栈
	tcb_list[tid].os_status_type 	= OS_READY;

	tcb_list[tid].stack = t_stack;
	tcb_list[tid].stack_size = stack_size;
	
	
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
	os_task_create(os_idle_task, &task_idle_stack, 20, MAX_TASKS-1);//将task1函数装入MAX_TASKS-1号槽
	
	// 空闲任务先运行
	task_id = MAX_TASKS-1;
	// 将任务堆栈拷贝到公共堆栈里面
	memcpy(public_stack,tcb_list[task_id].stack,tcb_list[task_id].stack_size);
	
	SP = tcb_list[task_id].sp;
	

	EA = 1;//开中断
	return;
}
void os_init(void)
{
	
	memset(tcb_list, 0, (MAX_TASKS * sizeof(struct os_tcb_t) )  ); // 写入0
	memset(public_stack, 0, (MAX_TASK_DEP * sizeof(os_uint8_t) )  ); // 写入0
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
	OSCtxSw();

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
	while(1)
	{
		
	}
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
	for(ti = 0; ti<MAX_TASKS; ti++)
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
	os_taskSwtich();
}
