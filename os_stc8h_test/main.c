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

/* ��������ܶ��� */
sbit LED_R = P0^5;    // ��ɫLED
sbit LED_Y = P0^6;    // ��ɫLED
sbit LED_G = P0^7;    // ��ɫLED




// �����ջ��
os_uint8_t idata task_stack1[MAX_TASK_DEP];			/*����1��ջ.*/
os_uint8_t idata task_stack2[MAX_TASK_DEP];			/*����2��ջ.*/
// �����ջ��



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


unsigned int cut = 0;

// ���������룬�ɻ��������
void time0_handle(void)large reentrant
//void time0_handle(void)interrupt 1
{
	cut++;

	if(cut == 500)
	{
		cut = 0;
		LED_G = !LED_G;
	}
	time_handleHook();
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