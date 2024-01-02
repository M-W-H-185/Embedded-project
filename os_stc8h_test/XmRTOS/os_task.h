#ifndef __OS_TASK_H__
#define __OS_TASK_H__

#include "stc8g.h"
#include "intrins.h"
#include "stdio.h"
#include "string.h"

#define MAX_TASKS 5       /*����۸���.�����ʵ��������һ��*/
#define MAX_TASK_DEP 20   /*���ջ��.��Ͳ�������2��,����ֵΪ12*/


typedef     unsigned char    os_uint8_t;	//  8 bits 

typedef     unsigned int     os_uint16_t;	// 16 bits 

typedef     unsigned long    os_uint32_t;	// 32 bits 

enum OS_TASK_STATUS_TYPE
{
	OS_READY      = 1,	// ����̬
	OS_RUNING     = 2,	// ����̬
	OS_BLOCKED    = 3,	// ����̬
	OS_SUSPENDED  = 4		// ֹ̬ͣ

};

// ������ƿ�
typedef struct os_tcb_t
{
	os_uint16_t 			sp;					// sp ��ջָ��洢
	os_uint32_t 			delay_tick;			// ��ʱ�δ���
	os_uint8_t 				os_status_type;		// ����״̬
	os_uint8_t				*stack;				// �����˽�ж�ջ
}os_tcb;



void os_start(void);
void os_delay(os_uint32_t tasks);
void os_task_create(void(*task)(void), os_uint8_t *t_stack, int tid);
void os_switch(void);
void time_handleHook(void);

#endif

