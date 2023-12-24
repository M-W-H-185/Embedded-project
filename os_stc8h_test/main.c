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
os_uint8_t idata task_stack1[MAX_TASK_DEP];/*�����ջ.*/
os_uint8_t idata task_stack2[MAX_TASK_DEP];/*�����ջ.*/
unsigned int task_id;    /*��ǰ������*/
unsigned int max_task = 0;
 
os_uint8_t idata task_sp[MAX_TASKS];

void task_switch()
{

		task_sp[task_id] = SP;
		task_id++;
    if(task_id == max_task)
		{
			task_id = 0;
		}

    SP = task_sp[task_id];

}
 
void task_load(void(*task)(void) ,os_uint8_t *tstack,int tid)
{

	tstack[0] = (unsigned int)task & 0xff;
	tstack[1] = (unsigned int)task >> 8;

	task_sp[tid] = tstack+1;

	++max_task;
}

void task1()
{
		unsigned int task1_i = 0;

    while(1)
    {
			LED_R = !LED_R;
			task1_i ++;
			task1_i / 120;
			delay_ms(500);
			task_switch();
    }
}

void task2()
{

	unsigned int task2_i = 0;

	while(1)
	{

		LED_Y = !LED_Y;
		task2_i ++;
		task2_i / 120;
		task2_i * 120;
		task2_i % 120;

		delay_ms(500);
		task_switch();
	}
}

void os_Start()
{
	task_id = 0;
	SP = task_sp[task_id];  
	return;
}

unsigned int cut = 0;

void Timer0_Isr(void) interrupt 1
{
unsigned int c1ut = 0;
unsigned int c32ut = 0;
unsigned int c2342ut = 0;

	cut++;

	if(cut == 500)
	{
		cut = 0;
		LED_G = !LED_G;
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
	task_load(task1, &task_stack1, 0);//��task1����װ��0�Ų�
	task_load(task2, &task_stack2, 1);//��task2����װ��1�Ų�
	os_Start();


	while(1)
	{

	}
	

}



