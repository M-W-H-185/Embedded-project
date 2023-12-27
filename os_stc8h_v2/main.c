// 默认变量存入xdata

#include "stc8g.h"
#include "intrins.h"
#include "stdio.h"

/* 发光二极管定义 */
sbit LED_R = P0^5;    // 红色LED
sbit LED_Y = P0^6;    // 黄色LED
sbit LED_G = P0^7;    // 绿色LED

typedef     unsigned char    os_uint8_t;	// 8 bits 
typedef     unsigned int     os_uint16_t;	// 16 bits 
typedef     unsigned long    os_uint32_t;	// 32 bits 

typedef unsigned char 	OS_STK;			/* 定义堆栈入口宽度为8位 					*/



enum OS_TASK_STATUS_TYPE
{
	OS_READY      = 1,	// 就绪态
	OS_RUNING     = 2,	// 运行态
	OS_BLOCKED    = 3,	// 阻塞态
	OS_SUSPENDED  = 4	// 停止态

};

// 任务控制块
typedef struct os_tcb
{    
	OS_STK          	*OSTCBStkBase;	// 指向任务堆栈开始部分的指针
	os_uint32_t 		delay_tick;		// 延时滴答数
	os_uint8_t 			os_status;		// 任务状态

} OS_TCB;

#define MAX_TASK_COUNT_NUM 	5			// 最大任务数
#define MAX_STK_SIZE		30	   		// 任务最大堆栈

os_uint8_t 	xdata 	this_taskId = 0;				// 当前运行的任务id
OS_TCB 		xdata	tcb_List[MAX_TASK_COUNT_NUM];	// 存放TCB任务控制列表

OS_TCB 		xdata	*tcb_Runing;						// 当前运行的任务控制指针


// 用户堆栈
OS_STK xdata task_stk_idle[MAX_STK_SIZE] = {0};
OS_STK xdata task_stk_1[MAX_STK_SIZE] = {0};
OS_STK xdata task_stk_2[MAX_STK_SIZE] = {0};
// 用户堆栈

// 创建任务函数
// *task : 任务函数
// *p_arg: 任务函数的传参
// *task_stack : 任务堆栈数组
// tid : 任务的id 在存入任务列表的第几位
void os_task_create(
						void	 (*task)(void *p_arg),
						void     *p_arg,
						OS_STK *task_stack, 
						os_uint8_t tid
					)
{
	OS_STK *stk;
	p_arg = p_arg;
	
	stk	  =(OS_STK *)task_stack;			/* 任务堆栈最低有效地址 								*/
	*stk++=15;								/* 任务堆栈长度 										*/
	*stk++=(os_uint16_t)task & 0xFF;		/* 任务代码地址低8位 								*/
	*stk++=(os_uint16_t)task >> 8;			/* 任务代码地址高8位 								*/
	/* 处理器是按特定的顺序将寄存器存入堆栈的，所以用户在将寄存器存入堆栈的时候也要依照这一顺序 	*/
	*stk++=0x00;							/* PSW 												*/
	*stk++=0x0A;							/* ACC 												*/
	*stk++=0x0B;							/* B 												*/
	*stk++=0x00;							/* DPL 												*/
	*stk++=0x00;							/* DPH 												*/
	*stk++=0x00;							/* R0 												*/
	*stk++=0x01;							/* R1 												*/
	*stk++=0x02;							/* R2 												*/
	*stk++=0x03;							/* R3 												*/	
	*stk++=0x04;							/* R4 												*/
	*stk++=0x05;							/* R5 												*/
	*stk++=0x06;							/* R6 												*/
	*stk++=0x07;							/* R7 												*/
	/* 不用保存SP，任务切换时根据用户堆栈长度计算得出 								*/
	*stk++=(os_uint16_t)(task_stack + MAX_STK_SIZE) >> 8;	/* ?C_XBP 仿真堆栈指针高8位 		*/
	*stk++=(os_uint16_t)(task_stack + MAX_STK_SIZE) & 0xFF;	/* ?C_XBP 仿真堆栈低8位 			*/
	
	tcb_List[tid].OSTCBStkBase = task_stack;
	tcb_List[tid].os_status = OS_READY;
	tcb_List[tid].delay_tick = 0;
}
extern void OSStartHighRdy(void);
os_uint8_t test = 0;
// 启动 OS
void  os_start (void) large reentrant
{
	tcb_Runing = &tcb_List[0];
//	// 以下汇编启动!!!
	OSStartHighRdy();

}

void task_idle(void *p_arg ) large reentrant
{
	// 互相等于一下 驯服编译器
	p_arg = p_arg;
	while(1)
	{
	
		
	}
}

void task_1(void *p_arg ) large reentrant
{
	p_arg = p_arg;

	while(1)
	{
	
		
	}
}

void task_2(void *p_arg ) large reentrant
{
	p_arg = p_arg;

	while(1)
	{
	
		
	}
}

void main(void)
{

	os_task_create(task_idle,NULL,&task_stk_idle,0);
	os_task_create(task_1,   NULL,&task_stk_1,   1);
	os_task_create(task_2,   NULL,&task_stk_2,   2);
	os_start();
	while(1)
	{
		
	}
}

