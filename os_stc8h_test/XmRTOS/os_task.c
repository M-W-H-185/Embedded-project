#include "os_task.h"

#include "stc8g.h"
#include "intrins.h"
#include "stdio.h"
extern void PUSH_ALL_STACK(void);
extern void POP_ALL_STACK(void);
enum OS_TASK_STATUS_TYPE
{
	OS_READY      = 1,	// ����̬
	OS_RUNING     = 2,	// ����̬
	OS_BLOCKED    = 3,	// ����̬
	OS_SUSPENDED  = 4	// ֹ̬ͣ

};

// ������ƿ�
typedef struct os_tcb_t
{
	os_uint8_t 			sp;					// sp ��ջָ��洢
	os_uint32_t 		delay_tick;			// ��ʱ�δ���
	os_uint8_t 			os_status_type;		// ����״̬
	os_uint8_t			*stack;				// �����ջ

};

os_uint8_t data task_id = 0;    /*��ǰ������*/
os_uint8_t max_task = 0;

/* ��������б� */
struct os_tcb_t idata tcb_list[MAX_TASKS];
/*���������ջ.*/
os_uint8_t idata task_idle_stack[MAX_TASK_DEP];		


// �����л�����
void os_switch()
{
	os_uint8_t  ost_i = 0;

	EA = 0;//���ж�


//	// ��ջ
//	PUSH_ALL_STACK();
//	// ��ջ

		
	
	tcb_list[task_id].sp = SP;
	// �ҳ�����̬��һ��id
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
	
//	// ��ջ
//	POP_ALL_STACK();
//	// ��ջ
	
	
	
	
	
	EA = 1;//���ж�
}
 
void os_task_create(void(*task)(void) ,os_uint8_t *tstack,int tid)
{
	tstack[0] = 0x00;							// PSW

	tstack[1] = 0x0A;							// ACC
	tstack[2] = 0x0B;  							// B
	tstack[3] = (unsigned int)task & 0xff;		// DPL
	tstack[4] = (unsigned int)task >> 8;  		// DPH

	tstack[5] = 0x00;							// R0
	tstack[6] = 0x01;							// R1						
	tstack[7] = 0x02;							// R2
	tstack[8] = 0x03;							// R3
	tstack[9] = 0x04;							// R4
	tstack[10] = 0x05;							// R5
	tstack[11] = 0x06;							// R6
	tstack[12] = 0x07;							// R7

	
	
	
	tcb_list[tid].sp 				= tstack + 4;	// �����4ʵ���Ͼ���ȡ�˽�taskck[4]�ĵ�ַ�����ˡ�spָ�������൱��ָ����������
	tcb_list[tid].os_status_type 	= OS_READY;
	tcb_list[tid].stack 			= tstack ;
	
	
	
	
	
	
	max_task++;
}
void os_idle_task(void);

void os_start()
{
	EA = 0;//���ж�
	// װ�ؿ�������
	os_task_create(os_idle_task, &task_idle_stack, 0);//��task1����װ��0�Ų�
	
	// ��������������
	task_id = 0;
	SP = tcb_list[task_id].sp;
	

	EA = 1;//���ж�
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

// ������ʱ����
void os_delay(os_uint32_t tasks)
{	
	tasks = tasks;
	// ������ʱ�δ���
	tcb_list[task_id].delay_tick 	 = 	tasks;
	// ����������Ϊ����̬
	tcb_list[task_id].os_status_type = 	OS_BLOCKED;
	// ֻҪ������ʱ�ˣ��������л���ȥ
	os_switch();

}
// ���к���
void os_idle_task(void)
{
	while(1)
	{
		os_switch();
	}
}
void time_handleHook(void)
{
	os_uint8_t ti = 0;

	// �����ﴦ�������ʱ
	for(ti = 0; ti<max_task; ti++)
	{
		if(tcb_list[ti].os_status_type != OS_BLOCKED)
		{
			continue;
		}
		// ������ʱ�δ���
		if((tcb_list[ti].delay_tick - 1) == 0)
		{
			tcb_list[ti].delay_tick = 0;
			// ����������Ϊ����̬
			tcb_list[ti].os_status_type = OS_READY;
			continue;
		}
		tcb_list[ti].delay_tick--;
	}
	
}
