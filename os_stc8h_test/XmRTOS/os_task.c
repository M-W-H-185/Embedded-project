#include "os_task.h"
// ���񴴽����л�����ʱ�ļ�

os_uint8_t data task_id = 0;    /*��ǰ������*/
os_uint8_t max_task = 0;

/* ��������б� */
struct os_tcb_t xdata tcb_list[MAX_TASKS];
/* 
	������ջ
	1���ж�ѹջ�Ὣ����ѹ�������棬�л�����ǰ��Ҫ������ ˽�������ջ ��
	2����ջǰ�Ƚ�SPָ����Ҫ�л���������ƿ��sp�ڣ�Ȼ���ڽ� ˽�������ջ ������������ջ��
*/
static os_uint8_t idata public_stack[MAX_TASK_DEP];

/*���������ջ.*/
os_uint8_t xdata task_idle_stack[MAX_TASK_DEP];		
void os_idle_task(void);


/***********************************************************************
*	����������������Ⱥ���
*	���������
*				
* ���ز�����
*				void: �޷���ֵ
***********************************************************************/
void os_switch(void)
{
	// �����ж��Ѿ���ջ��
	os_uint8_t  ost_i = 0;

	// �Ѿ���ջ���˹�����ջ����
	// ��������ջ�����������ջ��
	memcpy(tcb_list[task_id].stack, public_stack,MAX_TASK_DEP);
	
	
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
	// �Ƚ���һ��id��spָ��SP
	SP = tcb_list[task_id].sp;

	// �������ջ������������ջ����
	memcpy(public_stack,tcb_list[task_id].stack,MAX_TASK_DEP);
	

	
	// �����жϻ���Ѿ���ջ
}

/***********************************************************************
*	��������������һ������
*	���������
*				task��		������ָ�룬ָ���������ں�����
*				t_stack: 	ָ����������ջָ��
*				tid:			�����λ��
* ���ز�����
*				void: �޷���ֵ
***********************************************************************/
void os_task_create(void(*task)(void), os_uint8_t *t_stack, int tid)
{
	if((max_task+1) >= MAX_TASKS)
	{
		return;
	}
	t_stack[0] = (unsigned int)task & 0xff;		// DPL
	t_stack[1] = (unsigned int)task >> 8;  		// DPH

	tcb_list[tid].sp 				= (public_stack+1);	// ȫ��ָ����������Ķ�ջ
	tcb_list[tid].os_status_type 	= OS_READY;

	tcb_list[tid].stack = t_stack;
	
	
	max_task++;
}
/***********************************************************************
*	��������������rtos����ʼ�������
*	���������
*				
* ���ز�����
*				void: �޷���ֵ
***********************************************************************/
void os_start(void)
{
	EA = 0;//���ж�
	// װ�ؿ�������
	os_task_create(os_idle_task, &task_idle_stack, 0);//��task1����װ��0�Ų�
	
	// ��������������
	task_id = 0;
	// �������ջ������������ջ����
	memcpy(public_stack,tcb_list[task_id].stack,MAX_TASK_DEP);
	
	SP = tcb_list[task_id].sp;
	

	EA = 1;//���ж�
	return;
}

/***********************************************************************
*	����������������ʱ���������ú����Ͻ���һ���������
*	���������
*				tasks����Ҫ��ʱ�ĵδ����� �δ�һ��1ms 
* ���ز�����
*				void: �޷���ֵ
***********************************************************************/
void os_delay(os_uint32_t tasks)
{	
	tasks = tasks;
	// ������ʱ�δ���
	tcb_list[task_id].delay_tick 	 = 	tasks;
	// ����������Ϊ����̬
	tcb_list[task_id].os_status_type = 	OS_BLOCKED;
	// ֻҪ������ʱ�ˣ��������������
	os_switch();

}

/***********************************************************************
*	��������������������
*	���������
*				
* ���ز�����
*				void: �޷���ֵ
***********************************************************************/
void os_idle_task(void)
{
	while(1);
}

/***********************************************************************
*	����������ϵͳ�δ��������붨ʱ���ж���ִ�С� ÿ��ִ�д���δ�һ��1ms
*	���������
*				
* ���ز�����
*				void: �޷���ֵ
***********************************************************************/
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
	os_switch();
}
