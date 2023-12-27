/*
���⣺LED���̵�ϵͳ
��Ƶ��35MHz
���ڣ�51��Ƭ��ѧϰ��-ѧ�� 
���ϣ�https://rationmcu.com
*/

#include "stc8g.h"
#include "intrins.h"
#include "stdio.h"


/* ��������ܶ��� */
sbit LED_R = P0^5;    // ��ɫLED
sbit LED_Y = P0^6;    // ��ɫLED
sbit LED_G = P0^7;    // ��ɫLED

typedef     unsigned char    os_uint8_t;	//  8 bits 

typedef     unsigned int     os_uint16_t;	// 16 bits 

typedef     unsigned long    os_uint32_t;	// 32 bits 
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
	};
// ������ƿ�Ľṹ���С
const os_uint8_t data OS_TCB_SIZE = sizeof(struct os_tcb_t);	// 1 + 4 + 1 = 6���ֽ�
 
#define MAX_TASKS 5       /*����۸���.�����ʵ��������һ��*/
#define MAX_TASK_DEP 20   /*���ջ��.��Ͳ�������2��,����ֵΪ12*/
os_uint8_t data task_id = 0;    /*��ǰ������*/
os_uint8_t max_task = 0;


 
extern void test(void);
//// ������������cpu��Դ
//extern void OSCtxSw(void);


// ��������б�
struct os_tcb_t idata tcb_list[MAX_TASKS];
// �����ջ��
os_uint8_t idata task_idle_stack[MAX_TASK_DEP];		/*���������ջ.*/
os_uint8_t idata task_stack1[MAX_TASK_DEP];			/*����1��ջ.*/
os_uint8_t idata task_stack2[MAX_TASK_DEP];			/*����2��ջ.*/
// �����ջ��


// ��ȡ��һ������id
void os_getNextTaskId(void)
{
	
//	
//	
//	

}
// �����л�����
void OSCtxSw()
{
	os_uint8_t  ost_i = 0;
	
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

}
 
void os_task_create(void(*task)(void) ,os_uint8_t *tstack,int tid)
{

	tstack[0] = (unsigned int)task & 0xff;
	tstack[1] = (unsigned int)task >> 8;

	tcb_list[tid].sp 				= tstack + 1;
	tcb_list[tid].os_status_type 	= OS_READY;

	max_task++;
}
void os_idle_task(void);

void os_start()
{
	// װ�ؿ�������
	os_task_create(os_idle_task, &task_idle_stack, 0);//��task1����װ��0�Ų�

	task_id = 0;
	SP = tcb_list[task_id].sp;  
	return;
}

// ������ʱ����
void os_delay(os_uint32_t tasks)
{	
	
	// ������ʱ�δ���
	tcb_list[task_id].delay_tick 	 = 	tasks;
	// ����������Ϊ����̬
	tcb_list[task_id].os_status_type = 	OS_BLOCKED;
	// ֻҪ������ʱ�ˣ��������л���ȥ
	OSCtxSw();
	//	
}
// ���к���
void os_idle_task(void)
{
	while(1)
	{
		OSCtxSw();
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
		OSCtxSw();

	}
}

void task2()
{

	while(1)
	{
		LED_Y = 1;
		os_delay(1000);
		
		LED_Y = 0;
		os_delay(1000);
		OSCtxSw();

	}
}


unsigned int cut = 0;

//void time0_handle(void)large reentrant
void time0_handle(void)interrupt 1
{
	os_uint8_t ti = 0;
	cut++;

	if(cut == 500)
	{
		cut = 0;
		LED_G = !LED_G;
	}
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

void Timer0_Init(void)		//1����@11.0592MHz
{
	AUXR |= 0x80;			//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;			//���ö�ʱ��ģʽ
	TL0 = 0xCD;				//���ö�ʱ��ʼֵ
	TH0 = 0xD4;				//���ö�ʱ��ʼֵ
	TF0 = 0;				//���TF0��־
	TR0 = 1;				//��ʱ��0��ʼ��ʱ
	ET0 = 1;				//ʹ�ܶ�ʱ��0�ж�
}

/* ������ */
void main()
{
	P0M0 = 0x00;   //����P0.0~P0.7Ϊ˫���ģʽ
	P0M1 = 0x00;
	Timer0_Init();
	EA = 1;
	P_SW2 |= (1<<7);

	os_task_create(task1, &task_stack1, 1);//��task1����װ��0�Ų�
	os_task_create(task2, &task_stack2, 2);//��task2����װ��1�Ų�
	os_start();


	while(1)
	{

	}
	

}