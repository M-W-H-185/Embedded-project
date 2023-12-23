/*
���⣺LED���̵�ϵͳ
��Ƶ��35MHz
���ڣ�51��Ƭ��ѧϰ��-ѧ�� 
���ϣ�https://rationmcu.com
*/

#include "stc8g.h"
#include "intrins.h"
#include "stdio.h"

#include "os_typedef.h"

/* ��������ܶ��� */
sbit LED_R = P0^5;    // ��ɫLED
sbit LED_Y = P0^6;    // ��ɫLED
sbit LED_G = P0^7;    // ��ɫLED

/* �̶���ʱ1���� @35MHz*/
void Delay1ms()		//@35MHz 
{
	unsigned char i, j;

	_nop_();
	_nop_();
	i = 46;
	j = 113;
	do
	{
		while (--j);
	} while (--i);
}

/* n������ʱ���� �������� ����ʱ������ */
void delay_ms(unsigned int ms) 
{
	while(ms--)
	{
		Delay1ms();
	}
}

 
#define MAX_TASKS 2       /*����۸���.�����ʵ��������һ��*/
#define MAX_TASK_DEP 20   /*���ջ��.��Ͳ�������2��,����ֵΪ12*/
unsigned char idata task_stack[MAX_TASKS][MAX_TASK_DEP];/*�����ջ.*/
unsigned int task_id;    /*��ǰ������*/
unsigned int max_task = 0;
 
unsigned char idata task_sp[MAX_TASKS];
 
void task_switch()
{
		EA = 0;
	
    task_sp[task_id] = SP;
		task_id++;
    if(task_id == max_task)
		{
			task_id = 0;
		}
    SP = (unsigned int)task_sp[task_id];

		EA = 1;
}
 
void task_load(unsigned int fn, int tid)
{
    task_sp[tid] = (unsigned char)task_stack[tid]+1;
    task_stack[tid][0] = (unsigned int)fn & 0xff;
    task_stack[tid][1] = (unsigned int)fn >> 8;
    ++max_task;
}

void task1()
{
    while(1)
    {
			LED_R = !LED_R;

			delay_ms(1000);

			task_switch();
    }
}

void task2()
{
    while(1)
    {
			LED_Y = !LED_Y;

			delay_ms(1000);

			task_switch();
    }
}
 
void switch_to(unsigned int tid)
{
    task_id = tid;
    SP = (unsigned int)task_sp[tid];
    return;
}

unsigned int cut = 0;

void Timer0_Isr(void) interrupt 1
{
	cut++;
			printf("Timer0_Isr=%d\n",cut);

	if(cut == 500)
	{
		cut = 0;
		LED_G = !LED_G;

	}

	
}
// ���ڳ�ʼ������ ����ʹ��printf
void Uart1Init(void)  //115200bps@35MHz
{
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x01;		//����1ѡ��ʱ��2Ϊ�����ʷ�����
	AUXR |= 0x04;		//��ʱ��ʱ��1Tģʽ
	T2L = 0xE8;			//���ö�ʱ��ʼֵ
	T2H = 0xFF;			//���ö�ʱ��ʼֵ
	AUXR |= 0x10;		//��ʱ��2��ʼ��ʱ
 TI = 1; // ����仰����ʹ��printf
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
	LED_Y = 0;
	Timer0_Init();
	Uart1Init();
	EA = 0;

	task_load(task1, 0);//��task1����װ��0�Ų�
	task_load(task2, 1);//��task2����װ��1�Ų�
	EA = 1;

	switch_to(0);
	
	while(1)
	{

	}
	

}



