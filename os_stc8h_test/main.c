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
#include "os_semaphore.h"

/* ��������ܶ��� */

sbit LED_R = P0^5;    // ��ɫLED
sbit LED_Y = P0^6;    // ��ɫLED
sbit LED_G = P0^7;    // ��ɫLED

/* �������Ŷ��� */ 
sbit K1 = P3^7;     // ����K1
sbit K2 = P3^6;     // ����K2
sbit K3 = P3^5;     // ����K3


// �����
os_uint8_t xdata task_stack1[80] = {0};			/*����1��.*/
os_uint8_t xdata task_stack2[20] = {0};			/*����2��.*/
os_uint8_t xdata task_stack3[80] = {0};			/*����2��.*/
// �����


typedef struct test_ty{
	os_uint8_t 	k1;
	os_uint16_t k2;
	os_uint32_t k3;
};

QueueHandle xdata queue_1;
struct test_ty xdata queue_buff[5];
SemaphoreHandle xdata sem_handle1;

SemaphoreHandle xdata sem_handle2;

void task1(void)
{
	while(1)
	{
		if(semaphoreTake(&sem_handle1,1000000) == RET_ERROR)
		{
		}
		else
		{
			EA = 0;
			printf("task1 task1 !!\r\n");
			EA = 1;
			LED_R = !LED_R;
		}
		

		OSCtxSw();	// ����������������
	}
}

void task2(void)
{

	while(1)
	{
//		struct test_ty test;
//		os_queueRead(&queue_1,&test,10000);	// ���ж�ȡʧ�� ��ʱ100��
		if(semaphoreTake(&sem_handle2,1000000) == RET_ERROR)
		{

		}
		else
		{
			LED_Y = 1;
			os_delay(100);
			os_delay(100);

			LED_Y = 0;
			os_delay(100);
			os_delay(100);
		}
		
		OSCtxSw();
	}
}


void task3(void)
{
	os_uint8_t ret = 0;
	

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
				while(!K3);
				test.k3 = 1;	
				ret = semaphoreGive(&sem_handle1);
				ret = semaphoreGive(&sem_handle2);
//				ret = os_queueSend(&queue_1,&test,0);	// ���������� ��ʱ0�롣������Ҫ�ڴ���ʱ̫��ʱ�䣬����д������������̬����ȡ�����ȡ����Ҳ��������̬�Ǿͺ�������
				EA = 0;
				printf("k3 pressk3 pressk3 pressk3 pressk3 pressk3 pressk3 pressk3 pressk3 p\r\n");
				printf("k3 pressk3 pressk3 pressk3 pressk3 pressk3 pressk3 pressk3 pressk3 p\r\n");
				printf("k3 pressk3 pressk3 pressk3 pressk3 pressk3 pressk3 pressk3 pressk3 p\r\n");
				EA = 1;
				LED_G = !LED_G;	
			
			}	
		}
		
		
		OSCtxSw();
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

// ���ڳ�ʼ������ ����ʹ��printf
void Uart1Init(void)  //115200bps@11.0592MHz
{
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x01;		//����1ѡ��ʱ��2Ϊ�����ʷ�����
	AUXR |= 0x04;		//��ʱ��ʱ��1Tģʽ
	T2L = 0xE8;			//���ö�ʱ��ʼֵ
	T2H = 0xFF;			//���ö�ʱ��ʼֵ
	AUXR |= 0x10;		//��ʱ��2��ʼ��ʱ
	TI = 1; // ����仰����ʹ��printf
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
	
	Uart1Init();
	Timer0_Init();		// ���ö�ʱ��0��Ϊrtosʱ�ӽ��ģ�����������ʱ�Լ��л�
	os_init();			// ����������д��0
	P_SW2 |= (1<<7);
	
	
	// ��������
	// os_queueCreate(&queue_1, &queue_buff, 5, sizeof(struct test_ty));

	// ����һ����ֵ���ź���
	semaphoreCreateBinary(&sem_handle1);
	// ������
	semaphoreCreateCount(&sem_handle2,5,0);
	
	os_task_create(task1, &task_stack1, 80, 1);//��task1����װ��1�Ų�
	os_task_create(task2, &task_stack2, 20, 2);//��task2����װ��2�Ų�
	os_task_create(task3, &task_stack3, 80, 3);//��task3����װ��3�Ų�
	printf("os_init success\r\n");
	os_start();


	while(1)
	{

	}
	

}

