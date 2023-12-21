/*
���⣺LED���̵�ϵͳ
��Ƶ��35MHz
���ڣ�51��Ƭ��ѧϰ��-ѧ�� 
���ϣ�https://rationmcu.com
*/

#include "stc8g.h"
#include "intrins.h"

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
    task_sp[task_id] = SP;
		task_id++;
    if(task_id == max_task)
		{
			task_id = 0;
		}
    SP = task_sp[task_id];
}
 
void task_load(unsigned int fn, int tid)
{
    task_sp[tid] = task_stack[tid]+1;
    task_stack[tid][0] = (unsigned int)fn & 0xff;
    task_stack[tid][1] = (unsigned int)fn >> 8;
    ++max_task;
}
 
void task1()
{
    static unsigned char i;
    while(1)
    {
			i++;
			LED_R = !LED_R;
			delay_ms(100);
			task_switch();
    }
}
 
void task2()
{
    static unsigned char j;
    while(1)
    {
			j+=2;
			LED_Y = !LED_Y;
			delay_ms(100);
			task_switch();
    }
}
 
void switch_to(unsigned int tid)
{
    task_id = tid;
    SP = task_sp[tid];
    return;
}


/* ������ */
void main()
{
	P0M0 = 0x00;   //����P0.0~P0.7Ϊ˫���ģʽ
	P0M1 = 0x00;
	LED_Y = 0;
	task_load(task1, 0);//��task1����װ��0�Ų�
	task_load(task2, 1);//��task2����װ��1�Ų�
	switch_to(0);

		while(1)
	{

	}
	

}



