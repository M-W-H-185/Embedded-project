// Ĭ�ϱ�������xdata

#include "stc8g.h"
#include "intrins.h"
#include "stdio.h"

/* ��������ܶ��� */
sbit LED_R = P0^5;    // ��ɫLED
sbit LED_Y = P0^6;    // ��ɫLED
sbit LED_G = P0^7;    // ��ɫLED

typedef     unsigned char    os_uint8_t;	// 8 bits 
typedef     unsigned int     os_uint16_t;	// 16 bits 
typedef     unsigned long    os_uint32_t;	// 32 bits 

typedef unsigned char 	OS_STK;			/* �����ջ��ڿ��Ϊ8λ 					*/



enum OS_TASK_STATUS_TYPE
{
	OS_READY      = 1,	// ����̬
	OS_RUNING     = 2,	// ����̬
	OS_BLOCKED    = 3,	// ����̬
	OS_SUSPENDED  = 4	// ֹ̬ͣ

};

// ������ƿ�
typedef struct os_tcb
{    
	OS_STK          	*OSTCBStkBase;	// ָ�������ջ��ʼ���ֵ�ָ��
	os_uint32_t 		delay_tick;		// ��ʱ�δ���
	os_uint8_t 			os_status;		// ����״̬

} OS_TCB;

#define MAX_TASK_COUNT_NUM 	5			// ���������
#define MAX_STK_SIZE		30	   		// ��������ջ

os_uint8_t 	xdata 	this_taskId = 0;				// ��ǰ���е�����id
OS_TCB 		xdata	tcb_List[MAX_TASK_COUNT_NUM];	// ���TCB��������б�

OS_TCB 		xdata	*tcb_Runing;						// ��ǰ���е��������ָ��


// �û���ջ
OS_STK xdata task_stk_idle[MAX_STK_SIZE] = {0};
OS_STK xdata task_stk_1[MAX_STK_SIZE] = {0};
OS_STK xdata task_stk_2[MAX_STK_SIZE] = {0};
// �û���ջ

// ����������
// *task : ������
// *p_arg: �������Ĵ���
// *task_stack : �����ջ����
// tid : �����id �ڴ��������б�ĵڼ�λ
void os_task_create(
						void	 (*task)(void *p_arg),
						void     *p_arg,
						OS_STK *task_stack, 
						os_uint8_t tid
					)
{
	OS_STK *stk;
	p_arg = p_arg;
	
	stk	  =(OS_STK *)task_stack;			/* �����ջ�����Ч��ַ 								*/
	*stk++=15;								/* �����ջ���� 										*/
	*stk++=(os_uint16_t)task & 0xFF;		/* ��������ַ��8λ 								*/
	*stk++=(os_uint16_t)task >> 8;			/* ��������ַ��8λ 								*/
	/* �������ǰ��ض���˳�򽫼Ĵ��������ջ�ģ������û��ڽ��Ĵ��������ջ��ʱ��ҲҪ������һ˳�� 	*/
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
	/* ���ñ���SP�������л�ʱ�����û���ջ���ȼ���ó� 								*/
	*stk++=(os_uint16_t)(task_stack + MAX_STK_SIZE) >> 8;	/* ?C_XBP �����ջָ���8λ 		*/
	*stk++=(os_uint16_t)(task_stack + MAX_STK_SIZE) & 0xFF;	/* ?C_XBP �����ջ��8λ 			*/
	
	tcb_List[tid].OSTCBStkBase = task_stack;
	tcb_List[tid].os_status = OS_READY;
	tcb_List[tid].delay_tick = 0;
}
extern void OSStartHighRdy(void);
os_uint8_t test = 0;
// ���� OS
void  os_start (void) large reentrant
{
	tcb_Runing = &tcb_List[0];
//	// ���»������!!!
	OSStartHighRdy();

}

void task_idle(void *p_arg ) large reentrant
{
	// �������һ�� ѱ��������
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

