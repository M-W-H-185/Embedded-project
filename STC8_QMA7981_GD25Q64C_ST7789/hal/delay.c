#include "delay.h"
#include "intrins.h"

/* �̶���ʱ1���뺯�� */
void Delay1ms()		//@35MHz
{
	xdata unsigned char  i, j;

	_nop_();
	_nop_();
	i = 46;
	j = 113;
	do
	{
		while (--j);
	} while (--i);
}

/* ��ʱn���뺯�� �����Ǽ� ����ʱ������ */
void delay_ms(unsigned int ms)
{
	while(ms--)
	{
		Delay1ms();
	}
}





