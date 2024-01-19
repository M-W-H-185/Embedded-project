#include "delay.h"
#include "intrins.h"

/* 固定延时1毫秒函数 */
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

/* 延时n毫秒函数 参数是几 就延时几毫秒 */
void delay_ms(unsigned int ms)
{
	while(ms--)
	{
		Delay1ms();
	}
}





