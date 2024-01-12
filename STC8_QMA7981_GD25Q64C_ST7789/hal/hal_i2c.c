#include "hal_i2c.h"
#include "stc8g.h"
void I2C_Isr() interrupt 24
{

    if (I2CMSST & 0x40)
    {
        I2CMSST &= ~0x40;                       //清中断标志
    }

}
// 获取空闲状态 0 空闲 1忙碌
uint8_t hal_I2cGetIdleState(void)
{
	// I2CMSST & 0x40 bit6为0的时候就是没执行完，bit6为1的时候为1的时候代表执行完了
	if((I2CMSST & 0x40) == 1)
	{
		I2CMSST &= ~0x40;	// 每次I2CMSCR执行完后都需要软件set0
		return 0;
	}
	else
	{
		return 1;
	}
	return 1;
	
}
// 发送起始信号
uint8_t hal_I2cStart(void)
{
	I2CMSCR = 0X01;
	while(hal_I2cGetIdleState()==1);	// 等待发送成功
	return 0;
}

// 发送停止信号
uint8_t hal_I2cStop(void)
{
	I2CMSCR = 0X06;
	while(hal_I2cGetIdleState()==1);	// 等待发送成功
	return 0;
}
// 发送byte
uint8_t hal_I2cSendByte(uint8_t _data)
{
	I2CTXD = _data; // 填充数据
	I2CMSCR = 0x02;	// 发送数据命令
	while(hal_I2cGetIdleState()==1);	// 等待发送成功
	return 0 ;
}
// 读取一个字节
uint8_t hal_I2cReadByteData(void)
{
		uint8_t temp = 0;
		I2CMSCR = 0x04;	// 读取数据命令
		while(hal_I2cGetIdleState()==1);		// 等待发送成功
		temp = I2CRXD;
		return temp;
}

// 发送一个nack
uint8_t hal_I2cSendNACK(void)
{
	I2CMSST = 0X01;
	I2CMSCR = 0X05;
	while(hal_I2cGetIdleState()==1);	// 等待发送成功
	return 0 ;
}
void hal_I2cRendACK()
{
	I2CMSCR = 0x03;                             //发送读ACK命令
	while(hal_I2cGetIdleState()==1);	// 等待发送成功
}

void hal_I2cInit(void)
{
	P_SW2 |= 0x30;	// bit45写入11 I2C功能叫选择位 
	I2CCFG |= 0xC0;	// 1100 0000 启用i2c设置为主机模式
	I2CCFG |= 0xD;	// 1100 1101 设置总线速度
	I2CMSCR|= 0x80;	// 1000 0000 允许主机中断 
}
