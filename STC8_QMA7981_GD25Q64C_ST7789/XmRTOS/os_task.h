#ifndef __OS_TASK_H__
#define __OS_TASK_H__

#include "stc8g.h"
#include "intrins.h"
#include "stdio.h"
#include "string.h"

#define MAX_TASKS 6       /*任务槽个数.必须和实际任务数一至*/
#define MAX_TASK_DEP 80   /*最大栈深.最低不得少于2个,保守值为12*/


typedef     unsigned char    os_uint8_t;	//  8 bits 

typedef     unsigned int     os_uint16_t;	// 16 bits 

typedef     unsigned long    os_uint32_t;	// 32 bits 

enum OS_TASK_STATUS_TYPE
{
	OS_READY      = 1,	// 就绪态
	OS_RUNING     = 2,	// 运行态
	OS_BLOCKED    = 3,	// 阻塞态
	OS_SUSPENDED  = 4		// 停止态

};
#define RET_TRUE   		1
#define RET_FALSE  		0
#define RET_SUCCESS		RET_TRUE
#define RET_ERROR		RET_FALSE

// 任务控制块
typedef struct os_tcb_t
{
	os_uint16_t 			sp;					// sp 堆栈指针存储
	os_uint32_t 			delay_tick;			// 延时滴答数
	os_uint8_t 				os_status_type;		// 任务状态
	os_uint8_t				*stack;				// 任务的私有堆栈
	os_uint8_t 				stack_size;			// 任务的私有堆栈
}os_tcb;

#define OS_DELAY_MAX 4294967295
extern os_uint8_t data task_id;  /*当前活动任务号*/
extern struct os_tcb_t xdata tcb_list[MAX_TASKS];

void os_start(void);
void os_init(void);
extern void OSCtxSw(void);

void os_delay(os_uint32_t tasks);


void os_task_create(void(*task)(void), os_uint8_t *t_stack,os_uint8_t stack_size, int tid);
void os_taskSwtich(void)  large reentrant;
void time_handleHook(void);

#endif

