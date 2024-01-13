/*
���⣺LED���̵�ϵͳ
��Ƶ��35MHz
���ڣ�51��Ƭ��ѧϰ��-ѧ�� 
���ϣ�https://rationmcu.com
*/

#include "stc8g.h"
#include "intrins.h"
#include "stdio.h"
#include "hal_i2c.h"
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
			LED_R = !LED_R;
		}
		OSCtxSw();	// ����������������
	}
}
unsigned char QMA7981Read(unsigned char reg);

void task2(void)
{
	unsigned char xdata id = 0;

	while(1)
	{
		EA = 0;
		id = Read_Byte(0x00);
		printf("id:%x\r\n",id);
		EA = 1;
		
		os_delay(1000);
		LED_Y = 1;
		os_delay(100);
		os_delay(100);

		LED_Y = 0;
		os_delay(100);
		os_delay(100);
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
				LED_G = !LED_G;	
			
			}	
		}
		OSCtxSw();
	}
}



void Timer0_Init(void)		//1����@35MHz
{
	AUXR |= 0x80;			//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;			//���ö�ʱ��ģʽ
	TL0 = 0x48;				//���ö�ʱ��ʼֵ
	TH0 = 0x77;				//���ö�ʱ��ʼֵ
	TF0 = 0;				//���TF0��־
	TR0 = 1;				//��ʱ��0��ʼ��ʱ
	ET0 = 1;				//ʹ�ܶ�ʱ��0�ж�
}

// ���������룬�ɻ��������
void time0_handle(void) large reentrant
{

	time_handleHook();

}

// ���ڳ�ʼ������ ����ʹ��printf
void Uart1Init(void)  //115200bps@35MHz
{
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x01;		//����1ѡ��ʱ��2Ϊ�����ʷ�����
	AUXR |= 0x04;		//��ʱ��ʱ��1Tģʽ
	T2L = 0xB4;			//���ö�ʱ��ʼֵ
	T2H = 0xFF;			//���ö�ʱ��ʼֵ
	AUXR |= 0x10;		//��ʱ��2��ʼ��ʱ
	TI = 1; // ����仰����ʹ��printf
}


/* ������ */
void main()
{

	EA = 0;
	P0M0 = 0x00;                                //����P0.0~P0.7Ϊ˫���ģʽ
	P0M1 = 0x00;
	P3M0 = 0x00;                                //����P3.0~P3.7Ϊ˫���ģʽ
	P3M1 = 0x00;
	

	Uart1Init();		// ��ʼ������1 
	Timer0_Init();		// ���ö�ʱ��0��Ϊrtosʱ�ӽ��ģ�����������ʱ�Լ��л�
	LED_G = 0;

	QMA7981Init();
	//	
	os_init();			// ����������д��0
	// ����һ����ֵ���ź���
	semaphoreCreateBinary(&sem_handle1);
	os_task_create(task1, &task_stack1, 80, 1);//��task1����װ��1�Ų�
	os_task_create(task2, &task_stack2, 20, 2);//��task2����װ��2�Ų�
	os_task_create(task3, &task_stack3, 80, 3);//��task3����װ��3�Ų�
	printf("os_init success\r\n");
	
	
	os_start();
	

	while(1)
	{

	}
	

}

