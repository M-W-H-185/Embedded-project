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
os_uint8_t ti0_cut = 0;
void Timer0_Isr(void) interrupt 1
{
	ti0_cut++;
	if(ti0_cut == 253)
	{
		LED_Y = !LED_Y;
		ti0_cut = 0;
	}

}

/* ��ʱ��T0��ʼ��Ϊ1ms�����ж� @35MHz */
void Timer0Init(void)		
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0x48;		//���ö�ʱ��ֵ 65536-35*1000
	TH0 = 0x77;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	ET0 = 1;    //����ʱ��T0����ж�
	EA = 1;     // �����ж�
}

/* ������ */
void main()
{
	P0M0 = 0x00;   //����P0.0~P0.7Ϊ˫���ģʽ
	P0M1 = 0x00;
	Timer0Init()	;
	EA = 1;
	

	
	
	while(1)
	{

	}
}



