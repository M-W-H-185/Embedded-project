/*
���⣺LED���̵�ϵͳ
��Ƶ��35MHz
���ڣ�51��Ƭ��ѧϰ��-ѧ�� 
���ϣ�https://rationmcu.com
*/

#include "stc8g.h"
#include "intrins.h"
#include "stdio.h"
#include "os_task.h"
#include "os_queue.h"
/* ��������ܶ��� */

sbit LED_R = P0^5;    // ��ɫLED
sbit LED_Y = P0^6;    // ��ɫLED
sbit LED_G = P0^7;    // ��ɫLED

/* �������Ŷ��� */ 
sbit K1 = P3^7;     // ����K1
sbit K2 = P3^6;     // ����K2
sbit K3 = P3^5;     // ����K3


// �����ջ��
os_uint8_t xdata task_stack1[MAX_TASK_DEP];			/*����1��ջ.*/
os_uint8_t xdata task_stack2[MAX_TASK_DEP];			/*����2��ջ.*/
os_uint8_t xdata task_stack3[MAX_TASK_DEP];			/*����2��ջ.*/
// �����ջ��



void task1()
{
	while(1)
	{

		
		// LED_R = 1;
		os_delay(100);
		

	}
}

void task2()
{

	while(1)
	{
		LED_Y = 1;
		os_delay(1000);
		os_delay(1000);
		
		LED_Y = 0;
		os_delay(1000);
		os_delay(1000);

	}
}

typedef struct test_ty{
	os_uint8_t 	k1;
	os_uint16_t k2;
	os_uint32_t k3;
};

QueueHandle xdata queue_1;
struct test_ty xdata queue_buff[5];
void task3()
{
	os_uint8_t ret = 0;
	

		// ��������
	os_queueCreate(&queue_1, &queue_buff, 5, sizeof(struct test_ty));
	
	while(1)
	{
		struct test_ty test;
		test.k1 = 0;
		test.k2 = 0;
		test.k3 = 0;
		if(K3 == 0)
		{	
			os_delay(15);
			if(K3 == 0)
			{	
				test.k3 = 1;
				
				
				ret = os_queueSend(&queue_1,&test,10000);
				if(ret = RINGBUFF_SUCCESS)
				{
					LED_G = !LED_G;	
				}
				while(!K3);
			}	
		}
		LED_R = 1;
		os_delay(100);

		LED_R = 0;
		os_delay(100);
//		os_taskSwtich();
		
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

unsigned int cut = 0;
// ���������룬�ɻ��������
void time0_handle(void) large reentrant
//void time0_handle(void)interrupt 1
{
//	cut++;

//	if(cut == 500)
//	{
//		cut = 0;
//		LED_G = !LED_G;
//	}
	time_handleHook();

}



/* ������ */
void main()
{
	EA = 0;
	P0M0 = 0x00;   //����P0.0~P0.7Ϊ˫���ģʽ
	P0M1 = 0x00;
	
	// �� P3.5��P3.6��P3.7 �˿ڳ�ʼ��Ϊ ׼˫��ģʽ
	P3M0 = 0x00;// 0000 0000
	P3M1 &= 0x1F;// 0001 1111
	
	
	Timer0_Init();
	os_init();
	P_SW2 |= (1<<7);
	
	os_task_create(task1, &task_stack1, 1);//��task1����װ��1�Ų�
	os_task_create(task2, &task_stack2, 2);//��task2����װ��2�Ų�
	os_task_create(task3, &task_stack3, 3);//��task3����װ��3�Ų�
	
	os_start();


	while(1)
	{

	}
	

}

